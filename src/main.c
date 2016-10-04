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
#include "main.h"

#include "debug.h"

volatile int t;

int main(void)
{
	/* enable pull-ups for all unused ports (reduces standby power) */
	//note: pins are already set as input
	PORTA = 0x0F;	//all ports
	PORTB = 0xFD;	//all except B,1 (piezo)
	PORTC = 0xCE;	//all except C,0 (ms5637 supply), C,4+5 (TWI)
	PORTD = 0xFF;	//all D,2 requires a pull-up anyway

	/* further reduce power consumption */
	power_spi_disable();
	power_adc_disable();

	/* Switch to 2Mhz */
	CLKPR = (1<<CLKPCE);
#if F_CPU == 2000000UL
	CLKPR = 2;		//div 4 -> 2Mhz
#elif F_CPU == 1000000UL
	//no need to change anything
#else
	#error adjust prescaler.
#endif

	/* Enable interrupts */
	sei();

	/* boot */
	btn_init();
	p_init();
	if(btn_pressed())
		p_config();
	climb_init();

	//debug_put(&p_dosink, 1);

	/* auto off */
	uint16_t idle_time = 0;

	/* button counter; instantaneously go into sleep mode @ power-up */
	uint8_t pressed = UINT8_MAX-1;

	while(1)
	{
		/* update climb rate */
		climb_update();

		/* check for flight condition */
		if((climb_cms < 80) && (climb_cms > -80))
			idle_time++;
		else
			idle_time = 0;

		//debug_put((uint8_t *) &climb_cms, sizeof(uint16_t));

		/* note: values heavily depends on F_CPU */
		/* also switch off about 30 minutes without climb or sink */
		if((pressed > 5) || (idle_time > MAINLOOP_HALFHOUR))
		{
			sleep();

			/* reset enter states */
			pressed = 0;
			idle_time = 0;
		}

		/* handle button */
		if(btn_pressed())
			pressed++;
		else
			pressed = 0;

	}
	return 1;
}


