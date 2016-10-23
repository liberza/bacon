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

void ISR(USART_RX_vect)
{
    uint8_t tmp = UDR0;
    if (tmp == (uint8_t)0x7D)
    {
        pingpong_swap(rx_buf);
    }
    pingpong_write(rx_buf, tmp);
}

