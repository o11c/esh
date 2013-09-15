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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "input-impl.h"
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


char *input_line(const char *prompt, IO *io, InputHook hook, void *userdata)
{
    write(io->_write_fd, prompt, strlen(prompt));
    InputBuffer ib;
    ib._cap = 16;
    ib._mem = xmalloc(ib._cap);
    ib._size = 0;
    ib._index = 0;
    while (ib._cap)
    {
        char buf[83];
        const char *expr = buf;
        int bufi = 0;
        if (!input_getc(io, &buf[bufi]))
            expr = NULL;
        // TODO put a loop to handle non-ASCII
        hook(&ib, expr, userdata);
    }
    return ib._mem;
}

void inputhook_stupid(InputBuffer *ib, const char *expr, void *userdata)
{
    assert (userdata == NULL);
    // In case of truncation, reject a partial last line.
    if (expr == NULL)
    {
        inputbuffer_reject(ib);
        return;
    }
    if (*expr == '\x7f')
    {
        inputbuffer_del_before(ib);
        return;
    }
    if (*expr == '\n')
    {
        inputbuffer_accept(ib);
        return;
    }
    inputbuffer_insert(ib, expr);
}
