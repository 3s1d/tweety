/*
 * sleep.c
 *
 *  Created on: Aug 2, 2016
 *      Author: sid
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "flexport.h"
#include "button.h"
#include "climb.h"
#include "piezo.h"
#include "sleep.h"

#include "debug.h"

ISR(INT0_vect)
{

}

void sleep(void)
{
	p_bye();

	/* wait for button released */
	while(btn_pressed());

	/* prepare system */
	p_off();
	climb_deinit();

	/* configure button to trigger an ext interrupt (int0 @ low level)*/
	//EICRA = (0<<ISC00);
	EIMSK = 1<<INT0;

	/* debug */
	uint8_t d = 10;
	debug_put(&d, 1);

	/* sleep */
	uint8_t stay_off = 100;
	while(stay_off)
	{
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();

		//todo
	}

	/* debug */
	d = 11;
	debug_put(&d, 1);

	/* disable int0 */
	EIMSK = 0;

	/* start up system again */
	climb_init();

	p_hello();
}
