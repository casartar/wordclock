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

int main(void)
{

	WS2812_Init();
	LED_Matrix_GFX_Init(WS2812_WirteBuffer, WS2812_ReadBuffer, 10, 11);
	led_init();
	sysTick_init();
	uart2_init();
	uart1_init();	// Debug Interface
	uart1_sendString("Wordclock template \r\n");


	config_init();

	if (config_read()){
		config_setDefault();
		config_write();
	}

	char Buffer[100];


	while(ctime.seconds < 2);
	queue_init(&uart2RecQueue);
	// Delete garbage from WLAN module

	//while(esp8266_accessPoint());
	LED_Matrix_Draw_Fill_Rect(0,0,11,10,0,0,0,255);
	WS2812_Update();
	while(1){
		cmdHandler();
	}
/*
	while(1){
		esp8266_requestTime();

		if(tetrisDelay == 0){


			uint8_t h_ = ctime.hours / 10;
			uint8_t _h = ctime.hours - 10 * h_;
			uint8_t m_ = ctime.minutes / 10;
			uint8_t _m = ctime.minutes - 10 * m_;
			uint8_t s_ = ctime.seconds /10;
			uint8_t _s = ctime.seconds - 10 * s_;
			LED_Matrix_Clear(0,0,0);

			LED_Matrix_Draw_V_line(0,0,h_,25,0,0,128);
			LED_Matrix_Draw_V_line(1,0,_h,25,0,0,128);
			LED_Matrix_Draw_V_line(3,0,m_,25,0,0,128);
			LED_Matrix_Draw_V_line(4,0,_m,25,0,0,128);
			LED_Matrix_Draw_V_line(6,0,s_,25,0,0,128);
			LED_Matrix_Draw_V_line(7,0,_s,25,0,0,128);
			led_redON();
			sprintf(Buffer,"%02u:%02u:%02u\n",ctime.hours,ctime.minutes,ctime.seconds);
			uart1_sendString(Buffer);

			WS2812_Update();
			tetrisDelay = 500;
		}


	}
*/

}

void DMA1_Channel4_5_6_7_IRQHandler(void)
{
	WS2812_DMA_IRQHandler();
}

void DMA1_Channel2_3_IRQHandler(void){
	WS2812_DMA_IRQHandler();
}


