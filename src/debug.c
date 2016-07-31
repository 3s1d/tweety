/*
 * debug.c
 *
 *  Created on: Jul 31, 2016
 *      Author: sid
 */

#include <avr/io.h>

#include "flexport.h"
#include "debug.h"

#ifdef DEBUG

/*
 * I2C mod, use a logic analyzer for evaluation
 */

void debug_put(uint8_t *buf, uint8_t length)
{
	/* initialize pins */
	OUTPUT(DEBUG_SCL_PIN);
	OUTPUT(DEBUG_SDA_PIN);
	SET(DEBUG_SCL_PIN);
	SET(DEBUG_SCL_PIN);

	/* start condition */
	CLR(DEBUG_SDA_PIN);

	for(uint8_t byte=0; byte<length; byte++)
	{
		for(int8_t bit=7; bit>=0; bit--)
		{
			CLR(DEBUG_SCL_PIN);

			/* set bit */
			if(buf[byte] & 1<<bit)
				SET(DEBUG_SDA_PIN);
			else
				CLR(DEBUG_SDA_PIN);

			SET(DEBUG_SCL_PIN);
		}

		/* ack bit */
		CLR(DEBUG_SCL_PIN);
		CLR(DEBUG_SDA_PIN);
		SET(DEBUG_SCL_PIN);
		CLR(DEBUG_SCL_PIN);

	}

	/* stop condition */
	SET(DEBUG_SCL_PIN);
	SET(DEBUG_SDA_PIN);
}




#endif
