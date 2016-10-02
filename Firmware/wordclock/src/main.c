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

	config_init();

	if (config_read()){
		config_setDefault();
		config_write();
	}

	// Wait for esp8266 bootup
	while(ctime.seconds < 2);
	// Delete garbage from WLAN module
	queue_init(&uart2RecQueue);

	Word_Clock_Init(0,255,0,255);
	Word_Clock_Set_Color(&clock_word_dict.min_0,  255,  0,  0,255);
	Word_Clock_Set_Color(&clock_word_dict.min_5,  127,127,127,255);
	Word_Clock_Set_Color(&clock_word_dict.min_10,   0,  0,255,255);
	Word_Clock_Set_Color(&clock_word_dict.min_15, 127,127,  0,255);
	Word_Clock_Set_Color(&clock_word_dict.min_20,   0,127,127,255);
	Word_Clock_Set_Color(&clock_word_dict.min_30, 127,  0,127,255);
	Word_Clock_Set_Color(&clock_word_dict.min_45,   0,200,200,255);

	uint8_t dim = 50;

	while(1){

		cmdHandler();
		esp8266_requestTime();
		if (updateInterval == 0){
			updateInterval = 1000;
			LED_Matrix_Clear(0,0,0);
			Word_Clock_Draw(ctime.hours, ctime.minutes, 255);
			WS2812_Update();
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


