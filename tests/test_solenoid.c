#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "../status.h"
#include "../controller.h"

#define ever ;;

// Solenoid test.
// Solenoid activates on green, deactivates on red.
int main(void)
{
    uint16_t ballast_time = 1000;
    uint16_t led_delay = 500;

    status_pin_init();
    status(STATUS0 | STATUS1 | STATUS2);
    tim_init();
    solenoid_init();
    sei();

    for(ever)
    {
        status(STATUS1 | STATUS2);
        activate_solenoid(ballast_time);
        _delay_ms(led_delay);

        status(STATUS0 | STATUS2);
        _delay_ms(led_delay);

        status(STATUS0 | STATUS1);
        _delay_ms(led_delay);

        status(STATUS0 | STATUS1 | STATUS2);
        _delay_ms(led_delay);
    }
}
