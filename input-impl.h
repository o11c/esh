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
#ifndef INPUT_IMPL_H
#define INPUT_IMPL_H

#include <stddef.h>

// Structures and functions needed to implement an input hook

typedef struct InputBuffer InputBuffer;
// Contents of a line while it is being fed.
struct InputBuffer
{
    char *_mem; // start of malloc'ed memory
    size_t _cap; // size of allocation
    size_t _size; // size of useful data
    size_t _index; // position of input cursor
};

void inputbuffer_accept(InputBuffer *);
void inputbuffer_reject(InputBuffer *);
void inputbuffer_expand(InputBuffer *, size_t);
void inputbuffer_erase(InputBuffer *, size_t);

size_t input_expr_len(const char *expr);

void inputbuffer_insert(InputBuffer *, const char *);
void inputbuffer_del_after(InputBuffer *);
void inputbuffer_del_before(InputBuffer *);

#endif //INPUT_IMPL_H
