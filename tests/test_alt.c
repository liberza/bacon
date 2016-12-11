#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "../lib/serial.h"
#include "../lib/controller.h"
#include "../lib/alt.h"

#define USART_BAUDRATE 1200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define ever ;;

// altimeter test.

int main(void)
{
	uint16_t measure_delay = 1000;

	serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);

	sei();

	for(ever)
	{
		put_byte('z');

		_delay_ms(measure_delay);
	}
}
