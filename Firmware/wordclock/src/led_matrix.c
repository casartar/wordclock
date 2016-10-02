/*
 * led_matrix.c
 *
 *  Created on: 07.06.2016
 *      Author: ElektronB
 */
#include "led_matrix.h"
#include <stdlib.h>

#ifndef min
	#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
	#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

static void LED_Matrix_Draw_Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t rad,  uint8_t cornername, int16_t delta, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
static void LED_Matrix_Draw_Circle_Helper( int16_t x0, int16_t y0, int16_t rad,  uint8_t cornername, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) ;

static void (*buffer_w)(uint16_t , uint8_t, uint8_t, uint8_t);
static void (*buffer_r)(uint16_t , uint8_t*, uint8_t*, uint8_t*);
static uint8_t matrix_height = 0;
static uint8_t matrix_width  = 0;

void LED_Matrix_GFX_Init( void (*func_ptr_w)(uint16_t , uint8_t, uint8_t, uint8_t),  void (*func_ptr_r)(uint16_t , uint8_t*, uint8_t*, uint8_t*), uint8_t h, uint8_t w){
	buffer_w		= func_ptr_w;
	buffer_r		= func_ptr_r;
	matrix_height	= h;
	matrix_width	= w;
}

void LED_Matrix_Draw_Pix(uint8_t x_pos, uint8_t y_pos, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	uint16_t cal_pos = 0;
	uint8_t tmp_r, tmp_g, tmp_b;
	uint16_t inv_alpha;

	//ungerade Reihe
	if(y_pos & 1){
		cal_pos = matrix_width * y_pos + (matrix_width - x_pos - 1);
	}
	//gerade Reihe
	else{
		cal_pos = matrix_width * y_pos + x_pos;
	}

	if(alpha < 255){
		//get actual pixel color
		buffer_r(cal_pos, &tmp_r, &tmp_g, &tmp_b);

		//blend with new color
		inv_alpha = 256 - alpha;

		r = (uint8_t)(((uint16_t) alpha * r + (uint16_t) inv_alpha * tmp_r) >> 8);
		g = (uint8_t)(((uint16_t) alpha * g + (uint16_t) inv_alpha * tmp_g) >> 8);
		b = (uint8_t)(((uint16_t) alpha * b + (uint16_t) inv_alpha * tmp_b) >> 8);
	}

	buffer_w(cal_pos, r, g, b);
}

void LED_Matrix_Clear(uint8_t r, uint8_t g, uint8_t b){
	uint16_t i = 0;
	for( i = 0; i < (matrix_width*matrix_height) ; ++i )
	{
		buffer_w(i, r, g, b);
	}
}

void LED_Matrix_Draw_H_line(uint8_t x_pos, uint8_t y_pos, uint8_t length, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	uint8_t end, tmp_pos;
	end 		= x_pos + length;
	tmp_pos 	= x_pos;

	while(tmp_pos < end){
		LED_Matrix_Draw_Pix(tmp_pos, y_pos, r, g, b, alpha);
		tmp_pos++;
	}
}

void LED_Matrix_Draw_V_line(uint8_t x_pos, uint8_t y_pos, uint8_t length, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	uint8_t end, tmp_pos;
	end 		= y_pos + length;
	tmp_pos 	= y_pos;

	while(tmp_pos < end){
		LED_Matrix_Draw_Pix(x_pos, tmp_pos, r, g, b, alpha);
		tmp_pos++;
	}
}

// Bresenham's algorithm - thx wikpedia
void LED_Matrix_Draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0<=x1; x0++) {
		if (steep) {
			LED_Matrix_Draw_Pix(y0, x0, r, g, b, alpha);
		} else {
			LED_Matrix_Draw_Pix(x0, y0, r, g, b, alpha);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void LED_Matrix_Draw_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	LED_Matrix_Draw_H_line(x    , y    , w,  r,  g,  b,  alpha);
	LED_Matrix_Draw_H_line(x    , y+h-1, w,  r,  g,  b,  alpha);
	LED_Matrix_Draw_V_line(x    , y    , h,  r,  g,  b,  alpha);
	LED_Matrix_Draw_V_line(x+w-1, y    , h,  r,  g,  b,  alpha);
}

void LED_Matrix_Draw_Fill_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha){
	for (int16_t i=x; i<x+w; i++) {
		LED_Matrix_Draw_V_line(i, y, h, r, g, b, alpha);
	}
}

