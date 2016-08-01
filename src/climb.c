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
float climb = 0.0;

int32_t d1,d2;

/* we are using linear regression here */

ISR(TIMER0_COMPA_vect)
{
	uint8_t t=0;
	debug_put(&t, 1);

	/* get d1, trigger d2 */
//	d1 = ms5637_get_reading_start_next(CMD_START_D2);
}

ISR(TIMER0_COMPB_vect)
{
	uint8_t t=1;
	debug_put(&t, 1);

	/*get d2, trigger d2 */
//	d2 = ms5637_get_reading_start_next(CMD_START_D1);
}

void climb_init(void)
{
	while(ms5637_init() != 0)
	{
		ms5637_deinit();
		_delay_ms(100);
	}

	/* configure timer0 to generate an interrupt every 17ms */
#if F_CPU >= 2000000UL
	/* CTC and div 1024 */
	TCCR0A = (1<<CTC0) | (5<<CS00);
	OCR0A = (2*OSR_8192_TIME*F_CPU)/(1000UL*1024UL);
#else
	/* CTC and div 256 */
	TCCR0A = (1<<CTC0) | (4<<CS00);
	OCR0A = (2*OSR_8192_TIME*F_CPU)/(1000UL*256UL);
#endif
	OCR0B = OCR0A>>1;
	TIMSK0 = (1<<OCIE0B) | (1<<OCIE0A);
}


float climb_update(int32_t alt_cm)
{
	/* add new value to buffer */
	climb_buffer[climb_buf_idx] = alt_cm;
	if(++climb_buf_idx >= CLIMB_SAMPLES)
		climb_buf_idx = 0;

	/* compute average altitude */
	int32_t avg_alt = 0;
	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)		//todo optimize
		avg_alt += climb_buffer[i];
	avg_alt /= CLIMB_SAMPLES;

	/* compute LR numerator */
	uint8_t idx = climb_buf_idx;
	int32_t LR_num = 0;
	for(uint8_t i=0; i<CLIMB_SAMPLES; i++)		//todo optimize
		LR_num += (i-LR_x_cross) * (climb_buffer[idx++] - avg_alt);

	/* average climb and migrate from cm to m/s (due to LR_den 100 factor ) */
	climb += ((float) (LR_num*CLIMB_SAMPLES_PER_SEC)) / LR_den;
	climb /= 2.0f;

	return climb;
}
