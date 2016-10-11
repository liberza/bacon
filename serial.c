#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t serial_rx_buffer;

uint8_t serial_init(uint16_t baudrate, uint8_t bits, uint8_t parity, uint8_t stop)
{
    if ((bits != DATA_BITS_5) &&
        (bits != DATA_BITS_6) &&
        (bits != DATA_BITS_7) &&
        (bits != DATA_BITS_8) &&
        (bits != DATA_BITS_9))
    {
        // error
    }
    else if ((parity != PARITY_DISABLED) &&
             (parity != PARITY_EVEN) &&
             (parity != PARITY_ODD))
    {
        // error
    }
    else if ((stop != STOP_BITS_1) && (stop != STOP_BITS_2))
    {
        // error
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
