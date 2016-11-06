#ifndef PINGPONG_H
#define PINGPONG_H

#include <stdint.h>
#include <stdlib.h>

#define WRITE_SUCCESS 0x00
#define CHUNK_FULL 0x01

#define BUF_SIZE 256

//! Pingpong buffer
typedef struct pingpong_t
{
    uint8_t buf1[BUF_SIZE];
    uint8_t buf2[BUF_SIZE];
    uint8_t *rbuf;  // current read buffer
    uint8_t *wbuf;  // current write buffer
    uint16_t i;     //! current byte index for pushing to wbuf
    uint8_t sel;    //! 0: 
    uint8_t ready;  //! 0: write chunk ready to be read 1: not ready
} pingpong_t;

pingpong_t* pingpong_create(uint16_t size //! # bytes in one of two chunks.
                            );

//! Free memory associated with a pingpong_t.
void pingpong_destroy(pingpong_t *p //! pingpong_t to destory
                      );

//! Switch rsel and wsel
void pingpong_swap(pingpong_t *p    //! pingpong_t for swapping
                   );

//! Write a single byte to the pingpong_t.
uint8_t pingpong_write(pingpong_t *p,  //! pingpong_t to write to
                       uint8_t byte    //! byte to write
                    );

//! Return the index of the start of the chunk to be read.
uint8_t* pingpong_chunk_ptr(pingpong_t *p   //! pingpong_t to read from
                            );

//! Read one byte from selected chunk in pingpong_t, at index i.
uint8_t pingpong_read(pingpong_t *p,   //! pingpong_t to read from
                      uint16_t i       //! index to read at, within selected chunk
                       );

#endif
