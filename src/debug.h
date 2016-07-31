/*
 * debug.h
 *
 *  Created on: Jul 31, 2016
 *      Author: sid
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG

#ifdef DEBUG

#define DEBUG_SCL_PIN		C,2
#define DEBUG_SDA_PIN		D,3

void debug_put(uint8_t *buf, uint8_t length);

#endif

#endif /* DEBUG_H_ */
