/*
 * led.h
 *
 *  Created on: 31.05.2016
 *      Author: Carsten
 */

#ifndef LED_H_
#define LED_H_

#include <stdint.h>


void led_init(void);
void led_Handler(void);
void led_greenON(void);
void led_greenOFF(void);
void led_redON(void);
void led_redOFF(void);


#endif /* LED_H_ */
