/*
 * piezo.c
 *
 *  Created on: Jul 30, 2016
 *      Author: sid
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "flexport.h"
#include "piezo.h"

uint8_t p_dosink;

void p_init(void)
{
	/* set pwm as output and clear it */
	OUTPUT(PIEZO_PIN);
	CLR(PIEZO_PIN);

	/* set volume */
	OCR1A = PIEZO_VOL;

	/* load config */
	p_dosink = eeprom_read_byte((uint8_t *) 0);
}

void p_set(uint16_t freq)
{
	/* set duty cycle */
	ICR1 = F_CPU / freq;

	/* configure hardware */
	/* clear OC1A on match */
	TCCR1A = (1<<COM1A1) | (1<<WGM11);

	/* fast pwm, full speed */
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS10);
}

void p_off(void)
{
	/* normal pin behavior */
	TCCR1A = 0;

	/* disable hardware */
	TCCR1B = 0;

	//todo clr pin?
}


/*
 * note: processor under heavy load -> _delay_ms() takes a lot more time!
 */
void p_hello(void)
{
	p_set(1000);
	_delay_ms(100);
	p_off();
	_delay_ms(20);
	p_set(2000);
	_delay_ms(100);
	p_off();
}

void p_bye(void)
{
	p_set(1000);
	_delay_ms(100);
	p_off();
	_delay_ms(20);
	p_set(500);
	_delay_ms(100);
	p_off();
}

void p_beep(uint8_t n)
{
	for(uint8_t i=0; i<n; i++)
	{
		p_set(1000);
		_delay_ms(100);
		p_off();
		_delay_ms(50);
	}
}
