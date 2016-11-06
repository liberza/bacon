#include "xbee.h"
#include "rbuf.h"
#include "status.h"
#include "serial.h"
#include <avr/io.h>
#include <util/delay.h>

volatile struct rbuf_t rbuf;

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
    rbuf->start = 0;
    rbuf->end = 0;
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
uint8_t* rx(uint8_t *frame)
{
    // Add timeout here.
    frame[0] == '\0';
    while (frame[0] == '\0')
        frame = find_frame(&rbuf, frame);

    frame = validate_frame(frame);
    return frame;
}

//! Checks the receive buffer for any potential frames. 
//! Pass the output of this function to validate_frame()
uint8_t* find_frame(volatile rbuf_t *r, uint8_t *frame)
{
    uint16_t frame_len;
    uint16_t buf_len;
    uint16_t i;
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

    if (rbuf_read(r, 0) != SPECIAL_BYTES.FRAME_DELIM)
    {
        // could not find frame delimiter.
        return frame;
    }

    // Length, besides delimiter, length and checksum.
    frame_len = ((uint16_t)(rbuf_read(rbuf, i+1) << 8) | rbuf_read(rbuf, i+2)) + 4;
    buf_len = rbuf_len(rbuf);

    if (buf_len >= frame_len)
    {
        // There are enough bytes to make this frame. Fill frame and return.
        for (int i=0; i < frame_len; i++)
        {
            frame[i] = rbuf_read(r, i);
        }
        rbuf_shift(r, frame_len);
    }

    return frame;
}

//! Loops through the frame, unescaping any escaped bytes.
//! Could be done in find_frame and save a loop, but let's see if
//! that's necessary before premature optimization...
void unescape(uint8_t *bytes, uint16_t size)
{
    uint16_t i = 0;
    uint16_t j;
    // Stop if we reach the end of the null-terminated array,
    // or if we reach the end of the array. Nulls are from calloc,
    // not the serial port.
    while ((bytes[i] != '\0') && (i < size))
    {
        // Check that we reached an escape byte.
        if (bytes[i] == SPECIAL_BYTES.ESCAPE)
        {
            j = i;
            // shift the right side of byte array to the left.
            while ((bytes[j] != '\0') && (j+1 < size))
            {
                bytes[j] = bytes[j+1];
                j++;
            }
            // Set last byte to null.
            // FIXME: Should be bytes[j] = '\0' right?
            bytes[size - 1] = '\0';

            // Unescape the character.
            bytes[i] = bytes[i] ^ 0x20;
        }
        i++;
    }
}

uint8_t escape(uint8_t *bytes, uint16_t size)
{
    uint16_t i = 0;
    uint16_t j;

    while ((bytes[i] != '\0') && (i < size))
    {
        if (bytes[i] == SPECIAL_BYTES.ESCAPE    ||
            bytes[i] == SPECIAL_BYTES.XON       ||
            bytes[i] == SPECIAL_BYTES.XOFF)
        {
            if (i + 1 == size) // can't add an escape char, buffer full
                return FRAME_SIZE_ERR;
            j = size - 1;
            // shift right side of array to the right.
            while (j > i)
            {
                bytes[j] = bytes[j - 1];
                j--;
            }
            bytes[i] = SPECIAL_BYTES.ESCAPE;
            bytes[i+1] = bytes[i+1] ^ 0x20;
            i++; // make sure we increment i twice in this case.
        }
        i++;
    }
    return 0;
}

uint8_t validate_frame(uint8_t *bytes)
{
    uint8_t ret = 0;
    uint8_t sum = 0;
    uint16_t len;
    len = ((uint16_t)bytes[1] << 8) | bytes[2];
    else if (bytes[0] != SPECIAL_BYTES.FRAME_DELIM)
    {
        return FRAME_DELIM_ERR;
    }
    else
    {
        // Sum the bytes after the length.
        for (int i=3; i<len; i++)
        {
            sum += bytes[i];
        }
        // Make sure they add to 0xFF, including the checksum
        if (sum == 0xFF)
        {
            //ret = len;
            ret = 0;
        }
        else
        {
            return FRAME_SUM_ERR;
        }
    }
    return ret;
}

ISR(USART_RX_vect)
{
    uint8_t tmp = UDR0;
    if (tmp == SPECIAL_BYTES.FRAME_DELIM)
    {
        rx_buf->ready = 1;
        pingpong_swap(rx_buf);
    }
    pingpong_write(rx_buf, tmp);
}

