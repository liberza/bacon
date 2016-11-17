#ifndef TIMER_H
#define TIMER_H

volatile uint16_t timer;
volatile uint16_t solenoid_timer;
volatile uint16_t solenoid_on_time;
volatile uint8_t solenoid_on;

void tim_init();

void solenoid_init();

void activate_solenoid(uint16_t on_time);

void deactivate_solenoid();

ISR(TIMER1_COMPA_vect);

#endif
