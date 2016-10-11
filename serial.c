#include <avr/io.h>
#include <avr/interrupt.h>

#include "serial.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t serial_rx_buffer;

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
    if (err)
    {
        // eventually can be used to light status LEDs
        return err;
    }
    else
    {
        UBRR0H = baudrate>>8;
        UBRR0L = baudrate;
        UCSR0C = 0x00 | bits | parity | stop;
        TX_ENABLE();
        RX_ENABLE();
        TX_INT_ENABLE();
    }
}

void ISR(USART_RXC_vect)
{
    serial_rx_buffer = UBR0;
}
