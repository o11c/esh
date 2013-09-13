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
#include <sys/wait.h>

#include <termios.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((noreturn))
void usage()
{
    fprintf(stderr, "Usage: savetty program args...\n");
    fprintf(stderr, "savetty takes no options of its own.\n");
    exit(0);
}

__attribute__((noreturn))
void edie(const char *msg)
{
    fprintf(stderr, "%s: %m\n", msg);
    exit(1);
}

int spawn_and_wait(char **argv)
{
    pid_t pid = fork();
    if (pid == -1)
        edie("fork");
    if (pid == 0)
    {
        // child
        execvp(argv[0], argv);
        // failed
        edie("exec");
    }
    // parent
    int status = 0;
    wait(&status);
    return status;
}

int main(int argc, char **argv)
{
    if (argc == 1 || argv[1][0] == '-')
        usage();
    int tty = open("/dev/tty", O_RDWR);
    if (tty == -1)
        edie("open /dev/tty");
    struct termios saved;
    if (tcgetattr(tty, &saved) == -1)
        edie("tcgetattr");
    int status = spawn_and_wait(argv + 1);
    if (tcsetattr(tty, TCSAFLUSH, &saved) == -1)
        edie("tcgetattr");
    close(tty);
    if (WIFSIGNALED(status))
    {
        fprintf(stderr, "%s%s\n", strsignal(WTERMSIG(status)), WCOREDUMP(status) ? " (core dumped)" : "");
        return 128 + WTERMSIG(status);
    }

    return WEXITSTATUS(status);
}
