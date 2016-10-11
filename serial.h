#ifndef SERIAL_H
#define SERIAL_H

#define TX_ENABLE()         UCSR0B |= (1<<TXEN0)
#define TX_DISABLE()        UCSR0B &= ~(1<<TXEN0)
#define TX_INT_ENABLE()     UCSR0B |= (1<<TXCIE0)
#define TX_INT_DISABLE()    UCSR0B &= ~(1<<TXCIE0)

#define RX_ENABLE()         UCSR0B |= (1<<RXEN0)
#define RX_DISABLE()        UCSR0B &= ~(1<<RXEN0)
#define RX_INT_ENABLE()     UCSR0B |= (1<<RXCIE0)
#define RX_INT_DISABLE()    UCSR0B &= ~(1<<RXCIE0)


#define TX

//! Initialize UART with the given parameters.
uint8_t serial_init(uint16_t baudrate); //! baudrate (prescaled)

//! Get a byte from the UART
uint8_t get_byte();

//! Put a byte to the UART
void put_byte(uint8_t byte);

//! ISR for handling UART RX
void ISR(USART_RXC_vect);
