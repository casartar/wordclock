/*
 * uart2.c
 *
 *  Created on: 21.05.2016
 *      Author: Carsten
 */

#include "uart2.h"
#include "stm32f0xx.h"
#include "queue.h"


void uart2_init(void){

	// Enable clock for GPIOA
	RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
	// Enabele clock for USART2
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	// GPIOA Pin 2 and 3 in alternate function mode
	GPIOA->MODER &= ~(GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0);
	GPIOA->MODER |=   GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1;
	// GPIOA Pin 2 and 3 in alternate function mode 1
	GPIOA->AFR[0] |= (1 << 8) | (1 << 12);
	// GPIOA Pin 2 push pull
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_2);
	// GPIOA Pin 3 open drain
	GPIOA->OTYPER |=   GPIO_OTYPER_OT_3;
	// GPIOA Pin 2 and 3 pull-up
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_1);
	GPIOA->PUPDR |=   GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR3_0;

	// Set baudrate to 115200 bit/s
	USART2->BRR = 480000 / 1152;
	USART2->CR3 |= USART_CR3_OVRDIS;
	// Receive Transceive UART enable

	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;


	NVIC_EnableIRQ(USART2_IRQn);

}

void uart2_sendChar (char character) {

	// Send character
	USART2->TDR = character;
	// Wait till transmission complete
	while((USART2->ISR & USART_ISR_TC) != USART_ISR_TC);
}

void uart2_sendString(char* string) {

	while(*string){
		uart2_sendChar(*string++);
	}
}

void USART2_IRQHandler (void) {

	if (USART2->ISR & USART_ISR_RXNE){
		if( queue_write(&uart2RecQueue, USART2->RDR) ){
			// Buffer full
		}
	}
}
