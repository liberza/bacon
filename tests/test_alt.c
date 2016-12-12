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
void wipe(char buf[30]);

int main(void)
{
	uint16_t measure_delay = 1000;
	char buffer[30];
	buffer[29] = '\n';
	
	int32_t altitude;
	uint32_t D1;
	uint32_t D2;
	uint16_t C[8];
	int32_t P;
	int32_t T;
	int32_t dT;
	int64_t OFF;
	int64_t SENS;
	int i;
	serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);
	sei();

	for (i = 0; i < 8; i++){ C[i] = 0;}
	wipe(buffer);
	// SS, MOSI, and SCK as outputs
	DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
		
	// MISO as input
	DDRB &= ~(1 << PB4);
						
	// SPI settings: master, mode 0, f/4
	SPCR = (1 << SPE) | (1 << MSTR);
									
	// module should be reset upon power up.
	//cmd_reset(); 
												
	for ( i = 0; i < 8; i++){
		C[i] = cmd_prom(i);
	}	
	
	for(ever)
	{
		D1 = cmd_adc(CMD_ADC_D1 + CMD_ADC_256); // read uncompensated pressure
		D2 = cmd_adc(0x14);						// read uncompensated temperature
		
		// calculate 1st order pressure and temperature (datasheet)
		dT = D2 - C[5]*pow(2,8);
		OFF = C[2]*pow(2,17) + dT*C[4]/pow(2,6);
		SENS = C[1]*pow(2,16) + dT*C[3]/pow(2,7);

		T = (2000 + (dT*C[6])/pow(2,23));
		P = (((D1*SENS)/pow(2,21) - OFF)/pow(2,15));
		
		/* 
		 * Calculate altitude using simplified barometric equation:
		 *
		 * 					  /         1/5.255\
		 * altitude = 44330 * \1 - (p/p0)      /
		 *
		 */

		P /= P0;
		P /= 100;
		P = pow(P, (1/5.255));
		P = 1 - P;
		altitude = 44330 * P;
		sprintf(buffer, "%ld", altitude);

		for (i = 0; i < 30; i++){
			put_byte(buffer[i]);
			//put_byte('q');
		}

		_delay_ms(measure_delay);
		wipe(buffer);
	}
}

void wipe( char buf[30] )
{
	for (int i = 0; i < 29; i++) buf[i] = 0;
}
