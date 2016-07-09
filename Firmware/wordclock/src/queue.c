/*
 * fifo.c
 *
 *  Created on: 24.05.2016
 *      Author: peter
 */
#include "queue.h"

QUEUE uart2RecQueue;

void queue_init (QUEUE* q){

	q->head = 0;
	q->tail  = 0;
}

uint8_t queue_empty (QUEUE* q){

	return (q->head == q->tail);
}

uint8_t queue_full (QUEUE* q){

	return ( (q->head+1) % MAX_QUEUE_SIZE == q->tail );
}

uint8_t queue_write (QUEUE* q, char item){

	if( queue_full(q) )
		return 1;
	q->queue[q->head] = item;
	q->head = (q->head+1) % MAX_QUEUE_SIZE;
	return 0;
}

uint8_t queue_read (QUEUE* q, char* item){

	if( queue_empty(q) )
		return 1;
	*item = q->queue[q->tail];
	q->tail = (q->tail+1) % MAX_QUEUE_SIZE;
	return 0;
}
