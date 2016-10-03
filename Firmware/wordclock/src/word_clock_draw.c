/*
 * word_clock_draw.c
 *
 *  Created on: 02.10.2016
 *      Author: ElektronB
 */
#include "word_clock_draw.h"
#include "word_clock_mask.h"
#include "led_matrix.h"
#include "timer.h"
#include "config.h"

clock_word_dict_t clock_word_dict;


static void Word_Clock_Init_Helper(clock_word_t *clock_word, uint8_t  x, uint8_t  y, uint8_t  len, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	clock_word->x		= x;
	clock_word->y		= y;
	clock_word->len 	= len;
	clock_word->r		= r;
	clock_word->g		= g;
	clock_word->b		= b;
	clock_word->alpha	= alpha;

}

void Word_Clock_Init(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	Word_Clock_Init_Helper(&clock_word_dict.it, 	WORD_CLOCK_MASK_IT		, config.wordColor[0].r, config.wordColor[0].g, config.wordColor[0].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.is, 	WORD_CLOCK_MASK_IS		, config.wordColor[1].r, config.wordColor[1].g, config.wordColor[1].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.before, WORD_CLOCK_MASK_BEFORE	, config.wordColor[2].r, config.wordColor[2].g, config.wordColor[2].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.after, 	WORD_CLOCK_MASK_AFTER	, config.wordColor[3].r, config.wordColor[3].g, config.wordColor[3].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.min_0, 	WORD_CLOCK_MASK_MIN_0	, config.wordColor[4].r, config.wordColor[4].g, config.wordColor[4].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.min_5, 	WORD_CLOCK_MASK_MIN_5	, config.wordColor[5].r, config.wordColor[5].g, config.wordColor[5].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.min_10, WORD_CLOCK_MASK_MIN_10	, config.wordColor[6].r, config.wordColor[6].g, config.wordColor[6].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.min_15, WORD_CLOCK_MASK_MIN_15	, config.wordColor[7].r, config.wordColor[7].g, config.wordColor[7].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.min_20, WORD_CLOCK_MASK_MIN_20	, config.wordColor[8].r, config.wordColor[8].g, config.wordColor[8].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.min_30, WORD_CLOCK_MASK_MIN_30	, config.wordColor[9].r, config.wordColor[9].g, config.wordColor[9].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.min_45, WORD_CLOCK_MASK_MIN_45	, config.wordColor[10].r, config.wordColor[10].g, config.wordColor[10].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_1, 	WORD_CLOCK_MASK_H_1		, config.wordColor[11].r, config.wordColor[11].g, config.wordColor[11].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_1s, 	WORD_CLOCK_MASK_H_1s	, config.wordColor[12].r, config.wordColor[12].g, config.wordColor[12].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_2, 	WORD_CLOCK_MASK_H_2		, config.wordColor[13].r, config.wordColor[13].g, config.wordColor[13].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_3, 	WORD_CLOCK_MASK_H_3		, config.wordColor[14].r, config.wordColor[14].g, config.wordColor[14].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_4, 	WORD_CLOCK_MASK_H_4		, config.wordColor[15].r, config.wordColor[15].g, config.wordColor[15].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_5, 	WORD_CLOCK_MASK_H_5		, config.wordColor[16].r, config.wordColor[16].g, config.wordColor[16].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_6, 	WORD_CLOCK_MASK_H_6		, config.wordColor[17].r, config.wordColor[17].g, config.wordColor[17].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_7, 	WORD_CLOCK_MASK_H_7		, config.wordColor[18].r, config.wordColor[18].g, config.wordColor[18].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_8, 	WORD_CLOCK_MASK_H_8		, config.wordColor[19].r, config.wordColor[19].g, config.wordColor[19].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_9, 	WORD_CLOCK_MASK_H_9		, config.wordColor[20].r, config.wordColor[20].g, config.wordColor[20].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_10, 	WORD_CLOCK_MASK_H_10	, config.wordColor[21].r, config.wordColor[21].g, config.wordColor[21].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_11, 	WORD_CLOCK_MASK_H_11	, config.wordColor[22].r, config.wordColor[22].g, config.wordColor[22].b, alpha);
	Word_Clock_Init_Helper(&clock_word_dict.h_12, 	WORD_CLOCK_MASK_H_12	, config.wordColor[23].r, config.wordColor[23].g, config.wordColor[23].b, alpha);
}

static void Word_Clock_Draw_Helper(clock_word_t *clock_word, uint8_t brightness){
	uint8_t r,g,b;
	r = (uint8_t)(((uint16_t)brightness * (uint16_t)clock_word->r) >> 8);
	g = (uint8_t)(((uint16_t)brightness * (uint16_t)clock_word->g) >> 8);
	b = (uint8_t)(((uint16_t)brightness * (uint16_t)clock_word->b) >> 8);

	LED_Matrix_Draw_H_line(clock_word->x, clock_word->y, clock_word->len, r, g, b, clock_word->alpha);
}

