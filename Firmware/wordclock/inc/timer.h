/*
 * timer.h
 *
 *  Created on: 01.06.2016
 *      Author: Carsten
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

extern volatile uint32_t updateInterval;
extern volatile uint32_t esp8266_Timeout;

void sysTick_init(void);
void clock_Counter(void);


struct time{
 uint8_t  seconds;
 uint8_t  minutes;
 uint8_t  hours;
 uint8_t  day;
 uint8_t  month;
 uint16_t  year;
 uint8_t  blink_1Hz;
 uint8_t  blink_2Hz;
 uint8_t  blink_0_5Hz;
}ctime;

extern struct time ctime;

#endif /* TIMER_H_ */
