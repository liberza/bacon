#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
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
#define NO_ADDR (uint64_t)0

//#define RX_TIMEOUT 211
#define RX_TIMEOUT 400
//#define SIM_INTERVAL 223
//#define PEER_INTERVAL 223
#define SIM_INTERVAL 223
#define PEER_INTERVAL 1000
#define CONTROL_INTERVAL 5000

#define INITIAL_RISE 1750

int main(void)
{
    // Initialize addresses
    uint64_t peer = NO_ADDR;
    uint64_t sim = NO_ADDR;

    uint8_t wat_type;
    uint8_t frame_len;
    uint8_t frame[MAX_BUF_SIZE];
    uint8_t msg_type;
    uint16_t ballast_time = 0;
    uint8_t send_ballast = 0;
    uint8_t mode = SIM_MODE;

    // Initialize altitudes.
    int32_t initial_alt = INT32_MIN;
    int32_t alt = INT32_MIN;
    int32_t peer_alt = INT32_MIN;
    //int32_t prev_dist = 0;
    int32_t prev_dists[PREV_DISTS];
    int32_t prev_delta_dists[PREV_DISTS];

    status_pin_init();
    mode_pin_init();
    status_set(STATUS0 | STATUS1 | STATUS2);
    tim_init();
    solenoid_init();
    serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);
    xbee_init();
    sei();
    wdt_enable(WDTO_1S);

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
            // Try rx, timeout if takes longer than RX_TIMEOUT
            timer_1 = 0;
            if (!rx(frame, RX_TIMEOUT))
            {
                status_clear(STATUS2);
                frame_len = get_frame_len(frame);
                msg_type = get_msg_type(frame, frame_len);
                if (msg_type == MSG_TYPES.WAT_REQUEST)
                {
                    uint64_t src = get_source_addr(frame);
                    send_wat_reply(src);
                    if ((peer != NO_ADDR) && src != peer)
                    {   // Send peer's address to show we peered successfully
                        if (sim == NO_ADDR)
                            sim = src;
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
                else
                {
                    // Got an unknown message. Hold off on transmitting and read more.
                    //peer_timer = 0;
                    //sim_timer = 0;
                }
                status_set(STATUS2);
            }
            if (((sim == NO_ADDR) || peer == NO_ADDR) && peer_timer >= PEER_INTERVAL)
            {
                tx((uint8_t*)&MSG_TYPES.WAT_REQUEST, 1, BROADCAST, 0x00);
                peer_timer = 0;
            }
            if ((sim != NO_ADDR) && (peer != NO_ADDR) && sim_timer >= PEER_INTERVAL)
            {
                send_sim_alt_request(sim, 0);
                sim_timer = 0;
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
        peer_timer = 0;
        sim_timer = 0;
        for(ever)
        {
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
                    sim_timer = 500;  // synchronize with the other payload
                    //status(STATUS4);
                    peer_alt = get_alt(frame, frame_len);
                    // Check that we rose INITIAL_RISE decimeters from our start.
                    if (alt - initial_alt > INITIAL_RISE)
                    {
                        send_ballast = 1;
                        ballast_time = control(alt, peer_alt, prev_dists, prev_delta_dists);
                        activate_solenoid(ballast_time);
                    }
                }
                else if (msg_type == MSG_TYPES.PAYLOAD_ALT_REQUEST)
                {
                    send_alt(peer, alt);
                }
                else if (msg_type == MSG_TYPES.WAT_REQUEST)
                {
                    send_wat_reply(get_source_addr(frame));
                }
                else
                {
                    //status(STATUS7);
                }
                status_set(STATUS2);
            }
            if (peer_timer >= CONTROL_INTERVAL)
            {
                send_payload_alt_request(peer, alt);
                peer_timer = 0;
                sim_timer = 450; // synchronize with the other payload
            }
            if ((sim_timer >= SIM_INTERVAL) && (mode == SIM_MODE))
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
        }
    }
}
