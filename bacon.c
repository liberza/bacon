#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "xbee.h"
#include "serial.h"
#include "status.h"
#include "solenoid.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define USART_BAUDRATE 1200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define ever ;;

volatile uint8_t rx_flag;

int main(void)
{
    status_pin_init();
    serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);
    xbee_init();
    solenoid_init();
    uint64_t peer;
    uint16_t data_len;
    uint8_t frame[MAX_BUF_SIZE];
    uint16_t start, end;

    sei();

    for(ever)
    {
        rx(frame);
        if (frame[3] == FRAME_TYPES.RX)
        {
            peer = (uint64_t)frame[4] << 56 |
                   (uint64_t)frame[5] << 48 |
                   (uint64_t)frame[6] << 40 |
                   (uint64_t)frame[7] << 32 |
                   (uint64_t)frame[8] << 24 |
                   (uint64_t)frame[9] << 16 |
                   (uint64_t)frame[10] << 8 |
                   (uint64_t)frame[11];

            data_len = (((uint16_t)frame[1]<<8)|(uint16_t)frame[2]);
            tx(frame+FRAME_OHEAD.RX-1, data_len-FRAME_OHEAD.RX+4, peer, 0x00);
        }
    }
}
