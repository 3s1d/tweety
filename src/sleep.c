/*
 * sleep.c
 *
 *  Created on: Aug 2, 2016
 *      Author: sid
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "flexport.h"
#include "battery.h"
#include "button.h"
#include "climb.h"
#include "piezo.h"
#include "sleep.h"

#include "debug.h"


ISR(PCINT2_vect)
{

}

void sleep(void)
{
	/* prepare system */
	climb_deinit();
	_delay_ms(250);

	p_bye();

	/* wait for button released */
	while(btn_pressed());

	/* configure button to trigger an PC interrupt (PCINT18 @ D,2) */
	PCMSK2 = _BV(PCINT18);
	PCICR = _BV(PCIE2);

	//uint8_t d = 10;
	//debug_put(&d, 1);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	uint8_t sleep = 1;
	while(sleep)
	{
		/* sleep */
		sleep_mode();

		/* minimal button press time */
		_delay_ms(500);

		//d = 13;
		//debug_put(&d, 1);

		/* way too short */
		if(!btn_pressed())
			continue;

		/* quickly indicate we are up and running */
		//note: longer beeps confuses me...
		p_set(1000);
		_delay_ms(100);
		p_off();
		_delay_ms(100);

		for(uint8_t i=0; i < 50; i++)
		{
			/* button released in time -> power on */
			if(!btn_pressed())
			{
				sleep = 0;
				break;
			}
			_delay_ms(100);
		}
	}

	//d = 11;
	//debug_put(&d, 1);

	/* disable PC int */
	PCICR = 0;

	/* indicate wakeup */
	p_hello();

	/* start up system again */
	climb_init();
}
