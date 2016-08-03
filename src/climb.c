/*
 * climb.c
 *
 *  Created on: Jul 31, 2016
 *      Author: sid
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "climb.h"
#include "ms5637.h"
#include "piezo.h"

#include "debug.h"

int32_t climb_buffer[CLIMB_SAMPLES];
volatile uint8_t climb_buf_idx;
volatile int32_t d1,d2;

/* climb rate in cm/s */
int16_t climb_cms;

/* first */
ISR(TIMER0_COMPB_vect)
{

	/*get d2, trigger d2 */
	d2 = ms5637_get_reading_start_next(CMD_START_D1);

	/* call piezo */
	p_climb();

//	uint8_t t=1;
//	debug_put(&t, 1);
}

/* second */
ISR(TIMER0_COMPA_vect)
{

	/* get d1, trigger d2 */
	d1 = ms5637_get_reading_start_next(CMD_START_D2);

	//uint8_t t=0;
	//debug_put(&t, 1);

	const int32_t p_pa = ms5637_get_pressure(d1, d2);

#ifdef CLIMB_TEST
	volatile int32_t alt_cm = ms5637_p2alt(p_pa);

	/* emulate altitude */
	static int32_t test_alt = C_START_ALT_CM;
	alt_cm = test_alt;

	/* add delta altitude */
	test_alt += C_ADD;
#else
	const int32_t alt_cm = ms5637_p2alt(p_pa);		//10ms @ 1Mhz
#endif

	/* add new value to buffer */
	climb_buffer[climb_buf_idx] = alt_cm;
	if(++climb_buf_idx >= CLIMB_SAMPLES)
		climb_buf_idx = 0;

	//debug_put((uint8_t *) &alt_cm, 4);
}

//int32_t LR_den;
void climb_init(void)
{
	while(ms5637_init() != 0)
	{
		ms5637_deinit();
		_delay_ms(100);
	}

	/* set index to a known position */
	climb_buf_idx = 0;
	climb_cms = 0;

	/* configure timer0 to generate an interrupt every OSR_8192_TIME */
	TCNT0 = 0;
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

//	LR_den = 0;
//	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)
//		LR_den += ((int64_t)i-LR_x_cross) * ((int64_t)i-LR_x_cross);

	/* pre-fill ring buffer */
	/* wait a few  altitude measurements and replicate them */
	//note: the first measurement will fail
	//note2: a proper solution requires to many flash due to the inlineing of ms5637_p2alt()
	while(climb_buf_idx < 5);
	cli();
	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)
		climb_buffer[i] = climb_buffer[4];
	sei();
}

void climb_deinit(void)
{
	/* stop timer0 */
	TCCR0A = 0;

	/* ensure piezo is quiet */
	p_off();

	/* stop ms5637 */
	ms5637_deinit();
}

/* we are using linear regression here */
void climb_update(void)
{
	/* compute average altitude */
	int32_t avg_alt = 0;
	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)
	{
		cli();
		volatile int_fast32_t buf = climb_buffer[i];
		sei();

		avg_alt += buf;
	}
	avg_alt /= CLIMB_SAMPLES;
	//debug_put((uint8_t *) &avg_alt, 3);

	/* compute LR numerator */
	int32_t LR_num = 0;
	uint8_t idx = climb_buf_idx;
	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)
	{
		/* first increasing, gives us a little time buffer */
		if(++idx >= CLIMB_SAMPLES)
			idx = 0;

		cli();
		volatile int_fast32_t buf = climb_buffer[idx];
		sei();

		LR_num += ((int32_t)i-LR_x_cross) * (buf - avg_alt);
	}

	/* final climb value */
	volatile const int_fast16_t climb = (LR_num*CLIMB_SAMPLES_PER_SEC) / LR_den;

	/* make publicly available */
	cli();
	climb_cms = climb;
	sei();
}
