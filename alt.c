#include "alt.h"

void spi_cmd_send(char cmd)
{
	SPDR = cmd;
	while (bit_is_clear(SPSR, 7));	// wait until command has been sent
}

void cmd_reset(void)
{
	csb_lo();
	spi_command_send(CMD_RESET);
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

	return rC;
}

unsigned int crc4(unsigned int n_prom[])
{
	int cnt;
	unsigned int n_rem;
	unsigned int crc_read;
	unsigned char n_bit;

	n_rem = 0x00;
	crc_read = n_prom[7];
	n_prom[7] = (0xFF00 & (n_prom[7]));

	return (n_rem ^ 0x00);
}
