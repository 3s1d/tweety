/*
 * main.c
 *
 *  Created on: Jun 22, 2016
 *      Author: sid
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

#include "flexport.h"
#include "ms5637.h"
#include "piezo.h"
#include "button.h"
#include "climb.h"

#include "debug.h"


int main(void)
{
	/* clock system */
	//TWHSR = 1
	//twi high speed requires i/o clock of 4mhz in case of 8mhz internal clock

	sei();

	uint8_t t=0;
	debug_put(&t, 1);

	btn_init();
	p_init();
	climb_init();

	while(1)
	{
//		while(!btn_pressed());

		const int16_t climb_cms = climb_get();

		debug_put((uint8_t *) &climb_cms, sizeof(uint16_t));

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


