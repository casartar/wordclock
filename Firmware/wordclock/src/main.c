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

uint8_t tetris_figures[7]={
	//bit: 7 6 5 4
	//     3 2 1 0
	//     # # # #
	//     - - - -
	0b11110000,

	//     - # - -
	//     # # # -
	0b01001110,

	//     - # # -
	//     - # # -
	0b01100110,

	//     # - - -
	//     # # # -
	0b10001110,

	//     - - - #
	//     - # # #
	0b00010111,

	//     # # - -
	//     - # # -
	0b11000110,

	//     - - # #
	//     - # # -
	0b00110110
};

void Tetris_Draw_Figure(uint8_t x0, uint8_t y0, uint8_t figure, uint8_t rotation){
	uint8_t fig_tmp, x, y,i;

	fig_tmp = tetris_figures[figure];

	x = x0+3;
	y = y0;
	i = 0;
	while( i++ < 8){
		if(fig_tmp & 1){
			LED_Matrix_Draw_Pix(x, y, 0, 0, 30, 255);
		}else{
			LED_Matrix_Draw_Pix(x, y, 0, 0,  0, 255);
		}
		fig_tmp>>=1;
		if(x > x0){
			x--;
		}else{
			x=x0+3;
			y++;
		}
	}
}

int main(void)
{

	WS2812_Init();
	led_init();
	sysTick_init();
	uart2_init();
	uart1_init();	// Debug Interface
	uart1_sendString("Wordclock template \r\n");
	config.summertimeFlag = 0;
	config.gmtOffsetSummertime = 2;
	config.gmtOffsetWintertime = 1;


	LED_Matrix_GFX_Init(WS2812_WirteBuffer, WS2812_ReadBuffer, 10, 11);

	while(esp8266_accessPoint());

	while(1){
		esp8266_requestTime();
	}
	//esp8266_accessPoint();
	//LED_Matrix_Demo_1();

	uint8_t fig= 0;
	uint8_t x_set=0, y_set=0;
	while(1){

		if(tetrisDelay == 0){
			LED_Matrix_Clear(0,0,0);

			if(y_set>0){
				y_set--;
			}else{
				y_set = 7;
				x_set++;
				if(x_set > 7){
					x_set = 0;
				}

				fig++;
				if(fig>7){
					fig = 0;
				}
			}

			Tetris_Draw_Figure(x_set,y_set,fig,0);

			WS2812_Update();
			tetrisDelay = 500;
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


