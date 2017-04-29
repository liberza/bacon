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
#include "../lib/xbee.h"

#define USART_BAUDRATE 1200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define ever ;;
#define BUF_LEN (uint16_t)30
#define ALT_SAMPLES (uint16_t)1

// altimeter test.
void wipe(char *buffer, uint16_t len);

int main(void)
{
	char buffer[BUF_LEN];
	int32_t altitude;
    uint16_t coeffs[8];
	serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);
    xbee_init();
	sei();

	wipe(buffer, BUF_LEN);

	alt_init(coeffs);
	for(ever)
	{
		altitude = get_alt(coeffs);

        sprintf(buffer, "%ld", altitude);
		tx((uint8_t*)buffer, BUF_LEN, BROADCAST, 0x00);

		wipe(buffer, BUF_LEN);
	}
}

void wipe(char *buffer, uint16_t len)
{
	for (int i = 0; i < len; i++) buffer[i] = 0;
}
