#ifndef RBUF_H
#define RBUF_H

#include <stdint.h>
#include <stdlib.h>

#define BUF_SIZE 256

#define WRITE_SUCCESS 0x00
#define BUF_FULL 0x01

typedef struct {
    uint8_t buf[BUF_SIZE];
    uint16_t start;
    uint16_t end;
} rbuf_t;

uint8_t rbuf_append(rbuf_t *r,
                    uint8_t x);

void rbuf_write(rbuf_t *r,
                uint8_t x,
                uint16_t i);

uint8_t rbuf_read(rbuf_t *r,
                  uint16_t i);

void rbuf_shift(rbuf_t *r,
                int shamt);

uint16_t rbuf_len(volatile rbuf_t *r);


#endif
