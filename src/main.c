/*
 * main.c
 *
 *  Created on: Jun 22, 2016
 *      Author: sid
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "flexport.h"
#include "button.h"
#include "climb.h"
#include "piezo.h"
#include "sleep.h"

#include "debug.h"


int main(void)
{
	//todo sleep: enable pull ups oder set i/o as output

	sei();

	uint8_t t=0;
	debug_put(&t, 1);

	/* boot */
	btn_init();
	p_init();
	climb_init();

	uint8_t pressed = 0;

	while(1)
	{
		const int16_t climb_cms = climb_get();

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
		}


		/* todo beep */
		p_off();
	}
	return 1;
}


