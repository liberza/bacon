#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "xbee.h"
#include "bmp.h"
#include "status.h"

#include "controller.h"

#define PEERING_LED_DELAY 500
#define THRESHOLD_DIST 100
#define MAX_SOLENOID_TIME 5000
#define CONTROLLER_P 20
#define CONTROLLER_I 0
#define CONTROLLER_D 5

volatile uint16_t timer_1 = 0;
volatile uint16_t timer_2 = 0;
volatile uint16_t peer_timer = 0;
volatile uint16_t sim_timer = 0;
volatile uint16_t solenoid_on_time = 0;
volatile uint8_t solenoid_on = 0;
volatile uint8_t currently_peering = 0;

// timeout fix

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

void deactivate_solenoid()
{
    solenoid_on = 0;
    PORTB &= ~(1 << PB1);
}

uint16_t control(int32_t alt, int32_t peer_alt, int32_t *prev_dist, int32_t *sum_dist)
{
    uint32_t release_time = 0;
    int32_t dist, delta_dist;

    /* distance = alt - peer_alt; */
    dist = peer_alt - alt;
    delta_dist = dist - *prev_dist;
    *prev_dist = dist;

    if (dist > THRESHOLD_DIST)
    {
        release_time = dist * CONTROLLER_P + delta_dist * CONTROLLER_D;
    }

    if (release_time > MAX_SOLENOID_TIME)
        release_time = MAX_SOLENOID_TIME;

    return (uint16_t)release_time;
}

// Timer ISR. Handles timer-sensitive events. Particularly, LED blinking and
// solenoid deactivation. The RX interrupt has priority over this, hence ISR_NOBLOCK.
ISR(TIMER1_COMPA_vect, ISR_NOBLOCK)
{
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
