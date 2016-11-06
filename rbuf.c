#include "rbuf.h"

//! Shift the start index ahead by shamt.
void rbuf_shift(rbuf_t *r, uint8_t shamt)
{
    r->start = ((r->start - shamt) % (BUF_SIZE));
    if (r->start < 0)
        r->start += BUF_SIZE;
}

//! Append a value to the buffer, moving the end index ahead by 1
//! and moving the start index ahead by 1 if necessary.
void rbuf_append(rbuf_t *r, uint8_t x)
{
    r->end = (r->end + 1) % BUF_SIZE;
    if (r->start == r->end)
    {
        r->start = (r->start + 1) % BUF_SIZE;
    }
    r->buf[r->end] = x;
}

//! Write a value to index i of the buffer.
void rbuf_write(rbuf_t *r, uint8_t x, uint16_t i)
{
    r->buf[(r->start + i) % BUF_SIZE] = x;
}

//! Read a value from index i in the buffer.
uint8_t rbuf_read(rbuf_t *r, uint16_t i)
{
    return r->buf[(r->start + i) % r->len];
}
