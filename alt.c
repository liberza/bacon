#include "alt.h"

void spi_cmd_send(char cmd)
{
	SPDR = cmd;
	while (bit_is_clear(SPSR, 7));	// wait until command has been sent
}

void cmd_reset(void)
{
	csb_lo();
	spi_cmd_send(CMD_RESET);
	_delay_ms(3);
	csb_hi();
}

unsigned long cmd_adc(char cmd)
{
	unsigned int ret;
	unsigned long temp;

	temp = 0;
	
	csb_lo();
	spi_cmd_send(CMD_ADC_CONV + cmd);

	// delays based on command sent
	switch (cmd & 0x0f)
	{
		case CMD_ADC_256	:	_delay_us(900);
		case CMD_ADC_512	:	_delay_ms(3);
		case CMD_ADC_1024	:	_delay_ms(4);
		case CMD_ADC_2048	:	_delay_ms(6);
		case CMD_ADC_4096	:	_delay_ms(10);
	}
	
	// pull csb high and then low to finish conversion
	csb_hi();
	csb_lo();

	// sends ADC read command
	spi_cmd_send(CMD_ADC_READ);
	
	spi_cmd_send(0x00);		// 1st byte
	ret = SPDR;
	temp = 65536 * ret;

	spi_cmd_send(0x00);		// 2nd byte
	ret = SPDR;
	temp = temp + 256 * ret;

	spi_cmd_send(0x00);		// 3rd byte
	ret = SPDR;
	temp = temp + ret;

	csb_hi();	// finished conversion
	
	return temp;
}

unsigned int cmd_prom(char coef_num)
{
	unsigned int ret;
	unsigned int rC;

	rC = 0;

	csb_lo();
	spi_cmd_send(CMD_PROM_RD + coef_num*2);
	spi_cmd_send(0x00);
	ret = SPDR;
	rC = 256 * ret;

	spi_cmd_send(0x00);
	ret = SPDR;

	rC = rC + ret;
	csb_hi();

	return rC;
}

double read_sensor(void)
{
	unsigned long D1;
	unsigned long D2;
	unsigned int C[8];
	double P;
	//double T;
	double dT;
	double OFF;
	double SENS;

	int i;

	// SS, MOSI, and SCK as outputs
	DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
	
	// MISO as input
	DDRB &= ~(1 << PB4);
	
	// SPI settings: master, mode 0, f/4
	SPCR = (1 << SPE) | (1 << MSTR);
	
	// module should be reset upon power up.
	// cmd_reset(); 

	for ( i = 0; i < 8; i++){
		C[i] = cmd_prom(i);
	}
	
	// read temperature (uncompensated)
	D1 = cmd_adc(CMD_ADC_D1 + CMD_ADC_256);
	D2 = cmd_adc(CMD_ADC_D2 + CMD_ADC_4096);

	dT = D2 - C[5]*pow(2,8);
	OFF = C[2]*pow(2,17) + dT*C[4]/pow(2,6);
	SENS = C[1]*pow(2,16) + dT*C[3]/pow(2,7);

	//T = (2000 + (dT*C[6])/pow(2,23))/100;
	P = (((D1*SENS)/pow(2,21) - OFF)/pow(2,15))/100;

	return P;
}
