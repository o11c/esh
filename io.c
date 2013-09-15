// Nacreous EggShell is a shell with no major limitations nor features.
// Copyright (C) 2013  Ben Longbons <b.r.longbons@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "io.h"

#include <termios.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "input-impl.h"
#include "key.h"
#include "mem.h"

int io_open_tty(IO *io, const char *path)
{
    int fd = open(path, O_RDWR | O_CLOEXEC);
    if (fd == -1)
        return errno;
    io->_read_fd = fd;
    io->_write_fd = fd;
    io->_clen = 0;
    return 0;
}

void io_close(IO *io)
{
    close(io->_read_fd);
    if (io->_write_fd != io->_read_fd)
        close(io->_write_fd);
    io->_read_fd = -1;
    io->_write_fd = -1;
}

static
size_t input_getc(IO *io, char *out)
{
    if (!io->_clen)
    {
        ssize_t rv = read(io->_read_fd, io->_cache, sizeof(io->_cache));
        if (rv == -1)
            return 0;
        if (rv == 0)
            return 0;
        io->_clen = rv;
    }
    *out = io->_cache[0];
    memmove(io->_cache, io->_cache + 1, io->_clen -= 1);
    return 1;
}

// semi-blocking version
// will wait for at least one byte to arrive, then return true if it is complete.
static
bool input_try(IO *io, Key *key)
{
    char c;
    if (!input_getc(io, &c))
    {
        key->value = KEY_ERROR;
        return true;
    }
    key->value = (unsigned char)c;
    return c != '\e';
}

// TODO actually support multichar sequences here (using bytes2key())

// fully blocking version
static
Key input_get(IO *io)
{
    while (true)
    {
        Key key;
        if (input_try(io, &key))
            return key;
    }
}

static
void io_write(IO *io, const char *s, size_t len)
{
    while (true)
    {
        ssize_t l = write(io->_write_fd, s, len);
        if (l == -1)
            // the terminal is probably dead, no point making an error message
            abort();
        if ((size_t)l == len)
            return;
        s += l;
        len -= l;
    }
}

static
void really_input_line(InputBuffer *ib, const char *prompt, IO *io, InputHook hook, void *userdata)
{
    size_t prompt_len, prompt_width = 0;
    for (prompt_len = 0; prompt[prompt_len]; ++prompt_len)
    {
        unsigned char c = prompt[prompt_len];
        if ((c & 0xc0) == 0x80)
            continue;
        if (c == '\r' || c == '\n')
        {
            prompt_width = 0;
            continue;
        }
        if (c == '\e')
        {
            c = prompt[++prompt_len];
            if (c == ']' || c == '^' || c == '_') // maybe also 'P'
            {
                while (true)
                {
                    c = prompt[++prompt_len];
                    if (c < ' ')
                        break;
                }
                continue;
            }
            if (c == '[')
            {
                while (true)
                {
                    c = prompt[++prompt_len];
                    if ('@' <= c && c <= '~')
                        break;
                }
                continue;
            }
        }
        //if (' ' <= c && c <= '~')
        {
            prompt_width++;
            continue;
        }
    }

    bool redraw = true;
    while (ib->_cap)
    {
        if (redraw)
        {
            redraw = false;
            // TODO actually reset the line
            io_write(io, prompt, prompt_len);
        }
        Key key = input_get(io);
        if (key.value == KEY_REDRAW)
        {
            redraw = true;
            continue;
        }

        hook(io, ib, key, userdata);
    }
}

char *input_line(const char *prompt, IO *io, InputHook hook, void *userdata)
{
    struct termios saved_termios, raw_termios;
    bool has_term = tcgetattr(io->_read_fd, &saved_termios) == 0;
    if (has_term)
    {
        if (saved_termios.c_cc[VERASE] != '\x7f')
            has_term = false;
    }
    if (has_term)
    {
        memcpy(&raw_termios, &saved_termios, sizeof(struct termios));
        cfmakeraw(&raw_termios);
        tcsetattr(io->_read_fd, TCSADRAIN, &raw_termios);
    }

    InputBuffer ib;
    ib._cap = 16;
    ib._mem = xmalloc(ib._cap);
    ib._size = 0;
    ib._index = 0;

    if (has_term)
        io_write(io, "\e[1m", 4);

    really_input_line(&ib, prompt, io, hook, userdata);

    if (has_term)
        io_write(io, "\e[0m", 4);

    if (has_term)
        tcsetattr(io->_read_fd, TCSADRAIN, &saved_termios);
    return ib._mem;
}


void inputhook_stupid(IO *io, InputBuffer *ib, Key expr, void *userdata)
{
    assert (userdata == NULL);
    // In case of truncation, reject a partial last line.
    if (expr.value == KEY_ERROR)
    {
        inputbuffer_reject(ib);
        io_write(io, "\n", 1);
        return;
    }
    if (expr.value == '\x7f')// KEY_BACKSPACE)
    {
        if (ib->_index)
        {
            unsigned char to_erase = ib->_mem[ib->_index - 1];
            if (to_erase < 0x20)
                io_write(io, "\b\b  \b\b", 6);
            else
                io_write(io, "\b \b", 3);
            inputbuffer_del_before(ib);
        }
        return;
    }
    if (expr.value == '\n' || expr.value == '\r')
    {
        inputbuffer_accept(ib);
        io_write(io, "\r\n", 2);
        return;
    }
    char buf[4];
    size_t n = key2bytes(expr, buf);
    inputbuffer_insert(ib, buf, n);
    if (expr.value < 0x20)
    {
        buf[0] = '^';
        buf[1] = expr.value + '@';
        n = 2;
    }
    io_write(io, buf, n);
}
