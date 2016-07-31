/*
 * button.h
 *
 *  Created on: Jul 31, 2016
 *      Author: sid
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#define BUTTON_PIN		D,2

inline void btn_init(void)
{
	INPUT(BUTTON_PIN);

	/* enable pull up */
	SET(BUTTON_PIN);
}

#define btn_pressed() 		(!GET(BUTTON_PIN))

#endif /* BUTTON_H_ */
