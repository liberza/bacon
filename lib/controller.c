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
#define MAX_SOLENOID_TIME 5000
#define CONTROLLER_P (uint32_t)(27)
#define CONTROLLER_D (uint32_t)(200)
#define CONTROLLER_P_NEAR (uint32_t)(2)
#define CONTROLLER_D_NEAR (uint32_t)(200)
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

// Determine how long to turn the solenoid on for.
uint16_t control(int32_t alt, int32_t peer_alt, int32_t *prev_dists, int32_t *prev_delta_dists)
{
    int32_t release_time = 0;
    int32_t dist, delta_dist;
    int32_t avg_dist = 0;
    int32_t avg_delta_dist = 0;

    dist = peer_alt - alt;
    delta_dist = dist - prev_dists[PREV_DISTS-1];

    // Average the previous distances and slide the window over simultaneously.
    for (int i=0; i<PREV_DISTS; i++) {
        avg_dist += prev_dists[i];
        avg_delta_dist += prev_delta_dists[i];
        if (i != PREV_DISTS-1) {
            prev_dists[i] = prev_dists[i+1];
            prev_delta_dists[i] = prev_delta_dists[i+1];
        }
    }
    prev_dists[PREV_DISTS-1] = dist;
    prev_delta_dists[PREV_DISTS-1] = delta_dist;

    avg_dist /= PREV_DISTS;
    avg_delta_dist /= PREV_DISTS;

    // distance and delta dist:
    // if they're positive, compensate
    // if they're negative, don't
    if (dist > THRESHOLD_DIST)
    {
        release_time = avg_dist * CONTROLLER_P + avg_delta_dist * CONTROLLER_D;
    }
    // The other payload is close, and going to pass. Start compensating.
    else if (dist > -THRESHOLD_DIST)
    {
        release_time = avg_dist * CONTROLLER_P_NEAR + delta_dist * CONTROLLER_D_NEAR;
    }

    if (release_time > MAX_SOLENOID_TIME)
        release_time = MAX_SOLENOID_TIME;
    /*
    else if (avg_delta_dist < MAX_DESIRED_SPEED)
        // if we're getting closer, don't drop anything. 
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
