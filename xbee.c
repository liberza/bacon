#include "xbee.h"
#include "pingpong.h"
#include <avr/io.h>

pingpong_t *rx_buf;

uint8_t xbee_init()
{
    rx_buf = pingpong_create(BUF_SIZE);
    TX_INT_ENABLE();
    RX_INT_ENABLE();
    return 0;
}

uint8_t rx()
{
    uint16_t len;
    uint8_t *rx_chunk;
    if (rx_buf->ready)
    {
        rx_buf->ready = 0;
        rx_chunk = pingpong_chunk_ptr(rx_buf);
        if (rx_chunk[0] != SPECIAL_BYTES.FRAME_DELIM)
        {
            return -1;
        }
        unescape(rx_chunk, rx_buf->size);
        len = (rx_buf->buf[1] << 8) | rx_buf->buf[0];
        return 1;
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

ISR(USART_RX_vect)
{
    uint8_t tmp = UDR0;
    if (tmp == SPECIAL_BYTES.FRAME_DELIM)
    {
        pingpong_swap(rx_buf);
        rx_buf->ready = 1;
    }
    pingpong_write(rx_buf, tmp);
}

