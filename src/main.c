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
#include "button.h"
#include "climb.h"
#include "piezo.h"
#include "sleep.h"

#include "debug.h"


int main(void)
{
	//todo: auto off

	sei();

	/* enable pull-ups for all unused ports (reduces standby power) */
	//note: pins are already set as input
	PORTA = 0x0F;	//all ports
	PORTB = 0xFD;	//all except B,1 (piezo)
	PORTC = 0xCE;	//all except C,0 (ms5637 supply), C,4+5 (TWI)
	PORTD = 0xFF;	//all D,2 requires a pull-up anyway

	/* further reduce power consumption */
	power_spi_disable();
	power_adc_disable();

	/* boot */
	btn_init();
	p_init();
	if(btn_pressed())
		p_config();
	climb_init();

	//debug_put(&p_dosink, 1);

	/* reset auto off */
	rest_time = 0;

	/* instantaneously go into sleep mode */
	uint8_t pressed = UINT8_MAX-1;
	while(1)
	{
		/* update climb rate */
		climb_update();

		/* check for flight condition */
		if((climb_cms < 80) && (climb_cms > -80))
			rest_time++;
		else
			rest_time = 0;

		//debug_put((uint8_t *) &climb_cms, sizeof(uint16_t));

		/* note: value heavily depends on F_CPU */
		/* also switch off about 30 minutes without climb or sink */
		if((pressed > 5) || (rest_time > HALFHOUR))
		{
			sleep();
			pressed = 0;
		}

		/* handle button */
		if(btn_pressed())
			pressed++;
		else
			pressed = 0;

	}
	return 1;
}


