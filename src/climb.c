/*
 * climb.c
 *
 *  Created on: Jul 31, 2016
 *      Author: sid
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ms5637.h"
#include "climb.h"

#include "debug.h"

int32_t climb_buffer[CLIMB_SAMPLES];
uint8_t climb_buf_idx = 0;

int32_t d1,d2;

/* first */
ISR(TIMER0_COMPB_vect)
{

	/*get d2, trigger d2 */
	d2 = ms5637_get_reading_start_next(CMD_START_D1);
	/* reset timer */
	TCNT0 = OCR0B;

//	uint8_t t=1;
//	debug_put(&t, 1);
}

/* second */
ISR(TIMER0_COMPA_vect)
{

	/* get d1, trigger d2 */
	d1 = ms5637_get_reading_start_next(CMD_START_D2);
	/* reset timer */
	TCNT0 = 0;

	//uint8_t t=0;
	//debug_put(&t, 1);

	const int32_t p_pa = ms5637_get_pressure(d1, d2);
	const int32_t alt_cm = ms5637_p2alt(p_pa);		//10ms @ 1Mhz

	/* add new value to buffer */
	climb_buffer[climb_buf_idx] = alt_cm;
	if(++climb_buf_idx >= CLIMB_SAMPLES)
		climb_buf_idx = 0;

	//debug_put((uint8_t *) &alt_cm, 4);
}

int32_t LR_den;
void climb_init(void)
{
	while(ms5637_init() != 0)
	{
		ms5637_deinit();
		_delay_ms(100);
	}

	/* configure timer0 to generate an interrupt every OSR_8192_TIME */
#if F_CPU >= 2000000UL
	/* CTC and div 1024 */
	TCCR0A = (1<<CTC0) | (5<<CS00);
	OCR0A = (2*OSR_8192_TIME*F_CPU)/(1000UL*1024UL);
#else
	/* CTC and div 256 */
	TCCR0A = (1<<CTC0) | (4<<CS00);
	OCR0A = (2*OSR_8192_TIME*F_CPU)/(1000UL*256UL)+1;
#endif
	OCR0B = OCR0A>>1;
	TIMSK0 = (1<<OCIE0B) | (1<<OCIE0A);


	//todo
	LR_den = 0;
	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)
	{
		LR_den += ((int64_t)i-LR_x_cross) * ((int64_t)i-LR_x_cross);
	}
//	LR_den *= 100.0f;				//unit of altitude is cm
}

/* we are using linear regression here */
int16_t climb_get(void)
{
	static int32_t climb;

	/* compute average altitude */
	int32_t avg_alt = 0;
	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)		//todo optimize
	{
		cli();
		volatile int32_t buf = climb_buffer[i];
		sei();

		avg_alt += buf;
	}
	avg_alt /= CLIMB_SAMPLES;
	debug_put((uint8_t *) &avg_alt, 3);

	/* compute LR numerator */
	int32_t LR_num = 0;
	uint8_t idx = climb_buf_idx;
	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)		//todo optimize
	{
		/* first increasing, gives us a little time buffer */
		if(++idx >= CLIMB_SAMPLES)
			idx = 0;

		cli();
		volatile int32_t buf =	climb_buffer[idx];
		sei();

		LR_num += ((int32_t)i-LR_x_cross) * (buf - avg_alt);
	}

	/* average climb and migrate */
	climb += (LR_num*CLIMB_SAMPLES_PER_SEC) / LR_den;
	climb /= 2;

	return (int16_t) climb;
}
