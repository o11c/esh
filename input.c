#include "input.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "input-impl.h"
#include "mem.h"

int input_open(Input *input, const char *path)
{
    input->_fd = open(path, O_RDONLY);
    if (input->_fd == -1)
        return errno;
    input->_clen = 0;
    return 0;
}

void input_close(Input *input)
{
    close(input->_fd);
    input->_fd = -1;
}

size_t input_getc(Input *input, char *out)
{
    if (!input->_clen)
    {
        ssize_t rv = read(input->_fd, input->_cache, sizeof(input->_cache));
        if (rv == -1)
            return 0;
        if (rv == 0)
            return 0;
        input->_clen = rv;
    }
    *out = input->_cache[0];
    memmove(input->_cache, input->_cache + 1, input->_clen -= 1);
    return 1;
}


char *input_line(Input *input, InputHook hook, void *userdata)
{
    InputBuffer ib;
    ib._cap = 16;
    ib._mem = xmalloc(ib._cap);
    ib._size = 0;
    ib._index = 0;
    while (ib._cap)
    {
        char buf[83];
        const char *expr = buf;
        int bufi = 0;
        if (!input_getc(input, &buf[bufi]))
            expr = NULL;
        // TODO put a loop to handle non-ASCII
        hook(&ib, expr, userdata);
    }
    return ib._mem;
}

void inputhook_stupid(InputBuffer *ib, const char *expr, void *userdata)
{
    assert (userdata == NULL);
    // In case of truncation, reject a partial last line.
    if (expr == NULL)
    {
        inputbuffer_reject(ib);
        return;
    }
    if (*expr == '\x7f')
    {
        inputbuffer_del_before(ib);
        return;
    }
    if (*expr == '\n')
    {
        inputbuffer_accept(ib);
        return;
    }
    inputbuffer_insert(ib, expr);
}
