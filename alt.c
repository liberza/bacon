#include "alt.h"

void spi_command_send(char cmd)
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
