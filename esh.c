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
#include "input.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

__attribute__((noreturn))
static
void die(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

__attribute__((noreturn))
static
void edie(int err, const char *msg)
{
    errno = err;
    fprintf(stderr, "%s: %m\n", msg);
    exit(1);
}

int main(int argc, char **argv __attribute__((unused)))
{
    char *line;
    int err;

    if (argc != 1)
        die("This program takes no options");

    Input tty;
    err = input_open(&tty, "/dev/tty");
    if (err)
        edie(err, "Failed to open TTY");

    while ((line = input_line(&tty, inputhook_stupid, (void *)NULL)))
    {
        printf("Got line >>> %s <<<\n", line);
        free(line);
    }
    puts("Got EOF");
    input_close(&tty);
}
