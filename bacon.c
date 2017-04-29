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
#include "lib/alt.h"

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

//#define INITIAL_RISE 1750
#define INITIAL_RISE 0

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

    // Initialize altitudes.
    int32_t initial_alt = INT32_MIN;
    int32_t peer_initial_alt = INT32_MIN;
    int32_t alt = INT32_MIN;
    int32_t peer_alt = INT32_MIN;
    int32_t offset = 0;
    int32_t prev_dists[PREV_DISTS];
    int32_t prev_dists_avg[PREV_DISTS];
    uint16_t alt_coeffs[8];

    for (int i=0; i < PREV_DISTS; i++) {
        prev_dists[i] = 0;
        prev_dists_avg[i] = 0;
    }

    // Perform various initializations
    status_pin_init();
    flight_mode_init();
    alt_init(alt_coeffs);
    status_set(STATUS0 | STATUS1 | STATUS2);
    tim_init();
    solenoid_init();
    serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);
    xbee_init();
    sei();
    wdt_enable(WDTO_1S);

    // Check for simulation mode or flight mode
    uint8_t mode = get_flight_mode();

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
                    status_set(STATUS3);
                    uint64_t src = get_source_addr(frame);
                    send_wat_reply(src);
                    if ((mode == SIM_MODE) && (peer != NO_ADDR) && (src != peer))
                    {   // Send peer's address to show we peered successfully
                        if (sim == NO_ADDR)
                            sim = src;
                        send_peer_addr(peer, sim);
                    }
                    status_clear(STATUS3);
                }
                else if (msg_type == MSG_TYPES.WAT_REPLY)
                {
                    status_set(STATUS4);
                    wat_type = get_wat_type(frame, frame_len);
                    if (wat_type == 'P')
                        peer = get_source_addr(frame);
                    else if (wat_type == 'S')
                        sim = get_source_addr(frame);
                    // ignore else
                    status_clear(STATUS4);
                }
                else if ((mode == SIM_MODE) && (msg_type == MSG_TYPES.SIM_ALT) && (peer != NO_ADDR))
                {
                    status_set(STATUS5);
                    // first altitude comes from simulator, once we konw the peer.
                    initial_alt = parse_alt(frame, frame_len);
                    // we just assume the peer's initial alt is ours for sim for now...
                    status_clear(STATUS5);
                }
                status_set(STATUS2);
            }
            if ((mode == FLIGHT_MODE) && (peer != NO_ADDR))
            {
                status_set(STATUS6);
                // first altitude comes from ourself, once we know the peer.
                initial_alt = get_alt(alt_coeffs);

                status_clear(STATUS6);
            }

            if ((((mode == SIM_MODE) && (sim == NO_ADDR)) || (peer == NO_ADDR)) && 
                (peer_timer >= PEER_INTERVAL))
            {
                tx((uint8_t*)&MSG_TYPES.WAT_REQUEST, 1, BROADCAST, 0x00);
                peer_timer = 0;
            }
            if ((mode == SIM_MODE) && 
                (sim != NO_ADDR) && 
                (peer != NO_ADDR) && 
                (sim_timer >= PEER_INTERVAL))
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
        if (mode == SIM_MODE)
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
                if ((mode == SIM_MODE) && (msg_type == MSG_TYPES.SIM_ALT))
                {
                    //status(STATUS3);
                    alt = parse_alt(frame, frame_len);
                }
                else if (msg_type == MSG_TYPES.PAYLOAD_ALT)
                {
                    if (mode == SIM_MODE)
                        sim_timer = 500;  // synchronize with the other payload
                    else
                        alt = get_alt(alt_coeffs);
                    peer_alt = parse_alt(frame, frame_len);
                    if (peer_initial_alt == INT32_MIN)
                    {
                        peer_initial_alt = peer_alt;
                        offset = initial_alt - peer_initial_alt;
                        send_alt(BROADCAST, offset);
                    }
                    // Check that we rose INITIAL_RISE decimeters from our start.
                    if (alt - initial_alt > INITIAL_RISE)
                    {
                        send_ballast = 1;
                        ballast_time = control(alt, peer_alt + offset, prev_dists, prev_dists_avg);
                        activate_solenoid(ballast_time);
                    }
                    // if not, do averaging anyway because we need delta dists right when we pass INITIAL_RISE.
                    else
                    {
                        do_averaging(alt, peer_alt + offset, prev_dists, prev_dists_avg);
                    }
                }
                else if (msg_type == MSG_TYPES.PAYLOAD_ALT_REQUEST)
                {
                    if (mode == FLIGHT_MODE)
                        alt = get_alt(alt_coeffs);
                    send_alt(peer, alt);
                }
                else if (msg_type == MSG_TYPES.WAT_REQUEST)
                {
                    send_wat_reply(get_source_addr(frame));
                }
                status_set(STATUS2);
            }
            if (peer_timer >= CONTROL_INTERVAL)
            {
                if (mode == FLIGHT_MODE)
                    alt = get_alt(alt_coeffs);
                send_payload_alt_request(peer, alt);
                peer_timer = 0;
                sim_timer = 450; // synchronize with the other payload
            }
            if ((mode == SIM_MODE) && (sim_timer >= SIM_INTERVAL))
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
