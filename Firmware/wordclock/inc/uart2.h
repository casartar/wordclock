/*
 * uart2.h
 *
 *  Created on: 21.05.2016
 *      Author: Carsten
 */

#ifndef UART2_H_
#define UART2_H_

#include <stdint.h>

void uart2_init(void);
void uart2_sendChar (char character);
void uart2_sendString(char* string);

#endif /* UART2_H_ */
