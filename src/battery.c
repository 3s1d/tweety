/*
 * battery.c
 *
 *  Created on: Aug 3, 2016
 *      Author: sid
 */

#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

//#include "debug.h"

uint8_t battery(void)
{
	/* turn on power */
	power_adc_enable();

	/* configure adc */
	ADMUX = _BV(REFS0) | _BV(ADLAR) | 0x0E;			//Vref=VCC | left adjusted | 1.1V channel
	ADCSRA = _BV(ADEN);
	_delay_ms(1);						//minimum settle time

	/* taking the measurement */
	ADCSRA = _BV(ADEN) | _BV(ADSC) | (3<<ADPS0);		//div 8 (F_CPU=1MHz -> 125kHz Sample freq (recommended: 50-200kHz))
	while(ADCSRA&(1<<ADSC));
	uint8_t bat = ADCH;

	/* cleanup */
	ADCSRA = 0;
	power_adc_disable();

	//uint8_t u = 2810/bat;
	//debug_put(&u, 1);

	/*
	 * Determine return value
	 *
	 * V = 281/bat
	 * 2V = 140
	 * 3V = 93
	 */
	if(bat <= 104)						// >= 2.7V	50%
		return 2;
	else if(bat<=112)					// >= 2.5V	25%
		return 1;
	else
		return 0;
}
