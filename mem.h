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
#ifndef MEM_H
#define MEM_H

#include <stdlib.h>

static inline
void *xmalloc(size_t n)
{
    void *p = malloc(n);
    if (!p)
        abort();
    return p;
}

static inline
void *xrealloc(void *ptr, size_t n)
{
    void *p = realloc(ptr, n);
    if (!p)
        abort();
    return p;
}

#endif //MEM_H
