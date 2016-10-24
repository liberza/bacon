#ifndef XBEE_H
#define XBEE_H

#include <stdint.h>
#include <avr/interrupt.h>
#include "pingpong.h"

#define TX_INT_ENABLE()     UCSR0B |= (1<<TXCIE0)
#define TX_INT_DISABLE()    UCSR0B &= ~(1<<TXCIE0)

#define RX_INT_ENABLE()     UCSR0B |= (1<<RXCIE0)
#define RX_INT_DISABLE()    UCSR0B &= ~(1<<RXCIE0)

// XBee node IDs
#define SIM_H       0x0000ba53
#define SIM_L       0x0000ba11
#define PAYLOAD_1H  0x00001337
#define PAYLOAD_1L  0x0000dead
#define PAYLOAD_0H  0x0000beef
#define PAYLOAD_0L  0x0000cafe

#define BUF_SIZE 256

//! Buffer for serial RX interrupt service routne.
extern pingpong_t *rx_buf;

// Digimesh frame special characters.
static struct
{
    uint8_t FRAME_DELIM;
    uint8_t ESCAPE;
    uint8_t XON;
    uint8_t XOFF;
} SPECIAL_BYTES =
{
    .FRAME_DELIM = (uint8_t)0x7E,
    .ESCAPE      = (uint8_t)0x7D,
    .XON         = (uint8_t)0x11,
    .XOFF        = (uint8_t)0x13
};

//! Digimesh frame types.
static struct 
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
} FRAME_TYPES =
{
    .AT             = (uint8_t)0x08,
    .AT_QPV         = (uint8_t)0x09,
    .TX             = (uint8_t)0x10,
    .EXPLICIT_TX    = (uint8_t)0x11,
    .REMOTE         = (uint8_t)0x17,
    .AT_RESP        = (uint8_t)0x88,
    .MODEM_STATUS   = (uint8_t)0x8A,
    .TX_STATUS      = (uint8_t)0x8B,
    .ROUTE_INFO     = (uint8_t)0x8D,
    .RX             = (uint8_t)0x90,
    .EXPLICIT_RX    = (uint8_t)0x91,
    .NODE_ID        = (uint8_t)0x95,
    .REMOTE_RESP    = (uint8_t)0x97
};

struct frame
{
    uint8_t type;
    uint8_t source;
};

//! Receive a Digimesh frame, if available in the serial buffer.
uint8_t rx();

//! Transmit a Digimesh frame.
uint8_t tx(uint8_t *frame);

//! Issue an AT command to the XBee unit.
uint8_t at();

//! Escape a sequence of bytes by XOR'ing special chars with 0x20 and
//! adding the ESCAPE byte before the special char.
uint8_t escape(uint8_t *bytes, uint16_t size);

//! Remove ESCAPE bytes and XOR special chars with 0x20.
void unescape(uint8_t *bytes, uint16_t size);

uint16_t validate_frame(uint8_t *bytes, uint16_t size);

//! ISR for writing bytes and swapping buffers when frame delimiter received.
ISR(USART_RX_vect);

#endif
