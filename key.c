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

#include "key.h"

size_t key2bytes(Key k, char b[4])
{
    if (/*0x00 <= k.value &&*/ k.value < 0x80)
    {
        b[0] = k.value;
        b[1] = '\0';
        b[2] = '\0';
        b[3] = '\0';
        return 1;
    }
    if (0x80 <= k.value && k.value < 0x800)
    {
        b[0] = ((k.value >> 6) & 0x1f) | 0xc0;
        b[1] = ((k.value >> 0) & 0x3f) | 0x80;
        b[2] = '\0';
        b[3] = '\0';
        return 2;
    }
    if (0x800 <= k.value && k.value < 0x10000)
    {
        b[0] = ((k.value >> 12) & 0x0f) | 0xe0;
        b[1] = ((k.value >> 6) & 0x3f) | 0x80;
        b[2] = ((k.value >> 0) & 0x3f) | 0x80;
        b[3] = '\0';
        return 3;
    }
    if (0x10000 <= k.value && k.value < 0x110000)
    {
        b[0] = ((k.value >> 18) & 0x07) | 0xf0;
        b[1] = ((k.value >> 12) & 0x3f) | 0x80;
        b[2] = ((k.value >> 6) & 0x3f) | 0x80;
        b[3] = ((k.value >> 0) & 0x3f) | 0x80;
        return 4;
    }
    return 0;
}

size_t bytes2key(const char *b, Key *k)
{
    if (*b & 0x80)
    {
        if (!(*b & 0x40))
            // illegal trailing char
            return 0;
        if (!(*b & 0x20))
        {
            // 110
            if ((b[1] & 0xc0) != 0x80)
                return false;
            k->value = ((b[0] & 0x1f) << 6) | ((b[1] & 0x3f) << 0);
            return 2;
        }
        if (!(*b & 0x10))
        {
            // 1110
            if ((b[1] & 0xc0) != 0x80)
                return false;
            if ((b[2] & 0xc0) != 0x80)
                return false;
            k->value = ((b[0] & 0x0f) << 12) | ((b[1] & 0x3f) << 6) | ((b[2] & 0x3f) << 0);
            return 3;
        }
        if (!(*b & 0x08))
        {
            // 1111 0
            if ((b[1] & 0xc0) != 0x80)
                return false;
            if ((b[2] & 0xc0) != 0x80)
                return false;
            if ((b[3] & 0xc0) != 0x80)
                return false;
            k->value = ((b[0] & 0x07) << 18) | ((b[1] & 0x3f) << 12) | ((b[2] & 0x3f) << 6) | ((b[2] & 0x3f) << 0);
            return 4;
        }
        return 0;
    }
    if (*b == '\x7f')
    {
        k->value = KEY_BACKSPACE;
        return 1;
    }
    if (*b == 'D' - '@')
    {
        k->value = KEY_ERROR;
        return 1;
    }
    if (*b == 'R' - '@')
    {
        k->value = KEY_REDRAW;
        return 1;
    }
    if (*b != '\e')
    {
        k->value = *b;
        return 1;
    }
    // TODO handle escape sequences ...
    return 0;
}
