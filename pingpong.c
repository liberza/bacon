
pingpong_t* pingpong_create(uint16_t size)
{
    // Need error handling for OOM
    pingpong_t *p = malloc(sizeof(pingpong_t));
    p->size = size;
    p->sel = 0;
    p->buf = calloc(r->size*2, sizeof(uint8_t));
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
    // This works even if p->sel is set to something that
    // doesn't make sense. This is intentional.
    if (p->sel == 0) p->sel = 1;
    else p->sel = 0;
}

uint8_t pingpong_write(pingpong_t *p, uint8_t byte)
{
    p->buf[(p->sel*(p->size)) + (p->i)] = byte;
    if (p->i + 1 < p->size)
    {
        p->i++;
        return WRITE_SUCCESS;
    }
    else
    {
        return CHUNK_FULL;
    }
}

uint8_t pingpong_read(pingpong_t *p, uint16_t i)
{
    return p->buf[(p->sel*(p->size)) + (p->i)];
}

