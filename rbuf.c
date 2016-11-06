#include "rbuf.h"
#include <util/atomic.h>

//! Shift the start index ahead by shamt.
void rbuf_shift(volatile rbuf_t *r, uint16_t shamt)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        r->start = ((r->start - shamt) % (BUF_SIZE));
        if (r->start < 0)
            r->start += BUF_SIZE;
    }
}

//! Append a value to the buffer, moving the end index ahead by 1
//! and moving the start index ahead by 1 if necessary.
uint8_t rbuf_append(volatile rbuf_t *r, uint8_t x)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint8_t ret = 0;
        if (r->end + 1 != r->start)
        {
            r->end = (r->end + 1) % BUF_SIZE;
            r->buf[r->end] = x;
            ret = WRITE_SUCCESS;
        }
        else
        {
            ret = RBUF_FULL;
        }
        return ret;
    }
}

//! Write a value to index i of the buffer.
void rbuf_write(volatile rbuf_t *r, uint8_t x, uint16_t i)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        r->buf[(r->start + i) % BUF_SIZE] = x;
    }
}

//! Read a value from index i in the buffer.
uint8_t rbuf_read(volatile rbuf_t *r, uint16_t i)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        return r->buf[(r->start + i) % r->len];
    }
}

uint16_t rbuf_len(volatile rbuf_t *r)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (rbuf->start <= rbuf->end)
            return rbuf->end - rbuf->start;
        else
            return BUF_SIZE - rbuf->start + rbuf->end;
    }
}
