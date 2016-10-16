#include <avr/io.h>
#include <avr/interrupt.h>


// Change cpu freq
#define	F_CPU 1000000UL		// 1 MHz

#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void solenoid_init(void)
{
	//uint8_t err = 0;
	
	PORTD &= ~(1 << PB1);
	DDRB |= (1 << PB1);			
}

void activate_solenoid(double release_time)
{
	double i;

	PORTD |= (1 << PB1);
	
	for (i = 0; i < release_time; i++){
		_delay_ms(1000);
	}
	
	PORTD &= ~(1 << PB1);
}
