#include "xbee.h"
#include "rbuf.h"
#include "status.h"
#include "serial.h"
#include <avr/io.h>
#include <util/delay.h>

volatile rbuf_t rbuf;
uint8_t func_code = 0x00;
uint8_t err_code = 0x00;

const static struct frame_types_t FRAME_TYPES =
{
    .AT             = (uint8_t)0x08,
    .AT_QPV         = (uint8_t)0x09,
    .TX             = (uint8_t)0x10,
    .EXPLICIT_TX    = (uint8_t)0x11,
    .REMOTE         = (uint8_t)0x17,
    .AT_RESP        = (uint8_t)0x88,
    .MODEM_STATUS   = (uint8_t)0x8A,
    .TX_STATUS      = (uint8_t)0x8B,
    .ROUTE_INFO     = (uint8_t)0x8D,
    .RX             = (uint8_t)0x90,
    .EXPLICIT_RX    = (uint8_t)0x91,
    .NODE_ID        = (uint8_t)0x95,
    .REMOTE_RESP    = (uint8_t)0x97
};

const static struct special_bytes_t SPECIAL_BYTES =
{
    .FRAME_DELIM = (uint8_t)0x7E,
    .ESCAPE      = (uint8_t)0x7D,
    .XON         = (uint8_t)0x11,
    .XOFF        = (uint8_t)0x13
};

void xbee_init()
{
    rbuf.start = 0;
    rbuf.end = 0;
    TX_INT_ENABLE();
    RX_INT_ENABLE();
}

uint8_t tx(uint8_t *data, uint8_t dsize, uint64_t dest, uint8_t opts)
{
    uint8_t frame[BUF_SIZE];

    // TX frame has 14 bytes overhead
    // does not include delimiter or length
    uint8_t fsize = dsize + 14;
    uint8_t sum = 0;

    // delim + len_high + len_low + fsize + checksum
    // must fit in the buffer.
    if ((fsize + 5) > BUF_SIZE)
        return FRAME_SIZE_ERR;

    frame[0] = 0x7E;
    frame[1] = (uint8_t)(fsize >> 8);
    frame[2] = (uint8_t)(fsize);
    frame[3] = FRAME_TYPES.TX;
    frame[4] = 0x01;
    frame[5] = (uint8_t)(dest >> 56);
    frame[6] = (uint8_t)(dest >> 48);
    frame[7] = (uint8_t)(dest >> 40);
    frame[8] = (uint8_t)(dest >> 32);
    frame[9] = (uint8_t)(dest >> 24);
    frame[10] = (uint8_t)(dest >> 16);
    frame[11] = (uint8_t)(dest >> 8);
    frame[12] = (uint8_t)(dest);
    frame[13] = 0xFF;   // reserved
    frame[14] = 0xFE;   // reserved
    frame[15] = 0x00;   // broadcast radius (default 0x00 for radius=max hops)
    frame[16] = opts;   // tx options (probably should just be 0x00)

    // compute first part of checksum.
    for (int i=3; i<17; i++)
    {
        sum += frame[i];
    }

    // append data and sum it
    for (int i=0; i<dsize; i++)
    {
        frame[17 + i] = data[i];
        sum += data[i];
    }
    // put checksum at the end
    frame[fsize + 4] = sum;

    // send it
    for (int i=0; i < fsize + 5; i++)
        put_byte(frame[i]);

    return 0;
}

//! rx(frame) assumes frame has BUF_SIZE bytes allocated already.
//! DO NOT use this if frame is unallocated.
uint8_t rx(uint8_t *frame)
{
    uint8_t ret;
    // Add timeout here.
    do
    {
        ret = find_frame(&rbuf, frame);
        status(ret);
    }
    while (ret != 0);
    return ret;
}

//! Checks the receive buffer for any potential frames. 
//! Try validation, and then shift out of the buffer if validated.
//! It is important that no interrupts call rbuf_shift()
//! while this function is executing.
uint8_t find_frame(volatile rbuf_t *r, uint8_t *frame)
{
    uint16_t buf_len;
    uint16_t i;
    uint8_t ret;
    // Check that the first byte is a frame delimiter.
    // If not, shift out bytes until we hit one.
    if (rbuf_read(r, 0) != SPECIAL_BYTES.FRAME_DELIM)
    {
        // Find the first frame delimiter.
        for (i=1; i<BUF_SIZE; i++)
        {
            if (rbuf_read(r, i) == SPECIAL_BYTES.FRAME_DELIM)
                break;
        }
        // This will shift up to the frame delimiter, or shift
        // everything out if one was not found.
        rbuf_shift(r, i);
    }

    if (rbuf_read(r, 0) == SPECIAL_BYTES.FRAME_DELIM)
    {
        buf_len = rbuf_len(r);

        for (int i=0; i < buf_len; i++)
        {
            frame[i] = rbuf_read(r, i);
            put_byte(frame[i]);
        }

        unescape(frame, BUF_SIZE);
        ret =  validate_frame(frame, BUF_SIZE);
    }
    else
    {
        // could not find frame delimiter.
        ret = -2;
    }
    return ret;
}

//! Loops through the frame, unescaping any escaped bytes.
//! Could be done in find_frame and save a loop, but let's see if
//! that's necessary before premature optimization...
void unescape(uint8_t *frame, uint16_t size)
{
    uint16_t i = 1;
    uint16_t j;
    // stop if we reach the end of the array. 
    while (i < size)
    {
        // Check that we reached an escape byte.
        if (frame[i] == SPECIAL_BYTES.ESCAPE)
        {
            j = i;
            // shift the right side of byte array to the left.
            while (j+1 < size)
            {
                frame[j] = frame[j+1];
                j++;
            }

            // Unescape the character.
            frame[i] = frame[i] ^ 0x20;
        }
        i++;
    }
}

uint8_t escape(uint8_t *frame, uint16_t size)
{
    uint16_t i = 1;
    uint16_t j;

    while (i < size)
    {
        if (frame[i] == SPECIAL_BYTES.FRAME_DELIM ||
            frame[i] == SPECIAL_BYTES.ESCAPE      ||
            frame[i] == SPECIAL_BYTES.XON         ||
            frame[i] == SPECIAL_BYTES.XOFF)
        {
            if (i + 1 == size) // can't add an escape char, buffer full
                return FRAME_SIZE_ERR;
            j = size - 1;
            // shift right side of array to the right.
            while (j > i)
            {
                frame[j] = frame[j - 1];
                j--;
            }
            frame[i] = SPECIAL_BYTES.ESCAPE;
            frame[i+1] = frame[i+1] ^ 0x20;
            i++; // make sure we increment i twice in this case.
        }
        i++;
    }
    return 0;
}

//! Check the checksum. 
uint8_t validate_frame(uint8_t *frame, uint16_t size)
{
    uint8_t ret = 0;
    uint8_t sum = 0;
    uint16_t len;
    len = ((uint16_t)frame[1] << 8) | frame[2];
    if (len >= BUF_SIZE)
    {
        return FRAME_SIZE_ERR;
    }
    else if (frame[0] != SPECIAL_BYTES.FRAME_DELIM)
    {
        return FRAME_DELIM_ERR;
    }
    else
    {
        // Sum the bytes after the length.
        for (int i=3; i<len; i++)
        {
            sum += frame[i];
        }
        // Make sure they add to 0xFF, including the checksum
        if (sum != 0xFF)
        {
            return FRAME_SUM_ERR;
        }
    }
    return ret;
}

ISR(USART_RX_vect)
{
    uint8_t tmp = UDR0;
    rbuf_append(&rbuf, tmp);
}

