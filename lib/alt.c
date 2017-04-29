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

int32_t cmd_adc(char cmd)
{
	int8_t ret;
	int32_t temp;

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
	//_delay_ms(2);
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

uint16_t cmd_prom(int coef_num)
{
	uint8_t ret;
	uint16_t rC;

	rC = 0;

	csb_lo();
	spi_cmd_send(CMD_PROM_RD + coef_num*2);
    _delay_ms(1);
	spi_cmd_send(0x00);
    _delay_ms(1);
	ret = SPDR;
	rC = 256 * ret;

	spi_cmd_send(0x00);
    _delay_ms(1);
	ret = SPDR;

	rC = rC + ret;
	csb_hi();

	return rC;
}

// Expects a uint16_t* of length 8.
void alt_init(uint16_t *C)
{
	// SS, MOSI, and SCK as outputs
	DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);

	// MISO as input
	DDRB &= ~(1 << PB4);

	// SPI settings: master, mode 0, f/4
	SPCR = (1 << SPE) | (1 << MSTR);

	cmd_reset();
    _delay_ms(5);

    for (int i=0; i<8; i++) {
        C[i] = cmd_prom(i);
        _delay_ms(1);
    }
}

// Expects a uint16_t* of length 8.
int32_t get_alt(uint16_t *C)
{
	int32_t altitude;
	uint32_t D1;
	uint32_t D2;
	int32_t P;
	int64_t dT;
	int64_t OFF;
	int64_t SENS;
	
	D1 = cmd_adc(CMD_ADC_D1 + CMD_ADC_4096);
	D2 = cmd_adc(CMD_ADC_D2 + CMD_ADC_4096);

	dT = D2 - C[5]*pow(2,8);
	OFF = C[2]*pow(2,17) + dT*C[4]/pow(2,6);
	SENS = C[1]*pow(2,16) + dT*C[3]/pow(2,7);
	
	//T = (2000 + (dT*C[6])/pow(2,23));
	
	P = (((D1*SENS)/pow(2,21) - OFF)/pow(2,15));
			
	altitude = -44330.77*(pow((long double)P/101325,0.190252)-1);
								
	return altitude;
}