void Word_Clock_Draw(uint8_t h, uint8_t m, uint8_t brightness){
	uint8_t offset_h = 0;

	if(h > 12){
		h -=12;
	}

	Word_Clock_Draw_Helper(&clock_word_dict.it,  brightness);
	Word_Clock_Draw_Helper(&clock_word_dict.is,  brightness);

	switch(m / 5){
		case 0:	//genau N Uhr
			Word_Clock_Draw_Helper(&clock_word_dict.min_0,  brightness);
			break;
		case 1:	//5 nach N
			Word_Clock_Draw_Helper(&clock_word_dict.min_5,  brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.after,  brightness);
			break;
		case 2:	//10 nach N
			Word_Clock_Draw_Helper(&clock_word_dict.min_10, brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.after,  brightness);
			break;
		case 3:	//15 nach N
			Word_Clock_Draw_Helper(&clock_word_dict.min_15, brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.after,  brightness);
			break;
		case 4:	//20 nach N
			Word_Clock_Draw_Helper(&clock_word_dict.min_20, brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.after,  brightness);
			break;
		case 5:	//5 vor Halb N+1
			Word_Clock_Draw_Helper(&clock_word_dict.min_5,  brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.before, brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.min_30, brightness);
			offset_h = 1;
			break;
		case 6:	//halb N+1
			Word_Clock_Draw_Helper(&clock_word_dict.min_30, brightness);
			offset_h = 1;
			break;
		case 7:	//5 nach halb N+1
			Word_Clock_Draw_Helper(&clock_word_dict.min_5,  brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.after,  brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.min_30, brightness);
			offset_h = 1;
			break;
		case 8:	//20 vor N+1
			Word_Clock_Draw_Helper(&clock_word_dict.min_20, brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.before, brightness);
			offset_h = 1;
			break;
		case 9:	//3/4 N+1
			//Word_Clock_Draw_Helper(&clock_word_dict.min_45, brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.min_15, brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.before, brightness);
			offset_h = 1;
			break;
		case 10://10 vor N+1
			Word_Clock_Draw_Helper(&clock_word_dict.min_10, brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.before, brightness);
			offset_h = 1;
			break;
		case 11://5 vor N+1
			Word_Clock_Draw_Helper(&clock_word_dict.min_5,  brightness);
			Word_Clock_Draw_Helper(&clock_word_dict.before, brightness);
			offset_h = 1;
			break;
	}

	switch(h+offset_h){
		case 1:
			if(m < 5){
				Word_Clock_Draw_Helper(&clock_word_dict.h_1,  brightness);
			}else{
				Word_Clock_Draw_Helper(&clock_word_dict.h_1s,  brightness);
			}
			break;
		case 2:
			Word_Clock_Draw_Helper(&clock_word_dict.h_2,  brightness);
			break;
		case 3:
			Word_Clock_Draw_Helper(&clock_word_dict.h_3,  brightness);
			break;
		case 4:
			Word_Clock_Draw_Helper(&clock_word_dict.h_4,  brightness);
			break;
		case 5:
			Word_Clock_Draw_Helper(&clock_word_dict.h_5,  brightness);
			break;
		case 6:
			Word_Clock_Draw_Helper(&clock_word_dict.h_6,  brightness);
			break;
		case 7:
			Word_Clock_Draw_Helper(&clock_word_dict.h_7,  brightness);
			break;
		case 8:
			Word_Clock_Draw_Helper(&clock_word_dict.h_8,  brightness);
			break;
		case 9:
			Word_Clock_Draw_Helper(&clock_word_dict.h_9,  brightness);
			break;
		case 10:
			Word_Clock_Draw_Helper(&clock_word_dict.h_10, brightness);
			break;
		case 11:
			Word_Clock_Draw_Helper(&clock_word_dict.h_11, brightness);
			break;
		case 12:
			Word_Clock_Draw_Helper(&clock_word_dict.h_12, brightness);
			break;
		case 13:
			if(m < 5){
				Word_Clock_Draw_Helper(&clock_word_dict.h_1,  brightness);
			}else{
				Word_Clock_Draw_Helper(&clock_word_dict.h_1s,  brightness);
			}

			break;
	}
}

 void Word_Clock_Set_Color(clock_word_t *clock_word, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	 clock_word->r = r;
	 clock_word->g = g;
	 clock_word->b = b;
	 clock_word->alpha = alpha;
 }
