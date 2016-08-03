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
#include "climb.h"
#include "button.h"
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
	p_dosink = eeprom_read_byte((uint8_t *) PIEZO_SINK_EEPROM);
}

void p_config(void)
{
	/* wait for user releasing the button */
	p_set(500);
	while(btn_pressed());
	p_off();

	uint8_t pressed = 0;
	while(1)
	{
		/* we need to execute btn_pressed() as often as possible */
		/* 2 beeps for sinking */
		for(uint8_t i=0; i<5; i++)
		{
			if((i>=3 && p_dosink) || i>=4)
				p_beep(1);
			else
				_delay_ms(300);

			if(btn_pressed())
			{
				/* longpress -> exit */
				if(++pressed >= 10)
				{
					/* store config */
					eeprom_write_byte((uint8_t *) PIEZO_SINK_EEPROM, p_dosink);

					return;
				}
			}
			else if(pressed && !btn_pressed())
			{
				/* toggle sink */
				p_dosink = !p_dosink;

				pressed = 0;
			}
		}
	}
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
}

void p_hello(void)
{
	p_set(1000);
	_delay_ms(250);
	p_off();
	_delay_ms(75);
	p_set(2000);
	_delay_ms(250);
	p_off();
}

void p_bye(void)
{
	p_set(1000);
	_delay_ms(250);
	p_off();
	_delay_ms(75);
	p_set(500);
	_delay_ms(250);
	p_off();
}

void p_beep(uint8_t n)
{
	for(uint8_t i=0; i<n; i++)
	{
		p_set(1000);
		_delay_ms(250);
		p_off();
		_delay_ms(75);
	}
}

/* called every 40ms */
void p_climb(void)
{
	/* todo */
	static uint8_t state = 0;

	if(climb_cms > 10)
		p_set(2000);
	else if(p_dosink && climb_cms < -250)
		p_set(500);
//	else if(state & 1)
//		p_set(1000);
	else
		p_off();


	state++;
}
