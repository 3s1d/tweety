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

volatile uint16_t c[7];	//identical to C1..C6
volatile int64_t c1_r16, c2_r17;
volatile int32_t c5_r8;

/*
unsigned char ms5637::crc4(unsigned int n_prom[]) {
    unsigned int n_rem;
    unsigned int crc_read;
    unsigned char n_bit;
    n_rem = 0x00;
    crc_read = n_prom[7];
    n_prom[7]=(0xFF00 & (n_prom[7]));
    for (int cnt = 0; cnt < 16; cnt++) {
            if (cnt%2 == 1) {
                n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
            } else {
                n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
            }
            for (n_bit = 8; n_bit > 0; n_bit--) {
                if (n_rem & (0x8000)) {
                    n_rem = (n_rem << 1) ^ 0x3000;
                } else {
                    n_rem = (n_rem << 1);
                }
            }
        }
    n_rem= (0x000F & (n_rem >> 12));
    n_prom[7]=crc_read;
    return (n_rem ^ 0x0);
}
*/
/*
The CRC code is calculated and written in factory with the LSB byte in the prom n_prom[7] set to 0x00 (see
Coefficient table below). It is thus important to clear those bytes from the calculation buffer before proceeding
with the CRC calculation itself:
n_prom[7]=(0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
As a simple test of the CRC code, the following coefficient table could be used:
unsigned int nprom[] = {0x3132,0x3334,0x3536,0x3738,0x3940,0x4142,0x4344,0x4500};
the resulting calculated CRC should be 0xB.

DB  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
Addr
0               16 bit reserved for manufacturer
1               Coefficient 1 (16 bit unsigned)
2               Coefficient 2 (16 bit unsigned)
3               Coefficient 3 (16 bit unsigned)
4               Coefficient 4 (16 bit unsigned)
5               Coefficient 5 (16 bit unsigned)
6               Coefficient 6 (16 bit unsigned)
7                                   0   0   0   0     CRC(0x0)
*/
/*
    //Returns 0x0b as per AP520_004
    C[0] = 0x3132;
    C[1] = 0x3334;
    C[2] = 0x3536;
    C[3] = 0x3738;
    C[4] = 0x3940;
    C[5] = 0x4142;
    C[6] = 0x4344;
    C[7] = 0x4546;
    n_crc = ms.crc4(C); // calculate the CRC
    pc.printf("testing CRC: 0x%x\n", n_crc);
*/



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
	_delay_ms(10);

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
	c1_r16 = ((int64_t) c[1]) << 16;
	c2_r17 = ((int64_t) c[2]) << 17;
	c5_r8 = ((int32_t) c[5]) << 8;

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
