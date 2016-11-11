#ifndef XBEE_H
#define XBEE_H

#include <stdint.h>
#include <avr/interrupt.h>
#include "rbuf.h"

#define TX_INT_ENABLE()     UCSR0B |= (1<<TXCIE0)
#define TX_INT_DISABLE()    UCSR0B &= ~(1<<TXCIE0)

#define RX_INT_ENABLE()     UCSR0B |= (1<<RXCIE0)
#define RX_INT_DISABLE()    UCSR0B &= ~(1<<RXCIE0)

#define STATUS_RX (uint8_t)(0x01)
#define STATUS_FIND_FRAME (uint8_t)(0x02)
#define STATUS_ESCAPE (uint8_t)(0x03)
#define STATUS_VALIDATE_FRAME (uint8_t)(0x04)
#define FRAME_SIZE_ERR (uint8_t)(0x01)
#define FRAME_DELIM_ERR (uint8_t)(0x02)
#define FRAME_SUM_ERR (uint8_t)(0x04)

//! Buffer for serial RX interrupt service routne.
volatile extern rbuf_t rbuf;

// Digimesh frame special characters.
struct special_bytes_t
{
    uint8_t FRAME_DELIM;
    uint8_t ESCAPE;
    uint8_t XON;
    uint8_t XOFF;
};

const static struct special_bytes_t SPECIAL_BYTES;

//! Digimesh frame types.
struct frame_types_t
{
    uint8_t AT;
    uint8_t AT_QPV;
    uint8_t TX;
    uint8_t EXPLICIT_TX;
    uint8_t REMOTE;
    uint8_t AT_RESP;
    uint8_t MODEM_STATUS;
    uint8_t TX_STATUS;
    uint8_t ROUTE_INFO;
    uint8_t RX;
    uint8_t EXPLICIT_RX;
    uint8_t NODE_ID;
    uint8_t REMOTE_RESP;
};

struct frame_t
{
    uint16_t len;
    uint8_t data[BUF_SIZE];
};

const static struct frame_types_t FRAME_TYPES;

//! Initialize buffer and interrupts
void xbee_init();

//! Receive a Digimesh frame, if available in the serial buffer.
uint8_t rx(uint8_t *frame);

//! Transmit a Digimesh frame.
uint8_t tx(uint8_t *data, uint8_t psize, uint64_t dest, uint8_t opts);

//! Escape a sequence of bytes by XOR'ing special chars with 0x20 and
//! adding the ESCAPE byte before the special char.
uint8_t escape(uint8_t *frame, uint16_t size);

//! Remove ESCAPE bytes and XOR special chars with 0x20.
void unescape(uint8_t *frame, uint16_t size);

uint8_t validate_frame(uint8_t *frame, uint16_t size);

uint8_t find_frame(volatile rbuf_t *r, uint8_t *frame);

//! ISR for writing bytes and swapping buffers when frame delimiter received.
ISR(USART_RX_vect);

#endif
