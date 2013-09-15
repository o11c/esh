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
#include "input-impl.h"

#include <string.h>

#include "mem.h"

void inputbuffer_accept(InputBuffer *ib)
{
    ib->_mem = xrealloc(ib->_mem, ib->_size + 1);
    ib->_mem[ib->_size] = '\0';
    ib->_cap = 0;
}

void inputbuffer_reject(InputBuffer *ib)
{
    free(ib->_mem);
    ib->_mem = NULL;
    ib->_cap = 0;
}

void inputbuffer_expand(InputBuffer *ib, size_t delta)
{
    if (ib->_size + delta > ib->_cap)
        ib->_mem = xrealloc(ib->_mem, ib->_cap *= 2);
    size_t old_foo = ib->_index;
    size_t new_foo = old_foo + delta;
    if (ib->_size != ib->_index)
        memmove(ib->_mem + new_foo, ib->_mem + old_foo, ib->_size - ib->_index);
    ib->_size += delta;
}

void inputbuffer_erase(InputBuffer *ib, size_t delta)
{
    ib->_size -= delta;
    size_t new_foo = ib->_index;
    size_t old_foo = new_foo + delta;
    if (ib->_size != ib->_index)
        memmove(ib->_mem + new_foo, ib->_mem + old_foo, ib->_size - ib->_index);
    if (ib->_size * 3 + 16 < ib->_cap)
        ib->_mem = xrealloc(ib->_mem, ib->_cap /= 2);
}


void inputbuffer_insert(InputBuffer *ib, const char *expr, size_t n)
{
    inputbuffer_expand(ib, n);
    memcpy(ib->_mem + ib->_index, expr, n);
    ib->_index += n;
}

void inputbuffer_del_after(InputBuffer *ib)
{
    inputbuffer_erase(ib, 1);
}

void inputbuffer_del_before(InputBuffer *ib)
{
    ib->_index--;
    inputbuffer_erase(ib, 1);
}
