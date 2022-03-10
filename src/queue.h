#ifndef _QUEUE_H_
#define _QUEUE_H_

/* 
* Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "link.h"


/* 
* Datatypes 
*/

typedef struct request 
{
	int  operation;
	int  block_id;
	char block[BLOCK_SIZE];
	pid_t  pid_process[MAX_PROCESS_PER_REQUEST];  //Pid del proceso que ha realizado la peticion 
	int pidOcupados;
	int error;
	/* TODO: ADD YOUR OWN REQUEST STRUCTURE FIELDS */

} request;

struct queue_element
{
	request req;
	struct queue_element* next;
	struct queue_element* prev;
};

struct queue
{
	struct queue_element* head;
	struct queue_element* tail;
};


/* 
* Functions 
*/

/* Create an empty queue */
struct queue* queue_new ( void );

/* Destroy a previously created queue */ 
void queue_destroy ( struct queue *q );


/* Enqueue an element */
struct queue* enqueue ( struct queue* s, request * req );

/* Try to enqueue an element in an ordered fashion */
struct queue* enqueue_ordered ( struct queue* s, request * req );

/* Remove an element */
int remove_request ( struct queue* s, request * req );


/* To find an existing request */
request * find_request ( struct queue* s, int block_id, int operation );

/* Get the next element, or return the first one in the queue */
request * next_request ( struct queue* s, request * req );

/* Return 1 if the queue is empty and 0 otherwise */
int is_queue_empty ( struct queue* s );

//propia cosecha
request * find_request_impachetabil ( struct queue* s, int block_id, int operation );

#endif

