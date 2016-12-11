#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "lib/xbee.h"
#include "lib/serial.h"
#include "lib/status.h"
#include "lib/controller.h"
#include "lib/bmp.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define USART_BAUDRATE 1200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define ever ;;
#define RX_TIMEOUT 1453
#define SIM_INTERVAL 500
#define PEER_INTERVAL 10000
#define INITIAL_RISE 1750

int main(void)
{
    // Initialize addresses
    uint64_t peer = (uint64_t)0;
    uint64_t sim = (uint64_t)0;

    uint8_t wat_type;
    uint16_t frame_len;
    uint8_t frame[MAX_BUF_SIZE];
    uint8_t msg_type;
    uint16_t ballast_time = 0;
    uint8_t send_ballast = 0;

    // Initialize altitudes.
    int32_t initial_alt = INT32_MIN;
    int32_t alt = INT32_MIN;
    int32_t peer_alt = INT32_MIN;
    int32_t prev_dist = 0;
    int32_t sum_dist = 0;

    status_pin_init();
    tim_init();
    solenoid_init();
    serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);
    xbee_init();
    sei();

    // Send a WAT request right when booting up.
    tx((uint8_t*)&MSG_TYPES.WAT_REQUEST, 1, BROADCAST, 0x00);

    // Initialize timers after sending the first tx.
    timer_1 = 0;
    sim_timer = 0;
    peer_timer = 0;
    currently_peering = 1;
    for(ever)
    {
        // Stay in this loop until we get our first altitude.
        while (initial_alt == INT32_MIN)
        {
            /* if (((sim == 0) || peer == 0) && peer_timer >= 5300) */
            if (((sim == 0) || peer == 0) && peer_timer >= SIM_INTERVAL)
            {
                tx((uint8_t*)&MSG_TYPES.WAT_REQUEST, 1, BROADCAST, 0x00);
                peer_timer = 0;
            }
            /* else if ((sim != 0) && (peer != 0) && sim_timer >= 10000) */
            else if ((sim != 0) && (peer != 0) && sim_timer >= SIM_INTERVAL)
            {
                send_sim_alt_request(sim, 0);
                sim_timer = 0;
            }
            // Try rx, timeout if over 3s
            timer_1 = 0;
            if (!rx(frame, RX_TIMEOUT))
            {
                /* status_clear(STATUS2); */
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
                    initial_alt = get_alt(frame, frame_len);
                }
                /* status_set(STATUS2); */
            }
        }

        // This payload's initialization is complete. The other payload and simulator may 
        // not be done yet though, or may reset during flight, so keep watching for 
        // WAT requests.

        // Set green LED to solid instead of blinking.
        currently_peering = 0;
        status_clear(STATUS0);
        alt = initial_alt;
        send_payload_alt_request(peer, initial_alt);
        send_sim_alt_request(sim, 0);
        timer_1 = 0;
        peer_timer = 0;
        sim_timer = 0;
        for(ever)
        {
            /* if (sim_timer >= 1755) */
            if (sim_timer >= SIM_INTERVAL)
            {
                if (send_ballast)
                {
                    send_sim_alt_request(sim, ballast_time);
                    send_ballast = 0;
                }
                else
                {
                    send_sim_alt_request(sim, 0);
                }

                sim_timer = 0;
            }
            if (peer_timer >= PEER_INTERVAL)
            {
                send_payload_alt_request(peer, alt);
                peer_timer = 0;
            }
            timer_1 = 0;
            if(!rx(frame, RX_TIMEOUT))
            {
                status_clear(STATUS2);
                frame_len = get_frame_len(frame);
                msg_type = get_msg_type(frame, frame_len);
                if (msg_type == MSG_TYPES.SIM_ALT)
                {
                    //status(STATUS3);
                    alt = get_alt(frame, frame_len);
                }
                else if (msg_type == MSG_TYPES.PAYLOAD_ALT)
                {
                    //status(STATUS4);
                    peer_alt = get_alt(frame, frame_len);
                    // Check that we rose INITIAL_RISE decimeters from our start.
                    if (alt - initial_alt > INITIAL_RISE)
                    {
                        send_ballast = 1;
                        ballast_time = control(alt, peer_alt, &prev_dist, &sum_dist);
                        activate_solenoid(ballast_time);
                    }
                }
                else if (msg_type == MSG_TYPES.PAYLOAD_ALT_REQUEST)
                {
                    //status(STATUS5);
                    send_alt(peer, alt);
                }
                else if (msg_type == MSG_TYPES.WAT_REQUEST)
                {
                    //status(STATUS6);
                    send_wat_reply(get_source_addr(frame));
                }
                else
                {
                    //status(STATUS7);
                }
                status_set(STATUS2);
            }
        }
    }
}
