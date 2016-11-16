#ifndef ALT_H
#define ALT_H

#include <avr/io.h>
#include <interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Initializes SPI communication for master (AVR) 
 */
void spi_master_init(
	void
	);

/*
 * Function connects to the Altimeter (slave),
 * and returns the current altitude.
 *
 */
uint8_t measure(
	uint8_t buffer
	);

#endif
