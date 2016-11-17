#ifndef TIMER_H
#define TIMER_H

#define POUR_RATE	4.92893	// pour rate in mL/s
#define PROP_SCALER	600000	// divider in ms

volatile uint16_t timer;
volatile uint16_t peer_timer;
volatile uint16_t sim_timer;
volatile uint16_t solenoid_timer;
volatile uint16_t solenoid_on_time;
volatile uint8_t solenoid_on;

void tim_init();

void solenoid_init();

void activate_solenoid(uint16_t on_time);

void deactivate_solenoid();

uint16_t control(int32_t alt, int32_t peer_alt);

ISR(TIMER1_COMPA_vect);

#endif
