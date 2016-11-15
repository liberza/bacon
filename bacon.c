#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "xbee.h"
#include "serial.h"
#include "status.h"
#include "solenoid.h"
#include "bmp.h"

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
    uint64_t peer = (uint64_t)0;
    uint64_t sim = (uint64_t)0;
    uint8_t wat_type;
    uint16_t frame_len;
    uint8_t frame[MAX_BUF_SIZE];
    uint8_t msg_type;
    uint8_t count = 0;

    sei();

    tx((uint8_t*)&MSG_TYPES.WAT_REQUEST, 1, BROADCAST, 0x00);
    for(ever)
    {
        status(STATUS0);
        if ((sim == (uint64_t)0) && count >= 5)
        {
            tx((uint8_t*)&MSG_TYPES.WAT_REQUEST, 1, BROADCAST, 0x00);
            count = 0;
        }
        rx(frame);
        status(0);
        frame_len = get_frame_len(frame);
        msg_type = get_msg_type(frame, frame_len);
        if (msg_type == MSG_TYPES.WAT_REQUEST)
        {
            status(STATUS1);
            send_wat_reply(get_source_addr(frame));
        }
        else if (msg_type == MSG_TYPES.WAT_REPLY)
        {
            status(STATUS2);
            wat_type = get_wat_type(frame, frame_len);
            if (wat_type == 'P')
                peer = get_source_addr(frame);
            else if (wat_type == 'S')
                sim = get_source_addr(frame);
            else
            {
                status(STATUS3); // invalid WAT reply.
                tx(&wat_type, 1, BROADCAST, 0x00);
            }
        }
        else
            status(STATUS4);
        count++;
    }
}