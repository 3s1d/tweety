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

uint8_t battery(uint8_t rep)
{
	/* turn on power */
	power_adc_enable();

	/* configure adc */
	ADMUX = _BV(REFS0) | _BV(ADLAR) | 0x0E;			//Vref=VCC | left adjusted | 1.1V channel
	ADCSRA = _BV(ADEN);
	//_delay_ms(1);						//minimum settle time

	/* taking the measurement */
	uint16_t bat = 0;
	for(uint8_t i=0; i<rep; i++)
	{
		_delay_ms(10);
		ADCSRA = _BV(ADEN) | _BV(ADSC) | (3<<ADPS0);		//div 8 (F_CPU=1MHz -> 125kHz Sample freq (recommended: 50-200kHz))
		while(ADCSRA&(1<<ADSC));
		bat += ADCH;
	}
	bat /= rep;

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
	if(bat <= 112)						// >= 2.5V	50%
		return 2;
	else if(bat<=135)					// >= 2.08V	25%
		return 1;
	else
		return 0;
}
