#include "rbuf.h"

//! Shift the start index ahead by shamt.
void rbuf_shift(volatile rbuf_t *r, uint16_t shamt)
{
    r->start = ((r->start - shamt) % (BUF_SIZE));
    if (r->start < 0)
        r->start += BUF_SIZE;
}

//! Append a value to the buffer, moving the end index ahead by 1
//! and moving the start index ahead by 1 if necessary.
uint8_t rbuf_append(volatile rbuf_t *r, uint8_t x)
{
    uint8_t ret = 0;
    if (r->end + 1 != r->start)
    {
        r->end = (r->end + 1) % BUF_SIZE;
        r->buf[r->end] = x;
    }
    else
    {
        ret = RBUF_FULL;
    }
    return ret;
}

//! Write a value to index i of the buffer.
void rbuf_write(volatile rbuf_t *r, uint8_t x, uint16_t i)
{
    r->buf[(r->start + i) % BUF_SIZE] = x;
}

//! Read a value from index i in the buffer.
uint8_t rbuf_read(volatile rbuf_t *r, uint16_t i)
{
    return r->buf[(r->start + i) % r->len];
}
