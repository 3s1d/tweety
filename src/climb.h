/*
 * climb.h
 *
 *  Created on: Jul 31, 2016
 *      Author: sid
 */

#ifndef CLIMB_H_
#define CLIMB_H_


#define CLIMB_SAMPLES_PER_SEC		25						//approx value must be measured upon F_CPU changes
#define CLIMB_AVG_SEC			1
#define CLIMB_SAMPLES			38	//(CLIMB_SAMPLES_PER_SEC * CLIMB_AVG_SEC)
					//1.5sec

#define CLIMBTRESHOLD			30
#define SINKTHRESHOLD			-250

#define LR_x_cross 			(CLIMB_SAMPLES>>1)
#define LR_n				((int32_t)CLIMB_SAMPLES)
#define LR_x				((int32_t)LR_x_cross)
#define LR_den				((LR_n*(LR_n+1)*(2*LR_n+1))/6 - LR_n*LR_n - LR_n*LR_n*LR_x + LR_n*LR_x + LR_n*LR_x*LR_x)
					//sum((i-LR_x_cross)^2) from 0 to CLIMB_SAMPLES-1


//#define CLIMB_TEST

#ifdef CLIMB_TEST

/* adjust test values here */
#define C_START_ALT_CM			200000
#define	C_CLIMB_CMS			300		//resolution is CLIMB_SAMPLES_PER_SEC
/******/

#define C_ADD				C_CLIMB_CMS/CLIMB_SAMPLES_PER_SEC

#endif

extern int16_t climb_cms;

void climb_init(void);
void climb_deinit(void);

void climb_update(void);

#endif /* CLIMB_H_ */
