#include "pingpong.h"

pingpong_t* pingpong_create(uint16_t size)
{
    // Need error handling for OOM
    pingpong_t *p = malloc(sizeof(pingpong_t));
    p->size = size;
    p->wsel = 0;
    p->rsel = 1;
    p->ready = 0;
    p->buf = calloc(p->size*2, sizeof(uint8_t));
    return p;
}

void pingpong_destroy(pingpong_t *p)
{
    free(p->buf);
    p->buf = NULL;
    free(p);
    p = NULL;
}

void pingpong_swap(pingpong_t *p)
{
    if (p->wsel == 0)
    {
        p->wsel = 1;
        p->rsel = 0;
    }
    else 
    {
        p->wsel = 0;
        p->rsel = 1;
    }
    p->i = 0;
}

uint8_t pingpong_write(pingpong_t *p, uint8_t byte)
{
    if (p->i < p->size)
    {
        p->buf[(p->wsel*(p->size)) + (p->i)] = byte;
        p->i++;
        return WRITE_SUCCESS;
    }
    else
    {
        return CHUNK_FULL;
    }
}

uint8_t* pingpong_chunk_ptr(pingpong_t *p)
{
    return (uint8_t*)(&(p->buf) + (p->rsel)*(p->size));
}

uint8_t pingpong_read(pingpong_t *p, uint16_t i)
{
    return p->buf[(p->rsel*(p->size)) + (p->i)];
}
