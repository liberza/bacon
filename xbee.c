#include "xbee.h"
#include "pingpong.h"
#include <avr/io.h>

pingpong_t *rx_buf;

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
    rx_buf = pingpong_create(BUF_SIZE);
    TX_INT_ENABLE();
    RX_INT_ENABLE();
}

uint8_t rx()
{
    uint16_t len;       // actual frame length
    uint8_t *rx_chunk;  // pointer to the "read" chunk of rx_buf
    if (rx_buf->ready)
    {
        rx_buf->ready = 0;
        rx_chunk = pingpong_chunk_ptr(rx_buf);
        unescape(rx_chunk, rx_buf->size);
        len = validate_frame(rx_chunk, rx_buf->size);
        return len;
    }
    return 0;
}

void unescape(uint8_t *bytes, uint16_t size)
{
    uint16_t i = 0;
    uint16_t j;
    // Stop if we reach the end of the null-terminated array,
    // or if we reach the end of the array.
    while ((bytes[i] != '\0') && (i < size))
    {
        // Check that we reached an escape byte.
        if (bytes[i] == SPECIAL_BYTES.ESCAPE)
        {
            j = i;
            // shift the right side of byte array to the left.
            while ((j != '\0') && (j+1 < size))
            {
                bytes[j] = bytes[j+1];
                j++;
            }
            // Set last byte to null in case the chunk was full.
            bytes[size - 1] = '\0';

            // Unescape the character.
            bytes[i] = bytes[i] ^ 0x20;
            i++;
        }
    }
}

uint8_t escape(uint8_t *bytes, uint16_t size)
{
    return 0;
}

uint16_t validate_frame(uint8_t *bytes, uint16_t size)
{
    uint16_t ret = 0;
    uint8_t sum = 0;
    uint16_t len;
    if (size < 5)
    {
        // too small to be a frame, early return
    }
    len = (bytes[1] << 8) | bytes[2];
    // verify length
    if (len >= size)
    {
        // error
    }
    else if (bytes[0] != SPECIAL_BYTES.FRAME_DELIM)
    {
        // error
    }
    else
    {
        // Sum the bytes after the length.
        for (int i=3; i<len; i++)
        {
            sum += bytes[i];
        }
        // Make sure they add to 0xFF, including the checksum
        if (sum != 0xFF)
        {
            ret = len;
        }
        else
        {
            // error
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

