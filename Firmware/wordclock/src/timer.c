/*
 * timer.c
 *
 *  Created on: 01.06.2016
 *      Author: Carsten
 */

#include "timer.h"
#include "led.h"
#include "stm32f0xx.h"
#include <string.h>
#include <stdlib.h>


volatile uint32_t tetrisDelay = 0;
volatile uint32_t esp8266_Timeout = 0;
struct time ctime;


void sysTick_init(void){
	// Init SysTick Timer
	SysTick_Config(SystemCoreClock / 1000); // = 1ms
}


void SysTick_Handler (void) {

	clock_Counter();
	led_Handler();
	if (tetrisDelay) tetrisDelay--;
	if (esp8266_Timeout) esp8266_Timeout--;

}

void clock_Counter(void){

	static uint16_t millisecondCounter = 0;

	if(millisecondCounter == 500){
		// generate blink pulse
		ctime.blink_2Hz = !ctime.blink_2Hz;
	}
	if ( millisecondCounter >= 1000) {
		// generate blink pulse
		ctime.blink_1Hz = !ctime.blink_1Hz;
		ctime.blink_2Hz = !ctime.blink_2Hz;
		if(ctime.seconds % 2 == 0){
			ctime.blink_0_5Hz = !ctime.blink_0_5Hz;
		}
		if (ctime.seconds >= 60){
			if(ctime.minutes >= 60){
				if(ctime.hours >= 24){
					// new day
					ctime.hours = 0;
				}
				ctime.hours++;
				ctime.minutes = 0;
			}
			ctime.minutes++;
			ctime.seconds = 0;
		}
		ctime.seconds++;
		millisecondCounter = 0;
	}
	millisecondCounter++;


}



/*
 	static uint8_t ledStatus = 0;

 // toggle LED for testing
		if (ledStatus == 1) {
			led_greenOFF();
			ledStatus = 0;
		}
		else{
			led_greenON();
			ledStatus = 1;
		}

 */
