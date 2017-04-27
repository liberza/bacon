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
	uint16_t measure_delay = 500;
	char buffer[BUF_LEN];
	int32_t altitude;
	uint32_t D1;
	uint32_t D2;
	uint16_t C[8];
    float alts[ALT_SAMPLES];
    float alt_sum = 0;
	int32_t P;
	int32_t T;
	int32_t T2;
	int32_t dT;
	int64_t OFF;
	int64_t OFF2;
	int64_t SENS2;
	int64_t SENS;
	int i;
	serial_init(BAUD_PRESCALE, DATA_BITS_8, STOP_BITS_1, PARITY_DISABLED);
    xbee_init();
	sei();

	for (i = 0; i < 8; i++){ C[i] = 0;}
    for (i = 0; i < ALT_SAMPLES; i++) { alts[i] = 0.0; }
	wipe(buffer, BUF_LEN);
	// SS, MOSI, and SCK as outputs
	DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
		
	// MISO as input
	DDRB &= ~(1 << PB4);
						
	// SPI settings: master, mode 0, f/4
	SPCR = (1 << SPE) | (1 << MSTR);
									
	// module should be reset upon power up.
	cmd_reset(); 
												
	for ( i = 0; i < 8; i++){
		C[i] = cmd_prom(i);	
	}	
	
	for(ever)
	{
        D1 = cmd_adc(CMD_ADC_D1 + CMD_ADC_4096); // read uncompensated pressure
        D2 = cmd_adc(CMD_ADC_D2 + CMD_ADC_4096);// read uncompensated temperature
        
        // calculate 1st order pressure and temperature (datasheet)
        dT = D2 - C[5]*pow(2,8);
        OFF = C[2]*pow(2,17) + dT*C[4]/pow(2,6);
        SENS = C[1]*pow(2,16) + dT*C[3]/pow(2,7);
		
		T = (2000 + (dT*C[6])/pow(2,23));

		// calculate 2nd order pressure and temperature TEMP < 20C
		//T2 = pow(dT,2)/pow(2,31);
		//OFF2 = 61*pow((T-2000),2)/pow(2,4);
		//SENS2 = 2*pow((T-2000),2);

		// calculate 3rd order pressure and temperature TEMP < -15C
		//OFF2 = OFF2 + 15*pow((T+1500),2);
		//SENS2 = SENS2 + 8*pow((T+1500),2);

		//T = T - T2;
		//OFF = OFF - OFF2;
		//SENS = SENS - SENS2;

		//P = (D1 * SENS) - OFF;
        P = (((D1*SENS)/pow(2,21) - OFF)/pow(2,15));
        
		altitude = -443307.7*(pow((long double)P/101325,0.190252)-1);
		

        sprintf(buffer, "%ld", altitude);
		tx((uint8_t*)buffer, BUF_LEN, BROADCAST, 0x00);

		_delay_ms(measure_delay);
		wipe(buffer, BUF_LEN);
	}
}

void wipe(char *buffer, uint16_t len)
{
	for (int i = 0; i < len; i++) buffer[i] = 0;
}
