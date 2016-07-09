/*
 * uart1.h
 *
 *  Created on: 21.05.2016
 *      Author: Carsten
 */

#ifndef UART1_H_
#define UART1_H_

#include <stdint.h>

#define UART1_REC_BUF_MAX 100

extern char uart1RecBuf[UART1_REC_BUF_MAX];
extern uint8_t uart1RecFlag;

void uart1_init(void);
void uart1_sendChar (char character);
void uart1_sendString(char* string);

#endif /* UART1_H_ */
