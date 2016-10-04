/*
 * main.h
 *
 *  Created on: Aug 8, 2016
 *      Author: sid
 */

#ifndef MAIN_H_
#define MAIN_H_

#if F_CPU == 1000000UL
	#define MAINLOOP_TICKSPERMINUTE		356U
#elif F_CPU == 2000000UL
	#define MAINLOOP_TICKSPERMINUTE		1150U
#else
	#warning overflow may happen!
	#define MAINLOOP_TICKSPERMINUTE		((F_CPU/1000000UL) * 356)
#endif
#define MAINLOOP_HALFHOUR		(MAINLOOP_TICKSPERMINUTE * 30)

#endif /* MAIN_H_ */
