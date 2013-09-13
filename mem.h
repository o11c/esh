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
