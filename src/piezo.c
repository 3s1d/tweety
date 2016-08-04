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
uint64_t milli_seconds = 0;

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

int16_t mapDuration[][2] = { { 400, 100 }, { 200, 200 }, { 0, 300 }, { -500, 600 } };

uint16_t map_eval_duration(int16_t climb)
{
	uint8_t i, part;
	uint16_t duration;

	if (climb > 200) //climbing > 2m/s
	{
		duration = ((int32_t) 80000) / climb;
	}
	else
	{
		for (i = 0; mapDuration[i][0] > climb; i++)
			;
		part = ((climb - mapDuration[i][0])) / (mapDuration[i - 1][0] - mapDuration[i][0]);
		duration = (mapDuration[i][1] + (part * (mapDuration[i - 1][1] - mapDuration[i][1])));
	}
	if(((int16_t)duration - TONPAUSE) > (int16_t)duration / 2)
		return ((int16_t)duration - TONPAUSE);
	else
		return duration / 2;
}

uint8_t piepsen_on_off(void)
{
	uint8_t ret = 2;
	uint16_t pause, duration;
    static uint64_t ms_last_off, ms_last_on;
    static uint8_t sinking = 0, climbing = 0, beepswitch = 0;

    duration = map_eval_duration(climb_cms);

    //if (duration <= TONPAUSE)
    {
        pause = duration*1.5;
    }
    // Sinkton einschalten wenn er nicht an ist
    if ((climb_cms < SINKTRESHOLD) && (sinking == OFF))
    {
    		sinking = ON;
    }
    // Sinkton ausschalten wenn er an ist
    if ((climb_cms >= SINKTRESHOLD) && (sinking == ON))
    {
    		sinking = OFF;
    }
    // Steigton einschalten wenn er nicht an ist
    if ((climb_cms > CLIMBTRESHOLD) && (climbing == OFF))
    {
    		climbing = ON;
    }
    // Steigton ausschalten wenn er an ist
    if ((climb_cms <= CLIMBTRESHOLD) && climbing == ON)
    {
    		climbing = OFF;
    }

    //////////////////////////////////////////////////////////////////////////////////

    if (sinking == ON && (milli_seconds > (ms_last_off + MINTONELENGTH)))
    {
        ret = 1;
        ms_last_on = milli_seconds;
        beepswitch = ON;
    }
    else
    {
        if (climbing == ON)
        {
            if (beepswitch == ON)
            {
                if (milli_seconds > (ms_last_on + duration))
                {
                    ret = 0;
                    ms_last_off = milli_seconds;
                    beepswitch = OFF;
                }
            }
            else
            {
                if (milli_seconds > (ms_last_off + pause))
                {
                    ret = 1;
                    ms_last_on = milli_seconds;
                    beepswitch = ON;
                }
            }
        }
        else
        {
            if (!((sinking == ON) || (climbing == ON)) && (beepswitch == ON) && milli_seconds > (ms_last_on + MINTONELENGTH))
            {
                ret = 0;
                ms_last_off = milli_seconds;
                beepswitch = OFF;
            }
        }
    }
    return (ret);
}

/* called every 40ms */
void p_climb(void)
{
	uint8_t p_on_off;

	milli_seconds += 40;

	p_on_off = piepsen_on_off();

	if (p_on_off == ON)
	{
		p_set(STARTFREQUENCY + climb_cms / 1.5);
	}
	else if (p_on_off == OFF)
	{
		p_off();
	}
}
