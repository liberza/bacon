#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "xbee.h"
#include "serial.h"
#include "status.h"
#include "controller.h"
#include "bmp.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define USART_BAUDRATE 1200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define ever ;;

int main(void)
{
    status_pin_init();
    serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);
    xbee_init();
    tim_init();
    solenoid_init();
    uint64_t peer = (uint64_t)0;
    uint64_t sim = (uint64_t)0;
    uint8_t wat_type;
    uint16_t frame_len;
    uint8_t frame[MAX_BUF_SIZE];
    uint8_t msg_type;
    uint16_t ballast_time = 0;

    int32_t alt = INT32_MIN;
    int32_t initial_alt = INT32_MIN;

    sei();

    tx((uint8_t*)&MSG_TYPES.WAT_REQUEST, 1, BROADCAST, 0x00);
    timer = 0;
    for(ever)
    {
        while (initial_alt == INT32_MIN)
        {
            status(STATUS0);
            if (((sim == 0) || peer == 0) && timer >= 3000)
            {
                tx((uint8_t*)&MSG_TYPES.WAT_REQUEST, 1, BROADCAST, 0x00);
                timer = 0;
            }
            else if ((sim != 0) && (peer != 0) && timer >= 10000)
            {
                send_alt_request(sim, 0);
                timer = 0;
            }
            // Try rx, timeout if over 3s
            if (!rx(frame, 3000))
            {
                status(0);
                frame_len = get_frame_len(frame);
                msg_type = get_msg_type(frame, frame_len);
                if (msg_type == MSG_TYPES.WAT_REQUEST)
                {
                    send_wat_reply(get_source_addr(frame));
                    if (peer != 0)
                    {   // Send peer's address to show we peered successfully
                        send_peer_addr(peer, sim);
                    }
                }
                else if (msg_type == MSG_TYPES.WAT_REPLY)
                {
                    wat_type = get_wat_type(frame, frame_len);
                    if (wat_type == 'P')
                        peer = get_source_addr(frame);
                    else if (wat_type == 'S')
                        sim = get_source_addr(frame);
                    // ignore else
                }
                else if (msg_type == MSG_TYPES.SIM_ALT)
                {
                    status(STATUS3);
                    initial_alt = get_alt(frame, frame_len);
                }
            }
        }
        status(STATUS2);
        alt = initial_alt;
        send_alt_request(sim, 0);
        timer = 0;
        for(ever)
        {
            if (timer >= 10000)
            {
                send_alt_request(sim, ballast_time);
                timer = 0;
                ballast_time = 0;
            }
            if(!rx(frame, 3000))
            {
                frame_len = get_frame_len(frame);
                msg_type = get_msg_type(frame, frame_len);
                if (msg_type == MSG_TYPES.SIM_ALT)
                {
                    alt = get_alt(frame, frame_len);
                }
                if (alt > 1750)
                    status(STATUS3);
            }
        }
    }
}