void LED_Matrix_Draw_Circle(int16_t x0, int16_t y0, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
	int16_t f = 1 - rad;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * rad;
	int16_t x = 0;
	int16_t y = rad;

	LED_Matrix_Draw_Pix(x0    , y0+rad, r, g, b, alpha);
	LED_Matrix_Draw_Pix(x0    , y0-rad, r, g, b, alpha);
	LED_Matrix_Draw_Pix(x0+rad, y0    , r, g, b, alpha);
	LED_Matrix_Draw_Pix(x0-rad, y0    , r, g, b, alpha);


	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LED_Matrix_Draw_Pix(x0 + x, y0 + y, r, g, b, alpha);
		LED_Matrix_Draw_Pix(x0 - x, y0 + y, r, g, b, alpha);
		LED_Matrix_Draw_Pix(x0 + x, y0 - y, r, g, b, alpha);
		LED_Matrix_Draw_Pix(x0 - x, y0 - y, r, g, b, alpha);

		LED_Matrix_Draw_Pix(x0 + y, y0 + x, r, g, b, alpha);
		LED_Matrix_Draw_Pix(x0 - y, y0 + x, r, g, b, alpha);
		LED_Matrix_Draw_Pix(x0 + y, y0 - x, r, g, b, alpha);
		LED_Matrix_Draw_Pix(x0 - y, y0 - x, r, g, b, alpha);

	}
}

static void LED_Matrix_Draw_Circle_Helper( int16_t x0, int16_t y0, int16_t rad,  uint8_t cornername, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
	int16_t f     = 1 - rad;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * rad;
	int16_t x     = 0;
	int16_t y     = rad;

	while (x<y) {
		if (f >= 0) {
		y--;
		ddF_y += 2;
		f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			LED_Matrix_Draw_Pix(x0 + x, y0 + y , r, g, b, alpha);
			LED_Matrix_Draw_Pix(x0 + y, y0 + x , r, g, b, alpha);
		}
		if (cornername & 0x2) {
			LED_Matrix_Draw_Pix(x0 + x, y0 - y , r, g, b, alpha);
			LED_Matrix_Draw_Pix(x0 + y, y0 - x , r, g, b, alpha);
		}
		if (cornername & 0x8) {
			LED_Matrix_Draw_Pix(x0 - y, y0 + x , r, g, b, alpha);
			LED_Matrix_Draw_Pix(x0 - x, y0 + y , r, g, b, alpha);
		}
		if (cornername & 0x1) {
			LED_Matrix_Draw_Pix(x0 - y, y0 - x , r, g, b, alpha);
			LED_Matrix_Draw_Pix(x0 - x, y0 - y , r, g, b, alpha);
		}
	}
}


void LED_Matrix_Draw_Fill_Circle(int16_t x0, int16_t y0, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
	LED_Matrix_Draw_V_line(x0, y0-rad, 2*rad+1,  r,  g,  b,  alpha);
	LED_Matrix_Draw_Fill_Circle_Helper(x0, y0, rad, 3,  0 , r,  g,  b,  alpha);
}


static void LED_Matrix_Draw_Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t rad,  uint8_t cornername, int16_t delta, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
	int16_t f     = 1 - rad;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * rad;
	int16_t x     = 0;
	int16_t y     = rad;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1) {
			LED_Matrix_Draw_V_line(x0+x, y0-y, 2*y+1+delta,  r,  g,  b,  alpha);
			LED_Matrix_Draw_V_line(x0+y, y0-x, 2*x+1+delta,  r,  g,  b,  alpha);
		}
		if (cornername & 0x2) {
			LED_Matrix_Draw_V_line(x0-x, y0-y, 2*y+1+delta,  r,  g,  b,  alpha);
			LED_Matrix_Draw_V_line(x0-y, y0-x, 2*x+1+delta,  r,  g,  b,  alpha);
		}
	}
}

