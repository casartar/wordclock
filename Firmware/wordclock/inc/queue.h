/*
 * fifo.h
 *
 *  Created on: 24.05.2016
 *      Author: peter
 */

#include <inttypes.h>

#ifndef QUEUE_H_
#define QUEUE_H_

#define MAX_QUEUE_SIZE 512

typedef struct{
	volatile uint16_t tail;
	volatile uint16_t head;
	volatile char queue[MAX_QUEUE_SIZE];
}QUEUE;

extern QUEUE uart2RecQueue;

void queue_init (QUEUE* q);
uint8_t queue_empty (QUEUE* q);
uint8_t queue_full (QUEUE* q);
uint8_t queue_write (QUEUE* q, char item);
uint8_t queue_read (QUEUE* q, char* item);


#endif /* QUEUE_H_ */
