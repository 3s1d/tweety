/*
 * climb.c
 *
 *  Created on: Jul 31, 2016
 *      Author: sid
 */


#include <avr/io.h>

#include "climb.h"

int32_t climb_buffer[CLIMB_SAMPLES];
uint8_t climb_buf_idx = 0;
float climb = 0.0;

/* we are using linear regression here */

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
