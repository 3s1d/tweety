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

#include "debug.h"

uint16_t c[7];	//identical to C1..C6

int8_t ms5637_init(void)
{
	unsigned char messageBuf[4];

	/* enable power */
	OUTPUT(MS5637_PWR_PIN);
	SET(MS5637_PWR_PIN);
	_delay_ms(50);

	TWI_Master_Initialise();

	messageBuf[0] = (MS5637_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);		// The first byte must always consists of General Call code or the TWI slave address.
	messageBuf[1] = CMD_RESET;            						// The first byte is used for commands.
	TWI_Start_Transceiver_With_Data(messageBuf, 2);

	if(TWI_Get_State_Info() != TWI_NO_STATE)
		return -1;

	for(uint8_t i = 0; i < 7; i++)
	{
		messageBuf[0] = (MS5637_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	// The first byte must always consists of General Call code or the TWI slave address.
		messageBuf[1] = CMD_PROM_READ(i);      					// The first byte is used for commands.
		TWI_Start_Transceiver_With_Data(messageBuf, 2);

		//return value
		messageBuf[0] = (MS5637_ADDR<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
		TWI_Start_Transceiver_With_Data(messageBuf, 3);

		if(!TWI_Get_Data_From_Transceiver(messageBuf, 3))
			return -1;

		c[i] = ((uint16_t)messageBuf[1]) << 8;
		c[i] |= messageBuf[2];
	}

	debug_put((uint8_t *)c, 7*2);

	return 0;
}

void ms5637_deinit()
{
	/* disable TWI */
	TWI_Master_Deinitialise();

	/* disable power */
	OUTPUT(MS5637_PWR_PIN);
	CLR(MS5637_PWR_PIN);
}


uint32_t takeReading(uint8_t trigger_cmd)
{
	unsigned char messageBuf[4];

	/* start conversion */
	messageBuf[0] = (MS5637_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); 	// The first byte must always consists of General Call code or the TWI slave address.
	messageBuf[1] = trigger_cmd;           					 // The first byte is used for commands.
	TWI_Start_Transceiver_With_Data(messageBuf, 2);

	/* wait for conversion to be ready */
	_delay_ms(OSR_8192_TIME);

	/* read conversion */
	messageBuf[0] = (MS5637_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); 	// The first byte must always consists of General Call code or the TWI slave address.
	messageBuf[1] = CMD_READ_ADC;      					 // The first byte is used for commands.
	TWI_Start_Transceiver_With_Data(messageBuf, 2);

	/* get value */
	messageBuf[0] = (MS5637_ADDR<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
	TWI_Start_Transceiver_With_Data(messageBuf, 4);

	if(!TWI_Get_Data_From_Transceiver(messageBuf, 4))
		return 0;

	//sometime 1st try fails??

	uint32_t result = (uint32_t)messageBuf[1] << 16;
	result |= (uint32_t)messageBuf[2] << 8;
	result |= messageBuf[3];

	return result;
}

int32_t ms5637_get_pressure(void)
{
	int32_t d2 = takeReading(CMD_START_D2);

	if(d2 == 0)
		return 0;

	int64_t dt = d2 - c[5] * (1L<<8);

	int32_t temp = 2000 + (dt * c[6]) / (1L<<23);

	/* Second order temperature compensation */
	//int64_t t2;
	//if(temp >= 2000)
	//{
	//	/* High temperature */
	//	t2 = 5 * (dt * dt) / (1LL<<38);
	//}
	//else
	//{
	//	/* Low temperature */
	//	t2 = 3 * (dt * dt) / (1LL<<33);
	//}
	//temperature = (float)(temp - t2) / 100;

	int32_t d1 = takeReading(CMD_START_D1);
	if(d1 == 0)
		return 0;

	int64_t off = c[2] * (1LL<<17) + (c[4] * dt) / (1LL<<6);
	int64_t sens = c[1] * (1LL<<16) + (c[3] * dt) / (1LL<<7);

	/* Second order temperature compensation for pressure */
	if(temp < 2000)
	{
		/* Low temperature */
		int32_t tx = temp-2000;
		tx *= tx;
		int32_t off2 = 61 * tx / (1<<4);
		int32_t sens2 = 29 * tx / (1<<4);
		if(temp < -1500)
		{
			/* Very low temperature */
			tx = temp+1500;
			tx *= tx;
			off2 += 17 * tx;
			sens2 += 9 * tx;
		}
		off -= off2;
		sens -= sens2;
	}

	return ((int64_t)d1 * sens/(1LL<<21) - off) / (1LL << 15);
}
