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

/* note: we are using 20ms instead of 16.44ms to be a little more stable against 'hops' and to provide a stable clock */
#define OSR_8192_TIME		20		//17

//in centimeter
#define ms5637_p2alt(p)		( (1.0f - pow((float)p/101325.0f, 0.190295f)) * 4433000.0f )

extern volatile uint16_t c[7];
extern volatile int64_t c1_r16, c2_r17;
extern volatile int32_t c5_r8;

int8_t ms5637_init(void);
void ms5637_deinit();

uint32_t ms5637_get_reading_start_next(uint8_t next_cmd);

inline int32_t ms5637_get_pressure(int32_t d1, int32_t d2)
{
	if(d1 == 0 || d2 == 0)
		return 0;

	int64_t dt = d2 - c5_r8;

	int64_t off = c2_r17 + ((dt * c[4])>>6);
	int64_t sens = c1_r16 + ((dt * c[3])>>7);

	/* we can only go for the second order in case of 2Mhz and above */
	//note: 1Mhz (w/o -15deg) would be possible by tweaking the sampling duration slightly
#if F_CPU >= 2000000UL
	int32_t temp = 2000 + (int32_t)((dt * c[6])>>23);

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
#endif

	return ((((int64_t)d1 * sens)>>21) - off) >> 15;
}

#endif /* MS5637_H_ */
