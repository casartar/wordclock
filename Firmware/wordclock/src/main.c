/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include "ws2812.h"
#include "led_matrix.h"
#include "led.h"
#include "timer.h"
#include "uart1.h"
#include "uart2.h"
#include "esp8266.h"
#include "config.h"
#include "eeprom.h"
#include "queue.h"
#include "word_clock_draw.h"
#include "cmdHandler.h"

int main(void)
{

	WS2812_Init();
	LED_Matrix_GFX_Init(WS2812_WirteBuffer, WS2812_ReadBuffer, 10, 11);
	led_init();
	sysTick_init();
	uart2_init();
	uart1_init();	// Debug Interface and command Handler interface
	uart1_sendString("Wordclock template \r\n");

	// display red while initializing
	LED_Matrix_Clear(255,0,0);
	WS2812_Update();

	config_init();
	if (config_read()){
		config_setDefault();
		config_write();
	}


	// Wait for esp8266 bootup
	while(ctime.seconds < 2);
	// Delete garbage from WLAN module
	queue_init(&uart2RecQueue);

	Word_Clock_Init(config.colorRed, config.colorGreen, config.colorBlue, 255);


	while(1){

		cmdHandler();
		esp8266_requestTime();

		if (updateInterval == 0){
			updateInterval = 1000;
			LED_Matrix_Clear(0,0,0);
			// if day == 0 , there is no valid timestamp from ntp
			if(ctime.day != 0){
				Word_Clock_Draw(ctime.hours, ctime.minutes, config.brightness);
				WS2812_Update();
			}


		}

	}
}

void DMA1_Channel4_5_6_7_IRQHandler(void)
{
	WS2812_DMA_IRQHandler();
}

void DMA1_Channel2_3_IRQHandler(void){
	WS2812_DMA_IRQHandler();
}


