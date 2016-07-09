/*
 * ws2812.h
 *
 *  Created on: 28.05.2016
 *      Author: sascha
 */

#ifndef WS2812_H_
#define WS2812_H_

#define WordClock_Board
//#define WordClock_Board_RX_PIN
//#define WS2812_STM32F1

/* Includes ------------------------------------------------------------------*/
#if (defined(WordClock_Board) || defined(WordClock_Board_RX_PIN))
	#include "stm32f0xx.h"
	#define WS2812_STM32F0
#endif

/* Public typedef -----------------------------------------------------------*/
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
}RgbLed_t;


/* Public define ------------------------------------------------------------*/
#define WS2812_BAUDRATE				800000
#define WS2812_DUTYCYCLE			32
#define WS2812_LED_COUNT			110
#define WS2812_TX_BUFFER_COUNT		8

#ifdef WordClock_Board
	#define WS2812_TIMER				TIM3
	#define WS2812_TIMER_CLK			RCC_APB1Periph_TIM3
	#define WS2812_TIMER_CLK_CMD(p,s)	RCC_APB1PeriphClockCmd(p,s)
	#define WS2812_TIMER_OUT_CH			CCR1
	#define WS2812_TIMER_OC_CMD(p,s)	TIM_OC1Init(p,s);

	#define WS2812_DMA_CLK				RCC_AHBPeriph_DMA1
	#define WS2812_DMA_CLK_CMD(p,s)		RCC_AHBPeriphClockCmd(p,s)
	#define WS2812_DMA_CH				DMA1_Channel3
	#define WS2812_DMA_IRQ_HT			DMA1_IT_HT3
	#define WS2812_DMA_IRQ_TC			DMA1_IT_TC3
	#define WS2812_DMA_IRQ				DMA1_Channel2_3_IRQn

	#define WS2812_GPIO_PORT            GPIOA
	#define WS2812_GPIO_PIN             GPIO_Pin_6
	#define WS2812_GPIO_MODE			GPIO_Mode_AF
	#define WS2812_GPIO_SPEED			GPIO_Speed_50MHz
	#define WS2812_GPIO_SOURCE          GPIO_PinSource6
	#define WS2812_GPIO_AF              GPIO_AF_1
	#define WS2812_GPIO_CLK             RCC_AHBPeriph_GPIOA
	#define WS2812_GPIO_CLK_CMD(p,s)    RCC_AHBPeriphClockCmd(p,s)
#endif

#ifdef WordClock_Board_RX_PIN
	#define WS2812_TIMER				TIM1
	#define WS2812_TIMER_CLK			RCC_APB2Periph_TIM1
	#define WS2812_TIMER_CLK_CMD(p,s)	RCC_APB2PeriphClockCmd(p,s)
	#define WS2812_TIMER_OUT_CH			CCR3
	#define WS2812_TIMER_OC_CMD(p,s)	TIM_OC3Init(p,s);

	#define WS2812_DMA_CLK				RCC_AHBPeriph_DMA1
	#define WS2812_DMA_CLK_CMD(p,s)		RCC_AHBPeriphClockCmd(p,s)
	#define WS2812_DMA_CH				DMA1_Channel4
	#define WS2812_DMA_IRQ_HT			DMA1_IT_HT4
	#define WS2812_DMA_IRQ_TC			DMA1_IT_TC4
	#define WS2812_DMA_IRQ				DMA1_Channel4_5_6_7_IRQn

	#define WS2812_GPIO_PORT            GPIOA
	#define WS2812_GPIO_PIN             GPIO_Pin_10
	#define WS2812_GPIO_MODE			GPIO_Mode_AF
	#define WS2812_GPIO_SPEED			GPIO_Speed_50MHz
	#define WS2812_GPIO_SOURCE          GPIO_PinSource10
	#define WS2812_GPIO_AF              GPIO_AF_2
	#define WS2812_GPIO_CLK             RCC_AHBPeriph_GPIOA
	#define WS2812_GPIO_CLK_CMD(p,s)    RCC_AHBPeriphClockCmd(p,s)
#endif

#ifdef WS2812_STM32F1
	#define WS2812_TIMER				TIM1
	#define WS2812_TIMER_CLK			RCC_APB2Periph_TIM1
	#define WS2812_TIMER_CLK_CMD(p,s)	RCC_APB2PeriphClockCmd(p,s)

	#define WS2812_DMA_CLK				RCC_AHBPeriph_DMA1
	#define WS2812_DMA_CLK_CMD(p,s)		RCC_AHBPeriphClockCmd(p,s)
	#define WS2812_DMA_IRQ				DMA1_Channel4_IRQn

	#define WS2812_GPIO_PIN             GPIO_Pin_8
	#define WS2812_GPIO_PORT            GPIOA
	#define WS2812_GPIO_MODE			GPIO_Mode_AF_PP
	#define WS2812_GPIO_SPEED			GPIO_Speed_50MHz
	#define WS2812_GPIO_CLK             RCC_APB2Periph_GPIOA
	#define WS2812_GPIO_CLK_CMD(p,s)    RCC_APB2PeriphClockCmd(p,s)
#endif

/* Public function prototypes -----------------------------------------------*/
void WS2812_Init(void);
void WS2812_WirteBuffer(uint16_t buffer_pos, uint8_t r, uint8_t g, uint8_t b);
void WS2812_ReadBuffer(uint16_t buffer_pos, uint8_t *r, uint8_t *g, uint8_t *b);
void WS2812_Update(void);
uint8_t WS2812_Ready(void);
void WS2812_DMA_IRQHandler(void);

#endif /* WS2812_H_ */
