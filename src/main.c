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

	while(1)
	{
//		while(!btn_pressed());

		const int16_t climb_cms = climb_get();

		debug_put((uint8_t *) &climb_cms, sizeof(uint16_t));

		if(btn_pressed())
			sleep();

//		if(climb_ms > 0.0f)
//			_delay_us(99);
//		else
//			_delay_us(100);
//		_delay_ms(200);
		p_off();
//		_delay_ms(200);
	}
	return 1;
}


