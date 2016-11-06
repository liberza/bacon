#ifndef RBUF_H
#define RBUF_H

#define BUF_SIZE 256

typedef struct {
    uint8_t buf[BUF_SIZE];
    uint16_t start;
    uint16_t end;
} rbuf_t;

void rbuf_append(rbuf_t *r,
                 uint8_t x);

void rbuf_write(rbuf_t *r,
                uint8_t x,
                uint16_t i);

uint8_t rbuf_read(rbuf_t *r,
                  uint16_t i);

void rbuf_shift(rbuf_t *r,
                int shamt);
