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
#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

typedef struct IO IO;
// class for an input source, often a tty
struct IO
{
    int _read_fd, _write_fd;
    unsigned char _clen;
    // Small buffer for read(2) - user is not likely to type a lot at once.
    // Theoretically, we need up to 2 + 16*4 + 15*1 + 1 = 82, but I have
    // never seen a keystroke that long in the wild. And the code will be
    // correct in any case.
    char _cache[11];
};

// return an errno value
int io_open_tty(IO *, const char *);
void io_close(IO *);

typedef struct InputBuffer InputBuffer;

typedef void (*InputHook)(InputBuffer *, const char *expr, void *userdata);

// Read until the hook indicates 'accept' or 'eof'.
// Typically, this happens when 'newline' is pressed.
// The result is malloced, or NULL on EOF.
char *input_line(const char *prompt, IO *io, InputHook hook, void *userdata);

// Basic implementation of an input hook, just handles backspace and enter.
// The userdata must be NULL.
void inputhook_stupid(InputBuffer *, const char *, void *);

#endif //INPUT_H
