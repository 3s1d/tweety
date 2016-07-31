/*
 * piezo.h
 *
 *  Created on: Jul 30, 2016
 *      Author: sid
 */

#ifndef PIEZO_H_
#define PIEZO_H_

#define PIEZO_PIN	B,1

/* 3.3mH coil -> tau = 79uS */
/* do not succeed 150uS (2tau) charge time */

/* charge time in uS */
#define PIEZO_VOL	((F_CPU/1000000UL) * 150)

void p_init(void);

/* control piezo, freq in Hz, approx. [100,3000], avoid 0 !! */
void p_set(uint16_t freq);
void p_off(void);

#endif /* PIEZO_H_ */
