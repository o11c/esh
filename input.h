#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

typedef struct Input Input;
// class for an input source, often a tty
struct Input
{
    int _fd;
    // Small buffer for read(2) - user is not likely to type a lot at once.
    // Theoretically, we need up to 2 + 16*4 + 15*1 + 1 = 82, but I have
    // never seen a keystroke that long in the wild. And the code will be
    // correct in any case.
    char _cache[11];
    unsigned char _clen;
};

// return an errno value
int input_open(Input *, const char *);
void input_close(Input *);

typedef struct InputBuffer InputBuffer;

typedef void (*InputHook)(InputBuffer *, const char *expr, void *userdata);

// Read until the hook indicates 'accept' or 'eof'.
// Typically, this happens when 'newline' is pressed.
// The result is malloced, or NULL on EOF.
char *input_line(Input *input, InputHook hook, void *userdata);

// Basic implementation of an input hook, just handles backspace and enter.
// The userdata must be NULL.
void inputhook_stupid(InputBuffer *, const char *, void *);

#endif //INPUT_H
