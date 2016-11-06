#include "rbuf.h"
#include <util/atomic.h>

//! Shift the start index ahead by shamt.
void rbuf_shift(volatile rbuf_t *r, uint16_t shamt)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        //FIXME: check that this doesn't pass the end pointer.
        r->start = ((r->start - shamt) % (BUF_SIZE));
        if (r->start < 0)
            r->start += BUF_SIZE;
    }
}

//! Append a value to the buffer, moving the end index ahead by 1
//! This will fill up to BUF_SIZE-1. One address will be empty to
//! keep track of whether the buffer is full or empty.
uint8_t rbuf_append(volatile rbuf_t *r, uint8_t x)
{
    uint16_t new_end;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        new_end = (r->end + 1) % BUF_SIZE;
        if (new_end != r->start)
        {
            r->buf[r->end] = x;
            r->end = new_end;
            return WRITE_SUCCESS;
        }
        else
        {
            return RBUF_FULL;
        }
    }
}

//! Write a value to index i of the buffer.
//! If using this as a queue, never call this.
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

//! Return the length of the buffer, or the distance between
//! the start and end pointers.
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
