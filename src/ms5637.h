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

int8_t ms5637_init(void);
void ms5637_deinit();

uint32_t ms5637_get_reading_start_next(uint8_t next_cmd);

int32_t ms5637_get_pressure(void);

#endif /* MS5637_H_ */
