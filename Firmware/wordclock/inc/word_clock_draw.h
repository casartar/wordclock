/*
 * word_clock_draw.h
 *
 *  Created on: 02.20.2016
 *      Author: ElektronB
 */

#ifndef WORD_CLOCK_DRAW_H
#define WORD_CLOCK_DRAW_H

#include "stm32f0xx.h"

typedef struct{
	uint8_t  x;
	uint8_t  y;
	uint8_t  len;
	uint8_t  r; 
	uint8_t  g; 
	uint8_t  b; 
	uint8_t  alpha;
}clock_word_t;

typedef struct{
	clock_word_t it;
	clock_word_t is;
	clock_word_t before;
	clock_word_t after;
	clock_word_t min_0;
	clock_word_t min_5;
	clock_word_t min_10;
	clock_word_t min_15;
	clock_word_t min_20;
	clock_word_t min_30;
	clock_word_t min_45;
	clock_word_t h_1;
	clock_word_t h_2;
	clock_word_t h_3;
	clock_word_t h_4;
	clock_word_t h_5;
	clock_word_t h_6;
	clock_word_t h_7;
	clock_word_t h_8;
	clock_word_t h_9;
	clock_word_t h_10;
	clock_word_t h_11;
	clock_word_t h_12;
}clock_word_dict_t;

extern clock_word_dict_t clock_word_dict;

void Word_Clock_Init(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void Word_Clock_Draw(uint8_t h, uint8_t m, uint8_t brightness);
void Word_Clock_Set_Color(clock_word_t *clock_word, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);

#endif /* WORD_CLOCK_DRAW_H */
