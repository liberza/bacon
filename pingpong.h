#ifndef PINGPONG_H
#define PINGPONG_H

#define WRITE_SUCCESS 0x00
#define CHUNK_FULL 0x01

//! Pingpong buffer
struct 
{
    uint8_t *buf;   //! buffer with size of size*2
    uint16_t size;  //! size of one chunk
    uint16_t i;     //! current byte index for writing
    uint8_t sel;    //! 0: select 1st chunk. 1: select 2nd chunk.
} pingpong_t;

pingpong_t* pingpong_create(uint16_t size //! # bytes in one of two chunks.
                            );

//! Free memory associated with a pingpong_t.
void pingpong_destroy(pingpong_t *p //! pingpong_t to destory
                      );

//! Write a single byte to the pingpong_t.
void pingpong_write(pingpong_t *p,  //! pingpong_t to write to
                    uint8_t sel,    //! write to 1st or 2nd chunk
                    uint16_t i,     //! index to write at
                    uint8_t byte,   //! byte to write
                    );

//! Read an entire chunk from the pingpong_t.
uint8_t* pingpong_read(pingpong_t *p,   //! pingpong_t to read from
                       uint8_t sel      //! read 1st or 2nd chunk
                       );

#endif
