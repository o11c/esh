#include "input-impl.h"

#include <string.h>

#include "mem.h"

void inputbuffer_accept(InputBuffer *ib)
{
    ib->_mem = xrealloc(ib->_mem, ib->_size + 1);
    ib->_mem[ib->_size] = '\0';
    ib->_cap = 0;
}

void inputbuffer_reject(InputBuffer *ib)
{
    free(ib->_mem);
    ib->_mem = NULL;
    ib->_cap = 0;
}

void inputbuffer_expand(InputBuffer *ib, size_t delta)
{
    if (ib->_size + delta > ib->_cap)
        ib->_mem = xrealloc(ib->_mem, ib->_cap *= 2);
    size_t old_foo = ib->_index;
    size_t new_foo = old_foo + delta;
    if (ib->_size != ib->_index)
        memmove(ib->_mem + new_foo, ib->_mem + old_foo, ib->_size - ib->_index);
    ib->_size += delta;
}

void inputbuffer_erase(InputBuffer *ib, size_t delta)
{
    ib->_size -= delta;
    size_t new_foo = ib->_index;
    size_t old_foo = new_foo + delta;
    if (ib->_size != ib->_index)
        memmove(ib->_mem + new_foo, ib->_mem + old_foo, ib->_size - ib->_index);
    if (ib->_size * 3 + 16 < ib->_cap)
        ib->_mem = xrealloc(ib->_mem, ib->_cap /= 2);
}


size_t input_expr_len(const char *expr)
{
    // TODO return 2-4 for utf-8 chars
    // TODO return arbitrary amounts for special keys
    return 1;
}


void inputbuffer_insert(InputBuffer *ib, const char *expr)
{
    size_t n = input_expr_len(expr);
    inputbuffer_expand(ib, n);
    memcpy(ib->_mem + ib->_index, expr, n);
    ib->_index += n;
}

void inputbuffer_del_after(InputBuffer *ib)
{
    inputbuffer_erase(ib, 1);
}

void inputbuffer_del_before(InputBuffer *ib)
{
    ib->_index--;
    inputbuffer_erase(ib, 1);
}
