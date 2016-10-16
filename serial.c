#include <avr/io.h>
#include <avr/interrupt.h>

#include "serial.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// need to change buffer size
#define BUF_SIZE 255;

uint8_t serial_rx_buf_index = 0;
uint8_t serial_rx_buffer[BUF_SIZE];

uint8_t serial_init(uint16_t baudrate, uint8_t bits, uint8_t parity, uint8_t stop)
{
    uint8_t err = 0;
    if ((bits != DATA_BITS_5) &&
        (bits != DATA_BITS_6) &&
        (bits != DATA_BITS_7) &&
        (bits != DATA_BITS_8) &&
        (bits != DATA_BITS_9))
    {
        err |= DATA_BITS_ERR;
    }
    if ((parity != PARITY_DISABLED) &&
        (parity != PARITY_EVEN) &&
        (parity != PARITY_ODD))
    {
        err |= PARITY_ERR;
    }
    if ((stop != STOP_BITS_1) && (stop != STOP_BITS_2))
    {
        err |= STOP_BITS_ERR;
    }
    if (!err)
    {
        UBRR0H = baudrate >> 8;
        UBRR0L = baudrate;
        UCSR0C = bits | parity | stop;
        TX_ENABLE();
        RX_ENABLE();
        TX_INT_ENABLE();
    }
    return err;
}

void ISR(USART_RXC_vect)
{
	serial_rx_buffer[serial_rx_buf_index] = UBR0;
	serial_rx_buf_index++;	
}
