#ifndef TIMER_H
#define TIMER_H

#define PREV_DISTS 6
#define SIM_MODE (uint8_t)1
#define FLIGHT_MODE (uint8_t)2


volatile uint16_t timer_1;  // 1ms resolution timer
volatile uint16_t timer_2;  // used for controlling solenoid time and green LED blinking
volatile uint16_t peer_timer;   // timer for transmitting to peer
volatile uint16_t sim_timer;    // timer for transmitting to sim
volatile uint16_t solenoid_on_time; // time duration in ms for solenoid to be activated for.
volatile uint8_t solenoid_on;       // bool for whether or not the solenoid is currently on.
volatile uint8_t currently_peering; // bool for whether or not the payload is peering.

void flight_mode_init();

uint8_t get_flight_mode();

void tim_init();

void solenoid_init();

void activate_solenoid(uint16_t on_time);

void deactivate_solenoid();

void do_averaging(int32_t alt, int32_t peer_alt, int32_t *prev_dists, int32_t *prev_dists_avg);

uint16_t control(int32_t alt, int32_t peer_alt, int32_t *prev_dists, int32_t *prev_delta_dists);

ISR(TIMER1_COMPA_vect);

#endif
