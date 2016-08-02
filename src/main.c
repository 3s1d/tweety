/*
 * main.c
 *
 *  Created on: Jun 22, 2016
 *      Author: sid
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>

#include "flexport.h"
#include "battery.h"
#include "button.h"
#include "climb.h"
#include "piezo.h"
#include "sleep.h"

#include "debug.h"


int main(void)
{
	//todo: auto off
	//todo: config or go to sleepmode

	sei();

	/* enable pull-ups for all unused ports (reduces standby power) */
	//note: pins are already set as input
	PORTA = 0x0F;	//all ports
	PORTB = 0xFD;	//all except B,1 (piezo)
	PORTC = 0xCE;	//all except C,0 (ms5637 supply), C,4+5 (TWI)
	PORTD = 0xFF;	//all D,2 requires a pull-up anyway

	uint8_t t=0;
	debug_put(&t, 1);

	/* boot */
	btn_init();
	p_init();
	climb_init();

	/* further reduce power */
	power_spi_disable();
	power_adc_disable();

	uint8_t pressed = 0;

	while(1)
	{
		//todo make global variable
		const int16_t climb_cms = climb_get();

		/* todo beep */
		p_off();

		debug_put((uint8_t *) &climb_cms, sizeof(uint16_t));

		/* handle button */
		if(btn_pressed())
			pressed++;
		else
			pressed = 0;

		/* note: value heavily depends on F_CPU */
		if(pressed > 3)
		{
			sleep();
			pressed = 0;

			/* indicate battery state */
			//note: the previous 'power on'-beep is important as it stresses the battery a little
			_delay_ms(200);		//it's a little longer, the cpu has some heavy interrupt stuff going on
			p_beep(battery());
		}
	}
	return 1;
}


