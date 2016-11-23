#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "xbee.h"
#include "bmp.h"

#include "controller.h"

volatile uint16_t timer = 0;
volatile uint16_t peer_timer = 0;
volatile uint16_t sim_timer = 0;
volatile uint16_t solenoid_timer = 0;
volatile uint16_t solenoid_on_time = 0;
volatile uint8_t solenoid_on = 0;

// timeout fix

void tim_init()
{
    // 1ms resolution
    TCCR1B |= (1<<WGM12);
    TCCR1B |= (1<<CS11)|(1<<CS10);
    OCR1A = 125;

    TIMSK1 |= (1<<OCIE1A);
}

void solenoid_init()
{
    PORTB &= ~(1 << PB1);
    DDRB |= (1 << PB1);
}

// Activate solenoid for on_time ms
void activate_solenoid(uint16_t on_time)
{
    PORTB |= (1 << PB1);
    solenoid_timer = 0;
    solenoid_on = 1;
    solenoid_on_time = on_time;
}

void deactivate_solenoid()
{
    solenoid_on = 0;
    PORTB &= ~(1 << PB1);
}

uint16_t control(int32_t alt, int32_t peer_alt)
{
    uint16_t release_time = 0;
    int32_t distance;

    /* distance = alt - peer_alt; */
    distance = peer_alt - alt;

    // Greater than 100 decimeters
    if (distance > 100)
    {
        /* release_time = distance / PROP_SCALER; */
        /* release_time /= 1.5;    // gain 1.5 decimeters per second per */
        /* release_time *= 100;    // 100 grams of weight lost */
        /* release_time /= POUR_RATE; */
        release_time = distance * 25;
    }

    if (release_time > 14000)
        release_time = 14000;

    return release_time;
}

ISR(TIMER1_COMPA_vect)
{
    // timer is a ms counter
    timer++;
    sim_timer++;
    peer_timer++;
    solenoid_timer++;
    if (solenoid_on && (solenoid_timer >= solenoid_on_time))
    {
        deactivate_solenoid();
    }
}
