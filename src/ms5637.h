/*
 * ms5637.h
 *
 *  Created on: Jun 28, 2016
 *      Author: sid
 */

#ifndef MS5637_H_
#define MS5637_H_

/* i2c address of module */
#define MS5637_ADDR 		0x76

#define MS5637_PWR_PIN		C,0

/* module commands */
#define CMD_RESET 		0x1E
#define CMD_PROM_READ(offs) 	(0xA0+(offs<<1)) /* Offset 0-7 */
#define CMD_START_D1 		0x4A	//osr = 8192
#define CMD_START_D2		0x5A	//osr = 8192
#define CMD_READ_ADC		0x00

#define OSR_8192_TIME		17

//in centimeter
#define ms5637_p2alt(p)		( (1.0f - pow((float)p/101325.0f, 0.190295f)) * 4433000.0f )

extern uint16_t c[7];

int8_t ms5637_init(void);
void ms5637_deinit();

uint32_t ms5637_get_reading_start_next(uint8_t next_cmd);

inline int32_t ms5637_get_pressure(int32_t d1, int32_t d2)
{
	if(d1 == 0 || d2 == 0)
		return 0;

	int32_t dt = d2 - ((uint32_t)c[5]<<8);
	int32_t temp = 2000 + (int32_t)(((int64_t)dt * (uint64_t)c[6])>>23);

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

	int64_t off = ((uint64_t)c[2]<<17) + ((dt * ((uint64_t)c[4]))>>6);
	int64_t sens = ((uint64_t)c[1]<<16) + ((dt * ((uint64_t)c[3]))>>7);

	/* Second order temperature compensation for pressure */
	if(temp < 2000)
	{
		/* Low temperature */
		int32_t tx = temp-2000;
		tx *= tx;
		int32_t off2 = (61 * tx) >> 4;
		int32_t sens2 = (29 * tx) >> 4;
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

	return ((((int64_t)d1 * sens)>>21) - off) >> 15;
}

#endif /* MS5637_H_ */
