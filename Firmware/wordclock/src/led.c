/*
 * led.c
 *
 *  Created on: 31.05.2016
 *      Author: Carsten
 */

#include "led.h"
#include "stm32f0xx.h"




void led_init(void){
	// LED 1 PA4 green?
	// LED 2 PA5 red?

	// GPIOA Enable Clock
	RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;

	// GPIOA Pin 4 and 5 in general purpose output mode (bit value: 01)
	GPIOA->MODER &= ~(GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1);
	GPIOA->MODER |=   GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0;

	// GPIOA Pin 4 and 5 Speed 25Mhz (bit value: 01)
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR4_1 | GPIO_OSPEEDR_OSPEEDR5_1);
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR4_0 | GPIO_OSPEEDR_OSPEEDR5_0;

	// GPIOA Pin 4 and 5 push pull mode (bit value: 0)
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_4);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5);

	// GPIOA Pin 4 and 5  pull up resistor (bit value: 01)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR5_1);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR5_0;

	led_greenOFF();
	led_redOFF();
}
void led_Handler(void){
	static uint16_t tick = 0;

	if (tick++ > 1000) tick = 0;
	if (tick <= 500) led_greenON();
	else led_greenOFF();
}
void led_greenON(void){
	// GPIO PA4 low
	GPIOA->ODR &= ~GPIO_BSRR_BS_4;
}
void led_greenOFF(void){
	// GPIO PA4 high
	GPIOA->ODR |= GPIO_ODR_4;
}
void led_redON(void){
	// GPIO PA5 low
	GPIOA->ODR &= ~GPIO_BSRR_BS_5;
}
void led_redOFF(void){
	// GPIO PA5 high
	GPIOA->ODR |= GPIO_ODR_5;
}

