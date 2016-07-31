/*
 * climb.h
 *
 *  Created on: Jul 31, 2016
 *      Author: sid
 */

#ifndef CLIMB_H_
#define CLIMB_H_


#define CLIMB_SAMPLES_PER_SEC		28	//todo
#define CLIMB_AVG_SEC			3
#define CLIMB_SAMPLES			(CLIMB_SAMPLES_PER_SEC * CLIMB_AVG_SEC)

#define LR_x_cross 			(CLIMB_SAMPLES / 2)
#define LR_den				4940600.0f		//todo
					//sum((i-LR_x_cross)^2)*100 from 0 to CLIMB_SAMPLES-1, todo: start from 1 ??

float climb_update(int32_t alt_cm);

#endif /* CLIMB_H_ */
