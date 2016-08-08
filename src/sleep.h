/*
 * sleep.h
 *
 *  Created on: Aug 2, 2016
 *      Author: sid
 */

#ifndef SLEEP_H_
#define SLEEP_H_

#define TICKSPERMINUTE	356
#define HALFHOUR			TICKSPERMINUTE * 30

uint16_t rest_time;

void sleep(void);

#endif /* SLEEP_H_ */
