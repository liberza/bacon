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

ISR(USART_RX_vect)
{
    uint8_t tmp = UDR0;
    if (tmp == SPECIAL_BYTES.FRAME_DELIM)
    {
        pingpong_swap(rx_buf);
    }
    pingpong_write(rx_buf, tmp);
}