// Draw a rounded rectangle
void LED_Matrix_Draw_Round_Rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
	// smarter version
	LED_Matrix_Draw_H_line(x+rad  , y      , w-2*rad, r,  g,  b,  alpha); // Top
	LED_Matrix_Draw_H_line(x+rad  , y+h-1  , w-2*rad, r,  g,  b,  alpha); // Bottom
	LED_Matrix_Draw_V_line(x      , y+rad  , h-2*rad, r,  g,  b,  alpha); // Left
	LED_Matrix_Draw_V_line(x+w-1  , y+rad  , h-2*rad, r,  g,  b,  alpha); // Right

	// draw four corners
	LED_Matrix_Draw_Circle_Helper(x+rad    , y+rad    , rad, 1, r,  g,  b,  alpha);
	LED_Matrix_Draw_Circle_Helper(x+w-rad-1, y+rad    , rad, 2, r,  g,  b,  alpha);
	LED_Matrix_Draw_Circle_Helper(x+w-rad-1, y+h-rad-1, rad, 4, r,  g,  b,  alpha);
	LED_Matrix_Draw_Circle_Helper(x+rad    , y+h-rad-1, rad, 8, r,  g,  b,  alpha);
}

// Fill a rounded rectangle
void LED_Matrix_Draw_Fill_Round_Rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
	// smarter version
	LED_Matrix_Draw_Fill_Rect(x+rad, y, w-2*rad, h, r,  g,  b,  alpha);

	// draw four corners
	LED_Matrix_Draw_Fill_Circle_Helper(x+w-rad-1, y+rad, rad, 1, h-2*rad-1, r,  g,  b,  alpha);
	LED_Matrix_Draw_Fill_Circle_Helper(x+rad    , y+rad, rad, 2, h-2*rad-1, r,  g,  b,  alpha);
}

// Draw a triangle
void LED_Matrix_Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
	LED_Matrix_Draw_line(x0, y0, x1, y1, r,  g,  b,  alpha);
	LED_Matrix_Draw_line(x1, y1, x2, y2, r,  g,  b,  alpha);
	LED_Matrix_Draw_line(x2, y2, x0, y0, r,  g,  b,  alpha);
}

// Fill a triangle
void LED_Matrix_Draw_Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		_swap_int16_t(y0, y1);
		_swap_int16_t(x0, x1);
	}
	if (y1 > y2) {
		_swap_int16_t(y2, y1);
		_swap_int16_t(x2, x1);
	}
	if (y0 > y1) {
		_swap_int16_t(y0, y1);
		_swap_int16_t(x0, x1);
	}

	if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if(x1 < a){
			a = x1;
		}else if(x1 > b){
			b = x1;
		}

		if(x2 < a){
			a = x2;
		}else if(x2 > b){
			b = x2;
		}
		LED_Matrix_Draw_H_line(a, y0, b-a+1, red,  green,  blue,  alpha);
		return;
	}

	int16_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1;

	int32_t
		sa   = 0,
		sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
	if(y1 == y2){
		last = y1;   // Include y1 scanline
	}else{
		last = y1-1; // Skip it
	}

	for(y=y0; y<=last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b){
			_swap_int16_t(a,b);
		}
		LED_Matrix_Draw_H_line(a, y, b-a+1, red,  green,  blue,  alpha);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b){
			_swap_int16_t(a,b);
		}
		LED_Matrix_Draw_H_line(a, y, b-a+1, red,  green,  blue,  alpha);
	}
}

