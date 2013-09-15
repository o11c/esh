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
#ifndef KEY_H
#define KEY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Represents a keypress.
// The value may be Unicode or one of the defines below.
// It also may contain some bits
typedef struct Key Key;
struct Key
{
    uint32_t value;
};

size_t key2bytes(Key k, char b[4]);
size_t bytes2key(const char *b, Key *k);

#define MAX_UNICODE 0x110000

// Notes about special keys:
// - tab is not special, it's '\t' as usual.
// - esc is never generated plain, it is the introducer for specials.
//  However, alt-esc is KEYBIT_ALT | '\e'. rxvt is buggy about this.
// - Not all combinations of keybits can necessarily be pressed.

// Something has gone wrong with the terminal - probably EOF.
#define KEY_ERROR (MAX_UNICODE + 0)
// Something nonfatal has gone wrong with the terminal - redraw needed.
#define KEY_REDRAW (MAX_UNICODE + 1)
#define KEY_BACKSPACE (MAX_UNICODE + 2)
#define KEY_MENU (MAX_UNICODE + 3)
#define KEY_UP (MAX_UNICODE + 4)
#define KEY_DOWN (MAX_UNICODE + 5)
#define KEY_RIGHT (MAX_UNICODE + 6)
#define KEY_LEFT (MAX_UNICODE + 7)
#define KEY_INSERT (MAX_UNICODE + 8)
#define KEY_DELETE (MAX_UNICODE + 9)
#define KEY_HOME (MAX_UNICODE + 10)
#define KEY_END (MAX_UNICODE + 11)
#define KEY_PAGEUP (MAX_UNICODE + 12)
#define KEY_PAGEDOWN (MAX_UNICODE + 13)

// Function keys
#define KEY_F0 0x180000
#define KEY_F(n) (KEY_F0 + n)
#define KEY_IS_F(val) (KEY_NOBITS(val) >= KEY_F0)
#define KEY_F_NO(val) (KEY_NOBITS(val) - KEY_F0)

#define KEY_NOBITS(val) (val & ((1 << 22) - 1))
// Was shift held down for a special key (not for letters)
#define KEYBIT_SHIFT (1 << 22)
// Was ctrl held down for a special key (not for letters)
#define KEYBIT_CTRL (1 << 23)
// Was alt held down, or escape pressed before, any key (including letters)
#define KEYBIT_ALT (1 << 24)
// Was meta held down for a special key (not for letters)
#define KEYBIT_META (1 << 25)
// Was the key part of the numpad? (only if requested)
#define KEYBIT_NUMPAD (1 << 26)

// Mouse support is complicated.
//#define KEYBIT_MOUSE_ (1 << 27)
//#define KEYBIT_MOUSE_ (1 << 28)
//#define KEYBIT_MOUSE_ (1 << 29)
//#define KEYBIT_MOUSE_ (1 << 30)
//#define KEYBIT_MOUSE_ (1 << 31)

enum
{
    RAW_KEY_EXPRESSION_NORM,
    RAW_KEY_EXPRESSION_ESC,
    RAW_KEY_EXPRESSION_CSI,
    RAW_KEY_EXPRESSION_MOUSE,
};
typedef struct RawKeyExpression RawKeyExpression;
struct RawKeyExpression
{
    char type;
    char fin;
    uint16_t arg_mask;
    uint16_t args[16];
};

#endif //KEY_H
