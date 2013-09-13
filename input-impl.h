#ifndef INPUT_IMPL_H
#define INPUT_IMPL_H

#include <stddef.h>

// Structures and functions needed to implement an input hook

typedef struct InputBuffer InputBuffer;
// Contents of a line while it is being fed.
struct InputBuffer
{
    char *_mem; // start of malloc'ed memory
    size_t _cap; // size of allocation
    size_t _size; // size of useful data
    size_t _index; // position of input cursor
};

void inputbuffer_accept(InputBuffer *);
void inputbuffer_reject(InputBuffer *);
void inputbuffer_expand(InputBuffer *, size_t);
void inputbuffer_erase(InputBuffer *, size_t);

size_t input_expr_len(const char *expr);

void inputbuffer_insert(InputBuffer *, const char *);
void inputbuffer_del_after(InputBuffer *);
void inputbuffer_del_before(InputBuffer *);

#endif //INPUT_IMPL_H
