/*
 * uart1.c
 *
 *  Created on: 21.05.2016
 *      Author: Carsten
 */

#include "uart1.h"
#include "stm32f0xx.h"

char uart1RecBuf[UART1_REC_BUF_MAX];
uint8_t uart1RecFlag = 0;

void uart1_init(void){

	// Enable clock for GPIOA
	RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
	// Enabele clock for USART1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// GPIOA Pin 9 and 10 in alternate function mode
	GPIOA->MODER &= ~(GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0);
	GPIOA->MODER |=   GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
	// GPIOA Pin 9 and 10 in alternate function mode 1
	GPIOA->AFR[1] |= (1 << 4) | (1 << 8);
	// GPIOA Pin 9 push pull
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_9);
	// GPIOA Pin 10 open drain
	GPIOA->OTYPER |=   GPIO_OTYPER_OT_10;
	// GPIOA Pin 9 and 10 pull-up
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR9_1 | GPIO_PUPDR_PUPDR10_1);
	GPIOA->PUPDR |=   GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0;

	// Set baudrate to 115200 bit/s
	USART1->BRR = 480000 / 1152;
	USART1->CR3 |= USART_CR3_OVRDIS;
	// Receive Transceive UART enable

	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;


	NVIC_EnableIRQ(USART1_IRQn);

}

void uart1_sendChar (char character) {

	// Send character
	USART1->TDR = character;
	// Wait till transmission complete
	while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);
}

void uart1_sendString(char* string) {

	while(*string){
		uart1_sendChar(*string++);
	}
}

void USART1_IRQHandler (void) {

	static uint8_t uart1RecBufIndex = 0;
	char charTmp = 0;

	if (USART1->ISR & USART_ISR_RXNE){
		charTmp = USART1->RDR;
		if (!uart1RecFlag){
			if (uart1RecBufIndex < UART1_REC_BUF_MAX){
				if (charTmp != '\r'){
					uart1RecBuf[uart1RecBufIndex++] = charTmp;
				}
				else {
					uart1RecBuf[uart1RecBufIndex++] = '\0';
					uart1RecBufIndex = 0;
					uart1RecFlag = 1;
				}
			}
			else{
				if (charTmp != '\r'){
					uart1RecBuf[UART1_REC_BUF_MAX-1] = '\0';
					uart1RecBufIndex = 0;
					uart1RecFlag = 1;
				}
			}
		}
	}
}
