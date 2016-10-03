/*
 * led_matrix.h
 *
 *  Created on: 07.06.2016
 *      Author: ElektronB
 */

#ifndef LED_MATRIX_H_
#define LED_MATRIX_H_


#include "ws2812.h"
#include "stm32f0xx.h"

typedef struct{

	uint8_t r;
	uint8_t g;
	uint8_t b;
}rgb_t;

void LED_Matrix_GFX_Init( void (*func_ptr_w)(uint16_t , uint8_t, uint8_t, uint8_t),  void (*func_ptr_r)(uint16_t , uint8_t*, uint8_t*, uint8_t*), uint8_t h, uint8_t w);

void LED_Matrix_Draw_Pix(uint8_t x_pos, uint8_t y_pos, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void LED_Matrix_Clear(uint8_t r, uint8_t g, uint8_t b);

void LED_Matrix_Draw_H_line(uint8_t x_pos, uint8_t y_pos, uint8_t length, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void LED_Matrix_Draw_V_line(uint8_t x_pos, uint8_t y_pos, uint8_t length, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void LED_Matrix_Draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);

void LED_Matrix_Draw_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void LED_Matrix_Draw_Fill_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);

void LED_Matrix_Draw_Circle(int16_t x0, int16_t y0, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void LED_Matrix_Draw_Fill_Circle(int16_t x0, int16_t y0, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);

void LED_Matrix_Draw_Round_Rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) ;
void LED_Matrix_Draw_Fill_Round_Rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);

void LED_Matrix_Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void LED_Matrix_Draw_Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);

void LED_Matrix_Demo_1(void);
#endif /* LED_MATRIX_H_ */
