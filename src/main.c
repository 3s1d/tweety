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



int main(void)
{
	/* clock system */
	//TWHSR = 1
	//twi high speed requires i/o clock of 4mhz in case of 8mhz internal clock

	sei();

	btn_init();
	p_init();

	//todo
	while(ms5637_init() != 0)
	{
		ms5637_deinit();
		p_set(1000);
		_delay_ms(10);
		p_off();
		_delay_ms(100);
	}



	uint16_t freq = 3000;

	while(1)
	{
		while(!btn_pressed());

//		int32_t p = ms5637_get_pressure();
//		float baroalt = (1.0f - pow((float)p/101325.0f, 0.190295f)) * 4433000.0f; //centimeter

		p_set(freq);
		_delay_ms(200);
		p_off();
		_delay_ms(200);

		freq /= 2;
		if(freq < 300)
			freq = 3000;
	}
	return 1;
}


