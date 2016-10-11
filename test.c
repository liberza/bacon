#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define F_CPU 1000000UL
#define USART_BAUDRATE 1200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define ever ;;

int main(void)
{
    char rByte;

    init_serial();

    for(ever)
    {
        while ((UCSR0A & (1 << RXC0)) == 0) {};   // busy wait until data received
        rByte = UDR0;

        while ((UCSR0A & (1 << UDRE0)) == 0) {};  // busy wait until data ready to be written
        UDR0 = rByte; // echo back
    }
}

void init_serial()
{
    // Set the baud rate.
    UCSR0A |= 0<<U2X0;                 // Normal speed async
    UBRR0H= (BAUD_PRESCALE>>8);
    UBRR0L= BAUD_PRESCALE;

    UCSR0C= (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01); // 8 bit, no parity, 2 stop
    UCSR0B= (1<<RXEN0)|(1<<TXEN0);  // Enable RX / TX

}

