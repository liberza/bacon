#include "alt.h"

void spi_master_init(void)
{
	// MOSI, SCK, and SS as output
	DDRB |= (1 << PB3)|(1 << PB5)|(1 << PB2); 
	
	// SS to high (disconnects slave)
	PORTB |= (1 << PB2);

	// enable SPI (SPE)
	// set to master (MSTR)
	// SCK to f/16 (SPR0)
	SPCR = (1 << SPE)|(1 << MSTR)|(1 << SPR0);
}

uint8_t measure(uint8_t buffer)
{	
	// SS to low (connects slave)
	PORTB &= ~(1 << PB2);
	
	// loads SPI data register into buffer
	buffer = SPDR;
	
	// waits until transaction completes
	while (!(SPSR & (1 << SPIF)));

	// SS to high again (disconnects slave)
	PORTB |= (1 << PB2);

	return buffer;
}


