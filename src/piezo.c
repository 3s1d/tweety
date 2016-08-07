/*
 * piezo.c
 *
 *  Created on: Jul 30, 2016
 *      Author: sid
 */

#include <avr/io.h>
//#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "flexport.h"
#include "button.h"
#include "climb.h"
#include "ms5637.h"
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
	/* for some reason, this seems to be broken in avrlibc-2.0.0 */
	//p_dosink = eeprom_read_byte((uint8_t *) PIEZO_SINK_EEPROM);

	/* loads address */
	EEARL = PIEZO_SINK_EEPROM;

	/* read byte */
	EECR = 1<<EERE;
	p_dosink = EEDR;

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

					/* for some reason, this seems to be broken in avrlibc-2.0.0 */
					//eeprom_write_byte((uint8_t *) PIEZO_SINK_EEPROM, p_dosink);

					/* loads address */
					EEARL = PIEZO_SINK_EEPROM;
					/* loads current byte */
					EEDR = p_dosink;

					cli();
					/* master write enable */
					EECR = (1<<EEMPE);
					/* strobe eeprom write, must be within 4cycles -> no interrupts */
					EECR = (1<<EEMPE)|(1<<EEPE);
					sei();

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
	p_set(800);
	_delay_ms(250);
	p_off();
	_delay_ms(75);
	p_set(1000);
	_delay_ms(100);
	p_off();
	_delay_ms(75);
	p_set(1200);
	_delay_ms(100);
	p_off();
}

void p_bye(void)
{
	p_set(800);
	_delay_ms(200);
	p_off();
	_delay_ms(75);
	p_set(600);
	_delay_ms(150);
	p_off();
	_delay_ms(75);
	p_set(400);
	_delay_ms(100);
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


const int16_t vario_freq[] =
{
		42, 47, 49, 55, 64, 73, 88, 126, 145, 190, 296, 507, 745, 934, 977, 1019, 1070, 1122, 1155, 1223, 1257, 1342, 1394, 1446, 1502
};
const int16_t vario_paus[] =
{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 517, 402, 345, 254, 235, 214, 203, 178, 164, 137, 124, 106, 70, 32
};
const int16_t vario_leng[] =
{
		2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 1196, 160, 162, 196, 217, 181, 151, 136, 129, 115, 105, 75, 54, 43, 23
};

/* Sound profile */
/*
const int16_t vario_freq[] =
{
	127, 130, 133, 136, 146, 159, 175, 198, 234, 283, 344, 415, 564, 701, 788, 846, 894, 927, 955, 985, 1008, 1037, 1070, 1200, 1400
};
const int16_t vario_paus[] =
{
	140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 500, 420, 350, 300, 250, 220, 190, 170, 150, 130, 120, 110, 100
};
const int16_t vario_leng[] =
{
	40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 240, 180, 120, 100, 90, 80, 70, 65, 60, 55, 50, 45, 40
};
*/

/* linear approximation between two samples */
uint16_t get_near(int16_t vario, const int16_t * src)
{
	const int16_t fvario = vario/100;
	int8_t findex = fvario + 12;
	int16_t m = (vario-fvario*100) ;

	uint8_t index = findex;
	if (findex > 23)
	{
		index = 23;
		m = 1.0f;
	}

	if (findex < 0)
	{
		index = 0;
		m = 0.0f;
	}

	int16_t start = src[index];
	start = start + (((src[index + 1] - start)) * m) / 100;

	return start;
}

int8_t beep_on_off(void)
{
	/* state */
	static uint32_t time_ms = 0;
	static uint32_t last_change_ms = 0;
	static uint8_t beepswitch = BEEP_OFF;			//toggles between BEEP_ON and BEEP_OFF
	int8_t status = BEEP_NOCHANGE;

	/* advance time */
	time_ms += 2*OSR_8192_TIME;

	//p_dosink=1;
	if ((climb_cms >= CLIMBTRESHOLD) || (p_dosink && (climb_cms <= SINKTHRESHOLD)))
	{
		/* beeping */

		const uint32_t t = (uint32_t) get_near(climb_cms, beepswitch?vario_leng:vario_paus);
		if(time_ms >= last_change_ms + t)
		{
			/* toggle state */
			last_change_ms = time_ms;
			beepswitch = !beepswitch;

			status = beepswitch;
		}
	}
	else
	{
		/* intermediate state */

		if(beepswitch == BEEP_ON && time_ms > (last_change_ms + MINTONELENGTH))
		{
			/* turn off */
			last_change_ms = time_ms;
			beepswitch = BEEP_OFF;

			status = beepswitch;
		}
	}

	return status;
}

/* called every 40ms */
void p_climb(void)
{
	const int8_t p_on_off = beep_on_off();

	if (p_on_off == BEEP_ON)
		p_set(get_near(climb_cms, vario_freq));
	else if(p_on_off == BEEP_OFF)
		p_off();
}

