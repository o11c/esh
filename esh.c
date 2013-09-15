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

#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "spawn.h"

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

static
char **split_line(char *line)
{
    size_t alloc = 0;
    for (size_t i = 0; line[i]; ++i)
    {
        unsigned char c = line[i];
        if (isspace(c))
            alloc++;
    }
    char **words = malloc((alloc + 2) * sizeof(char *));
    size_t wi = 0;
    // TODO handle quoting
    while (line += strspn(line, " \f\n\r\t\v"), *line)
    {
        words[wi++] = line;
        line += strcspn(line, " \f\n\r\t\v");
        if (*line)
            *line++ = '\0';
    }
    words[wi] = NULL;
    return words;
}

static
void builtin_cd(const char *arg1)
{
    if (arg1 == NULL)
        arg1 = getenv("HOME");
    if (arg1 == NULL)
        fprintf(stderr, "$HOMEless!\n");
    else
        chdir(arg1);
}

int main(int argc, char **argv __attribute__((unused)), char **envp)
{
    if (argc != 1)
        die("This program takes no options");

    IO tty;
    int err = io_open_tty(&tty, "/dev/tty");
    if (err)
        edie(err, "Failed to open TTY");

    int exit_value = 0;
    for (char *line; (line = input_line("esh> ", &tty, inputhook_stupid, (void *)NULL)); free(line))
    {
        size_t len = strlen(line);
        bool background = len && line[len - 1] == '&';
        if (background)
        {
            line[len - 1] = '\0';
            if (!*line)
                continue;
        }
        char **lines = split_line(line);
        if (*lines)
        {
            if (strcmp(lines[0], "exit") == 0)
            {
                if (lines[1])
                    exit_value = atoi(lines[1]);
                free(lines);
                free(line);
                break;
            }
            else if (strcmp(lines[0], "cd") == 0)
                builtin_cd(lines[1]);
            else if (background)
                spawn_and_forget(lines, envp);
            else
                spawn_and_wait(lines, envp);
        }
        free(lines);
    }
    puts("");
    io_close(&tty);
    return exit_value;
}