void LED_Matrix_Demo_1(void){
	uint8_t demo_select=1;
	uint8_t state= 0;
	uint8_t fade= 0;
	uint32_t delay_set =0;
	while (1)
	{

		switch(demo_select){
		case 0:
			delay_set = 500000;
			switch(state++){
				//Clear Buffer			 R G B a
				case 0: LED_Matrix_Clear(0,0,0);					break;
				//Draw singel pix			x y  R G B a
				case 1: LED_Matrix_Draw_Pix(0,0,255,0,0,255);		break;
				case 2: LED_Matrix_Draw_Pix(10,0,127,127,0,255);	break;
				case 3: LED_Matrix_Draw_Pix(0,9,0,0,255,255);		break;
				case 4: LED_Matrix_Draw_Pix(10,9,120,120,120,255);	break;
				//Draw Horizontal Line		   x y l r g b a
				case 5: LED_Matrix_Draw_H_line(2,2,5,150,0,100,255); break;
				//Draw Vertical Line		   x y l  r g b a
				case 6: LED_Matrix_Draw_V_line(8,1,8,127,127,0,255); break;
				//Draw Rectangel			 x y w h r  g b a
				case 7: LED_Matrix_Draw_Rect(3,4,4,6,0,0,255,255); 	break;
				case 10: state = 0; demo_select++;					break;
				default:  											break;
			}
			break;


		case 1:
			delay_set = 25000;
			LED_Matrix_Clear(128-state/2,128-state/2,128-state/2);	//clear with wihte

			LED_Matrix_Draw_Pix(0,0,255,0,0,state);
			LED_Matrix_Draw_Pix(10,0,150,150,0,state);
			LED_Matrix_Draw_Pix(0,9,0,0,255,state);
			LED_Matrix_Draw_Pix(10,9,150,150,150,state);
			LED_Matrix_Draw_H_line(2,2,5,220,0,50,state);
			LED_Matrix_Draw_V_line(8,1,8,150,150,0,state);
			LED_Matrix_Draw_Rect(3,4,4,5,0,0,255,state);


			state++;
			if(state >= 255 ){
				state = 0;
				demo_select++;
			}
		break;

		case 2:
			delay_set = 750000;
			switch(state++){
				//Clear Buffer			 R G B
				case 0: LED_Matrix_Clear(75,0,0);					break;

				//Draw Horizontal Line		   x y l r g b a
				case 1: LED_Matrix_Draw_H_line(0,1,5,0,255,0,32);	break;
				case 2: LED_Matrix_Draw_H_line(1,3,5,0,255,0,64);	break;
				case 3: LED_Matrix_Draw_H_line(2,5,5,0,255,0,128);	break;
				case 4: LED_Matrix_Draw_H_line(3,7,5,0,255,0,196);	break;
				case 5: LED_Matrix_Draw_H_line(4,9,5,0,255,0,255); 	break;
				//Draw Vertical Line		   x y l  r g b a
				case 6: LED_Matrix_Draw_V_line(1,1,9,0,0,255,32); 	break;
				case 7: LED_Matrix_Draw_V_line(3,2,8,0,0,255,64); 	break;
				case 8: LED_Matrix_Draw_V_line(5,3,7,0,0,255,128); 	break;
				case 9: LED_Matrix_Draw_V_line(7,4,6,0,0,255,196); 	break;
				case 10: LED_Matrix_Draw_V_line(9,5,5,0,0,255,255); break;

				//Draw Rectangel			 x y w h r  g b a
				case 11: LED_Matrix_Draw_Rect(6,1,4,8,0,0,40,0); 	break;
				case 12: state = 0; demo_select++;					break;
				default:  											break;
			}
			break;


			case 3:
				delay_set = 750000;
				switch(state++){
					//Clear Buffer			 R G B
					case 0: LED_Matrix_Clear(0,0,25);					break;

					//Draw Horizontal Line		   x y l r g b a
					case 1:  LED_Matrix_Clear(0,0,25); LED_Matrix_Draw_Circle(0,0,0,0,100,0,255);	break;
					case 2:  LED_Matrix_Clear(0,0,25); LED_Matrix_Draw_Circle(1,1,1,0,100,0,255);	break;
					case 3:  LED_Matrix_Clear(0,0,25); LED_Matrix_Draw_Circle(2,2,2,0,100,0,255);	break;
					case 4:  LED_Matrix_Clear(0,0,25); LED_Matrix_Draw_Circle(3,3,3,0,100,0,255);	break;
					case 5:  LED_Matrix_Clear(0,0,25); LED_Matrix_Draw_Circle(4,4,4,0,100,0,255);	break;
					case 6:  LED_Matrix_Clear(0,0,25); LED_Matrix_Draw_Circle(5,5,5,0,100,0,255);	break;
					case 7:  LED_Matrix_Clear(0,0,25); LED_Matrix_Draw_Fill_Circle(5,5,5,0,100,0,255);	break;
					case 8:  LED_Matrix_Draw_Fill_Circle(5,5,4,50,50,0,255);	break;
					case 9:  LED_Matrix_Draw_Fill_Circle(5,5,3,100,0,0,255);	break;
					case 10: LED_Matrix_Draw_Fill_Circle(5,5,2,50,0,50,255);	break;
					case 11: LED_Matrix_Draw_Fill_Circle(5,5,1,0,0,100,255);	break;
					case 12: LED_Matrix_Draw_Fill_Circle(5,5,0,0,50,50,255);	break;

					case 13: state = 0; demo_select++;					break;
					default:  											break;
				}

				break;

				case 4:
					delay_set = 750000;
					switch(state++){
						//Clear Buffer			 R G B
						case 0: LED_Matrix_Clear(0,0,0);					break;

						//Draw Horizontal Line		  			  x  y  w  h        r   g   b   a
						case 1:  LED_Matrix_Draw_Rect			( 1, 4, 4, 5,       0,  0,255, 255);	break;
						case 2:  LED_Matrix_Draw_Fill_Rect		( 7, 5, 4, 5,     255,  0,  0, 255);	break;
						case 3:  LED_Matrix_Draw_Round_Rect		( 1, 1, 9, 7, 3,    0,255,  0, 255);	break;
						case 4:  LED_Matrix_Draw_Fill_Round_Rect( 4, 1, 7, 5, 2,  255,  0,255, 255);	break;

						case 6:  LED_Matrix_Clear(0,0,0);break;
						case 7:  LED_Matrix_Draw_Triangle(2,3,7,7,10,0,100,0,0,255);	break;
						case 8:  LED_Matrix_Draw_Fill_Triangle(1,1,3,9,8,6,0,100,0,255);	break;

						case 10:  LED_Matrix_Clear(0,0,0);break;
						case 11:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 1,9,255, 128,   0,255);	break;
						case 12:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 2,9,  0, 255, 128,255);	break;
						case 13:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 3,9,128,   0, 255,255);	break;
						case 14:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 4,9,255, 128,   0,255);	break;
						case 15:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 5,9,  0, 255, 128,255);	break;
						case 16:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 6,9,128,   0, 255,255);	break;
						case 17:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 7,9,255, 128,   0,255);	break;
						case 18:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 8,9,  0, 255, 128,255);	break;
						case 19:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0, 9,9,128,   0, 255,255);	break;
						case 20:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,9,255, 128,   0,255);	break;
						case 21:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,8,  0, 255, 128,255);	break;
						case 22:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,7,128,   0, 255,255);	break;
						case 23:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,6,255, 128,   0,255);	break;
						case 24:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,5,  0, 255, 128,255);	break;
						case 25:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,4,128,   0, 255,255);	break;
						case 26:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,3,255, 128,   0,255);	break;
						case 27:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,2,  0, 255, 128,255);	break;
						case 28:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,1,128,   0, 255,255);	break;
						case 29:  LED_Matrix_Clear(0,0,0); LED_Matrix_Draw_line(0,0,10,0,255, 128,   0,255);	break;
						case 32: state = 0; demo_select++;					break;
						default:  											break;
					}
					if(state >= 10){
						delay_set = 500000;
					}
				break;

				case 5:
				delay_set = 25000;
				switch(state){
					//Clear Buffer			 R G B
					case 0: LED_Matrix_Clear(0,0,0);
						fade = 1;
						state++;
					break;
					case 1: if(fade < 255){ LED_Matrix_Draw_Fill_Rect(0,0,11,10, 0        , fade     , 255-fade ,255); fade++;}
					else                  { fade = 0; state++; } break;
					case 2: if(fade < 255){ LED_Matrix_Draw_Fill_Rect(0,0,11,10, fade     , 255-fade , 0        ,255); fade++;}
							else          { fade = 0; state++; } break;
					case 3: if(fade < 255){ LED_Matrix_Draw_Fill_Rect(0,0,11,10, 255-fade , 0        , fade     ,255); fade++;}
							else          { fade = 0; state++; } break;
					case 4: if(fade < 255 ){
								LED_Matrix_Draw_Fill_Rect(0,0, 9,8, 0        , 255      , 0 ,5);
								fade++;
							}else{
								fade = 0; state++;
							}

						break;

					case 5: if(fade < 127 ){
								LED_Matrix_Draw_Fill_Rect(0,0, 7,6, 255      , 0      , 128 ,5);
								fade++;
							}else{
								fade = 0; state++;
							}

						break;

					case 6: if(fade < 255 ){
								LED_Matrix_Draw_Fill_Rect(0,0, 5,4, 50       , 0      , 0 ,5);
								fade++;
							}else{
								fade = 0; state++;
							}

						break;

					case 7: state = 0; demo_select=0;					break;
					default:  											break;
				}
				break;

			default:
				demo_select = 0;
				break;
		}

		WS2812_Update();

		while(delay_set--){
		}
	}
}
