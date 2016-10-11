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

#define DATA_BITS_5 (uint8_t)(0x00)
#define DATA_BITS_6 (uint8_t)(1<<UCSZ00)
#define DATA_BITS_7 (uint8_t)(1<<UCSZ01)
#define DATA_BITS_8 (uint8_t)((1<<UCSZ00)|(1<<UCSZ01))
#define DATA_BITS_9 (uint8_t)((1<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00))

#define STOP_BITS_1 (uint8_t)(0x00)
#define STOP_SIZE_2 (uint8_t)(1<<USBS0)

#define PARITY_DISABLED (uint8_t)(0x00)
#define PARITY_EVEN     (uint8_t)(1<<UPM01)
#define PARITY_ODD      (uint8_t)((1<<UPM01)|(1<<UPM00))

extern uint8_t serial_rx_buffer;

//! Initialize UART with the given parameters.
uint8_t serial_init(uint16_t baudrate, //! baudrate (prescaled)
                    uint8_t bits,      //! data bits setting
                    uint8_t parity,    //! parity bits setting
                    uint8_t stop);     //! stop bits setting


//! Get a byte from the UART
uint8_t get_byte();

//! Put a byte to the UART
void put_byte(uint8_t byte);

//! ISR for handling UART RX
void ISR(USART_RXC_vect);

#endif
