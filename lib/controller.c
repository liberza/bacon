#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include "xbee.h"
#include "bmp.h"
#include "status.h"

#include "controller.h"

#define PEERING_LED_DELAY 500
#define THRESHOLD_DIST 40
#define INNER_THRESHOLD_DIST 10
#define MAX_SOLENOID_TIME 5000
#define CONTROLLER_P (uint32_t)(27)
#define CONTROLLER_D (uint32_t)(170)
#define CONTROLLER_P_NEAR (uint32_t)(3)
//#define CONTROLLER_D_NEAR (uint32_t)(125)
//#define CONTROLLER_D_NEAR (uint32_t)(175)
#define CONTROLLER_D_NEAR (uint32_t)(130)
#define MAX_DESIRED_SPEED (uint32_t)(-25)

volatile uint16_t timer_1 = 0;
volatile uint16_t timer_2 = 0;
volatile uint16_t peer_timer = 0;
volatile uint16_t sim_timer = 0;
volatile uint16_t solenoid_on_time = 0;
volatile uint8_t solenoid_on = 0;
volatile uint8_t currently_peering = 0;

// Initialize timer.
void tim_init()
{
    // 1ms resolution
    // See datasheet if you don't know why this is 1ms.
    TCCR1B |= (1<<WGM12);
    TCCR1B |= (1<<CS11)|(1<<CS10);
    OCR1A = 125;

    TIMSK1 |= (1<<OCIE1A);
}

// Set solenoid pin to 0 and make it an output.
void solenoid_init()
{
    PORTB &= ~(1 << PB1);
    DDRB |= (1 << PB1);
}

// Set mode pin as an input.
// High = flight, low = sim
void flight_mode_init()
{
    DDRB &= ~(1 << PB0);
}

// Returns the flight mode, based on the mode switch.
uint8_t get_flight_mode()
{
    if (PINB & (1 << PB0))
        return FLIGHT_MODE;
    else
        return SIM_MODE;
}

// Activate solenoid for on_time ms
void activate_solenoid(uint16_t on_time)
{
    PORTB |= (1 << PB1);
    timer_2 = 0;
    solenoid_on = 1;
    solenoid_on_time = on_time;
}

// Deactivate the solenoid.
void deactivate_solenoid()
{
    solenoid_on = 0;
    PORTB &= ~(1 << PB1);
}

void do_averaging(int32_t alt, int32_t peer_alt, int32_t *prev_dists, int32_t *prev_dists_avg)
{
    int32_t dist = peer_alt - alt;
    int32_t avg_dist = 0;
    for (int i=0; i<PREV_DISTS; i++) {
        avg_dist += prev_dists[i];
        if (i != PREV_DISTS-1) {
            prev_dists[i] = prev_dists[i+1];
            prev_dists_avg[i] = prev_dists_avg[i+1];
        }
    }
    prev_dists[PREV_DISTS-1] = dist;
    avg_dist += dist;
    avg_dist /= PREV_DISTS;

    prev_dists_avg[PREV_DISTS-1] = avg_dist;
}

// Determine how long to turn the solenoid on for.
uint16_t control(int32_t alt, int32_t peer_alt, int32_t *prev_dists, int32_t *prev_dists_avg)
{
    int32_t release_time = 0;
    int32_t delta_dist;
    int32_t dist;

    dist = peer_alt - alt;

    /* Working on stopping after burst
    int32_t avg_alt = 0;
    for (int i=0; i<PREV_ALTS; i++) {
        avg_alt += prev_alts[i];
        if (i != PREV_ALTS-1)
            prev_alts[i] = prev_alts[i+1];
    }
    prev_alts[PREV_ALTS-1] = alt;
    avg_alt += alt;
    avg_alt /= PREV_ALTS;
    if (avg_alt < 
    */

    do_averaging(alt, peer_alt, prev_dists, prev_dists_avg);
    delta_dist = prev_dists_avg[PREV_DISTS-1] - prev_dists_avg[PREV_DISTS-2];

    // distance and delta dist:
    // if they're staying super close, don't do anything
    // if they're positive, compensate
    // if they're negative, don't
    if ((dist < INNER_THRESHOLD_DIST) && (dist > -INNER_THRESHOLD_DIST) &&
        (prev_dists_avg[PREV_DISTS-1] < INNER_THRESHOLD_DIST) &&
        (prev_dists_avg[PREV_DISTS-1] > -INNER_THRESHOLD_DIST))
    {
        release_time = 0;
    }
    else if (dist > THRESHOLD_DIST)
    {
        release_time = dist * CONTROLLER_P + delta_dist * CONTROLLER_D;
    }
    // The other payload is close, and going to pass. Start compensating.
    // We don't need to use the averages here, because the actual samples are less noisy.
    else if (dist > -THRESHOLD_DIST)
    {
        release_time = delta_dist * CONTROLLER_D_NEAR;
    }

    if (release_time > MAX_SOLENOID_TIME)
        release_time = MAX_SOLENOID_TIME;
    /*
    else if (avg_delta_dist < MAX_DESIRED_SPEED)
        // we're closing the gap fast, don't drop anything. 
        release_time = 0;
    */
    else if (release_time < 0)
        release_time = 0;

    return (uint16_t)release_time;
}

// Timer ISR. Handles timer-sensitive events. Particularly, LED blinking and
// solenoid deactivation. The RX interrupt has priority over this, hence ISR_NOBLOCK.
ISR(TIMER1_COMPA_vect, ISR_NOBLOCK)
{
    wdt_reset();
    // timer is a ms counter
    timer_1++;
    timer_2++;
    sim_timer++;
    peer_timer++;
    if (solenoid_on && (timer_2 >= solenoid_on_time))
    {
        deactivate_solenoid();
    }
    else if (currently_peering && (timer_2 >= PEERING_LED_DELAY))
    {
        // Blink green LED
        status_toggle(STATUS0);
        timer_2 = 0;
    }
}
