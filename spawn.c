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
#include "spawn.h"

#include <sys/wait.h>

#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static
const char *confstr_path()
{
    size_t n = confstr(_CS_PATH, NULL, 0);
    char *rv = malloc(n);
    confstr(_CS_PATH, rv, n);
    return rv;
}

static
void try_exec(const char *frist, char **args, char **env)
{
    execve(frist, args, env);
    if (errno == ENOEXEC)
    {
        fprintf(stderr, "%s is not really an executable file\n"
                "If you want to run it with the shell, add a shebang\n",
                frist);
    }
}

static
void silly_execvp(const char *frist, char **args, char **env)
{
    if (strchr(frist, '/'))
    {
        try_exec(frist, args, env);
        return;
    }
    static const char *PATH = NULL;
    if (PATH == NULL)
        PATH = getenv("PATH");
    if (PATH == NULL)
        PATH = confstr_path();
    bool checked_cwd = false;
    size_t fl = strlen(frist);
    char *buf = NULL;
    if (*PATH)
    {
        const char *eb = PATH, *ee;
        while (ee = strchrnul(eb, ':'), *ee)
        {
            size_t el = ee - eb;
            buf = realloc(buf, el + !el + 1 + fl + 1);
            char *it;
            if (el)
            {
                checked_cwd |= el == 1 && *eb == '.';
                it = mempcpy(buf, eb, el);
            }
            else
            {
                checked_cwd = true;
                *buf = '.';
                it = buf + 1;
            }
            *it++ = '/';
            strcpy(it, frist);
            try_exec(buf, args, env);
            eb = ee + 1;
        }
    }
    if (!checked_cwd)
    {
        // this (and above) is not *really* necessary, but be consistent
        buf = realloc(buf, 1 + 1 + fl + 1);
        char *it = strcpy(buf, "./") + 2;
        strcpy(it, frist);
        try_exec(buf, args, env);
    }
    free(buf);
}

// Yes, this is copy-pasta from savetty.
// But this is unavoidable since the assignment requires reimplementing
// execvp
int spawn_and_wait(char **argv, char **envp)
{
    // I *think* this is how signals need to be handled.
    sigset_t old_mask, new_mask;
    sigfillset(&new_mask);
    // signals that are too dangerous to play with
    sigdelset(&new_mask, SIGBUS);
    sigdelset(&new_mask, SIGFPE);
    sigdelset(&new_mask, SIGILL);
    sigdelset(&new_mask, SIGSEGV);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    fflush(stderr);
    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "fork: %m\n");
        return 0;
    }
    if (pid == 0)
    {
        // child
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
        silly_execvp(argv[0], argv, envp);
        // failed
        fprintf(stderr, "exec: %s: %m\n", argv[0]);
        _exit(-1);
    }
    // parent
    int status = 0;
    waitpid(pid, &status, 0);
    // TODO consume extra signals here?
    sigprocmask(SIG_SETMASK, &old_mask, NULL);
    return status;
}

void spawn_and_forget(char **argv, char **envp)
{
    fflush(stderr);
    // if we were a real shell we would actually wait later.
    // But I'm not going to write that kind of code without C++.
    pid_t tmp = fork();
    if (tmp == -1)
    {
        fprintf(stderr, "fork: %m\n");
        return;
    }
    if (tmp)
    {
        // not long
        waitpid(tmp, NULL, 0);
        return;
    }
    pid_t child = fork();
    if (child == -1)
    {
        fprintf(stderr, "fork: %m\n");
    }
    if (child)
    {
        // see?
        _exit(0);
    }
    // We are now alone (except for the session stuff. Is esh supposed to
    // set its own session in the first place? I think so).
    silly_execvp(argv[0], argv, envp);
    // failed
    fprintf(stderr, "exec: %s: %m\n", argv[0]);
}
