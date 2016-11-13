#include "rbuf.h"
#include <util/atomic.h>

//! Shift the start index ahead by shamt.
void rbuf_shift(volatile rbuf_t *r, uint16_t shamt)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        //FIXME: check that this doesn't pass the end pointer.
        r->start = ((r->start - shamt) % (MAX_BUF_SIZE));
        if (r->start < 0)
            r->start += MAX_BUF_SIZE;
    }
}

//! Append a value to the buffer, moving the end index ahead by 1
//! This will fill up to MAX_BUF_SIZE-1. One address will be empty to
//! keep track of whether the buffer is full or empty.
uint8_t rbuf_append(volatile rbuf_t *r, uint8_t x)
{
    uint16_t new_end;
    uint8_t ret = 3;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        new_end = (r->end + 1) % MAX_BUF_SIZE;
        if (new_end != r->start)
        {
            r->buf[r->end] = x;
            r->end = new_end;
            ret = RBUF_WRITE_SUCCESS;
        }
        else
        {
            ret = RBUF_FULL;
        }
    }
    return ret;
}

//! Read a value from index i in the buffer.
uint8_t rbuf_read(volatile rbuf_t *r, uint16_t i)
{
    uint8_t byte;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // FIXME: can read beyond r->end.
        byte = r->buf[(r->start + i) % MAX_BUF_SIZE];
    }
    return byte;
}

//! Return the length of the buffer, or the distance between
//! the start and end pointers.
uint16_t rbuf_len(volatile rbuf_t *r)
{
    uint16_t len;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (r->start <= r->end)
            len = r->end - r->start;
        else
            len = MAX_BUF_SIZE - r->start + r->end; 
    }
    return len;
}
