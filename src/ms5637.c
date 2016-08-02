/*
 * ms5637.c
 *
 *  Created on: Jun 28, 2016
 *      Author: sid
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "flexport.h"
#include "TWI_Master.h"
#include "ms5637.h"

uint16_t c[7];	//identical to C1..C6

int8_t ms5637_init(void)
{
	uint8_t messageBuf[2];

	/* enable power */
	OUTPUT(MS5637_PWR_PIN);
	SET(MS5637_PWR_PIN);
	_delay_ms(50);

	/* start TWI */
	TWIM_Init(TWI_FULL_SPEED);

	/* execute reset */
	messageBuf[0] = CMD_RESET;
	if(TWIM_write(MS5637_ADDR, messageBuf, 1) != TRUE)
		return -1;

	/* read prom */
	for(uint8_t i = 0; i < 7; i++)
	{
		/* set register for read */
		messageBuf[0] = CMD_PROM_READ(i);
		if(TWIM_write(MS5637_ADDR, messageBuf, 1) != TRUE)
			return -1;

		/* get value */
		if(TWIM_read(MS5637_ADDR, messageBuf, 2) != TRUE)
			return -1;

		c[i] = ((uint16_t)messageBuf[0]) << 8;
		c[i] |= messageBuf[1];
	}

	return 0;
}

void ms5637_deinit()
{
	/* disable TWI */
	TWIM_Deinit();

	/* disable power */
	//OUTPUT(MS5637_PWR_PIN);
	CLR(MS5637_PWR_PIN);
}


uint32_t ms5637_get_reading_start_next(uint8_t next_cmd)
{
	unsigned char messageBuf[3];
	uint32_t result;

	/* read conversion */
	messageBuf[0] = CMD_READ_ADC;
	if(TWIM_write(MS5637_ADDR, messageBuf, 1) == TRUE && TWIM_read(MS5637_ADDR, messageBuf, 3) == TRUE)
		result = ((uint32_t)messageBuf[0] << 16) | ((uint32_t)messageBuf[1] << 8) | messageBuf[2];
	else
		//todo
		result = 5;

	/* start conversion */
	messageBuf[0] = next_cmd;
	if(TWIM_write(MS5637_ADDR, messageBuf, 1) != TRUE)
		return 3;

	return result;
}
