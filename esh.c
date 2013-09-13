#include "input.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

__attribute__((noreturn))
void die(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

__attribute__((noreturn))
void edie(int err, const char *msg)
{
    errno = err;
    fprintf(stderr, "%s: %m\n", msg);
    exit(1);
}

int main(int argc, char **argv)
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
