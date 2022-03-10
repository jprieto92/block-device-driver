
/*
*  queue.c
*
*  DrvSim. Version 2.0
*
*  INFODSO@ARCOS.INF.UC3M.ES
*
*/

#include "queue.h"


/*
* Creates a new queue.
*
* Returns the pointer to the new created queue.
*
*/
struct queue* queue_new ( void )
{
	struct queue* p ;

	/* 1) To prepare a new queue */
	p = malloc(sizeof(struct queue));
	if ( NULL == p )
	{
		fprintf(stderr, "[%s:%d]: malloc() has failed\n", __FILE__, __LINE__);
		return p;
	}

	/* 2) To initializate the queue */
	p->head = p->tail = NULL;
	return p;
}

/*
* Destroy the queue.
*
* Dequeues every element and it frees each element.
*
*/
void queue_destroy ( struct queue *q )
{
	struct queue_element *u = NULL;
	struct queue_element *v = NULL;

	/* 1) To perform some error checking */
	if ( NULL == q )
	{
		fprintf(stderr, "[%s:%d]: no queue to destroy.\n", __FILE__, __LINE__);
		return;
	}
	if ( ((NULL == q->head) && (NULL != q->tail)) || ((NULL != q->head) && (NULL == q->tail)) )
	{
		fprintf(stderr, "[%s:%d]: There is something seriously wrong with your assignment of head/tail to the list.\n", __FILE__, __LINE__);
		return;
	}

	/* 2) removing every element, and the queue itself */
	u = q->head;
	while (NULL != u)
	{
		v = u;
		u = u->next;
		free(v);
	}

	free(q);
}


/*
* Enqueues a requests in the queue given by parameter.
* Returns a pointer to the new queue.
*
* NOTE: the element is enqueued by value (the functions makes a copy o the element)
* not by reference.
*
*/
struct queue* enqueue ( struct queue* s, request * req )
{
	struct queue_element* p ;

	/* 1) To perform some error checking */
	if ( NULL == s )
	{
		fprintf(stderr, "[%s:%d]: queue not initialized\n", __FILE__, __LINE__);
		return s;
	}

	if ( ((NULL == s->head) && (NULL != s->tail)) || ((NULL != s->head) && (NULL == s->tail)) )
	{
		fprintf(stderr, "[%s:%d]: There is something seriously wrong with your assignment of head/tail to the list.\n", __FILE__, __LINE__);
		return NULL;
	}

	/* 2) To prepare a new element to be inserted */
	p = malloc(sizeof(struct queue_element));
	if ( NULL == p )
	{
		fprintf(stderr, "[%s:%d]: malloc() has failed\n", __FILE__, __LINE__);
		return s;
	}

	memcpy(&(p->req), req, sizeof(request));
	p->next = NULL;
	p->prev = NULL;

	/* 3) To insert the new element (at the end) */
	if ( NULL == s->head && NULL == s->tail )
	{
		s->head = s->tail = p;
		return s;
	}

	s->tail->next = p;
	p->prev = s->tail;
	s->tail = p;

	return s;
}


/*
* Checks if the queue is empty.
* Returns 1 it is empty and 0 in other case.
*/
int is_queue_empty ( struct queue* s )
{
	/* 1) To check if it is empty (no queue) */
	if (NULL == s)
	{
		fprintf(stderr, "[%s:%d]: queue not initialized (and empty because that)\n", __FILE__, __LINE__);
		return 1;
	}

	/* 2) To check if it is empty (no elements) */
	if (s->head == NULL)
	{
		return 1;
	}

	/* 3) There are elements */
	return 0;
}


/*
* To find an existing request.
*
* Returns NULL if not found, or a pointer to the existing request.
*
*/
request * find_request ( struct queue* s, int block_id, int operation )
{
	struct queue_element* u = NULL;
	/* 1) To perform some error checking */
	if ( NULL == s )
	{
		fprintf(stderr, "[%s:%d]: queue not initialized\n",__FILE__, __LINE__);
		return NULL;
	}

	if ( ((NULL == s->head) && (NULL != s->tail)) || ((NULL != s->head) && (NULL == s->tail)) )
	{
		fprintf(stderr, "[%s:%d]: There is something seriously wrong with your assignment of head/tail to the list.\n", __FILE__, __LINE__);
		return NULL;
	}

	/* 2) To find the element */
	if ( NULL == s->head && NULL == s->tail ) return NULL;

	u = s->head;
	while (NULL != u)
	{
		if ( (u->req.block_id == block_id) && (u->req.operation == operation) )
		return &(u->req);
		u = u->next;
	}

	return NULL;
}

request * find_request_impachetabil ( struct queue* s, int block_id, int operation )
{
	struct queue_element* u = NULL;
	struct queue_element* v = NULL;

	/* 1) To perform some error checking */
	if ( NULL == s )
	{
		fprintf(stderr, "[%s:%d]: queue not initialized\n",__FILE__, __LINE__);
		return NULL;
	}

	if ( ((NULL == s->head) && (NULL != s->tail)) || ((NULL != s->head) && (NULL == s->tail)) )
	{
		fprintf(stderr, "[%s:%d]: There is something seriously wrong with your assignment of head/tail to the list.\n", __FILE__, __LINE__);
		return NULL;
	}

	/* 2.a) To find the element: empty */
	if ( NULL == s->head && NULL == s->tail )
             return NULL;

	/* 2.b) To find the element: last */
	if( ( s->tail->req.block_id == block_id) && (s->tail->req.operation == operation) && (s->tail->req.pidOcupados < 10))
	 	return &(s->tail->req);

	/* 2.b) To find the element: first & middle */
	u = s->head;
	if (u != NULL)
	    v = u->next;

	while (NULL != v)
	{
		if ( (u->req.block_id == block_id) && (block_id < v->req.block_id) )
		  {
		        if ( (u->req.operation == operation) && (u->req.pidOcupados < 10) )
		        {
			       return &(u->req);
		        }else{
			       return NULL;
		        }

  		}
		u = v;
		v = v->next;

	}
	return NULL;
}


/*
* Enqueues a requests in the queue given by parameter, ordered by the block_id value.
* Returns a pointer to the new queue.
*
* NOTE: the element is enqueued by value (the functions makes a copy o the element)
*
*/
struct queue* enqueue_ordered ( struct queue* s, request * req )
{
	struct queue_element* u = NULL;
	struct queue_element* v = NULL;
	struct queue_element* p ;

	/* 1) To perform some error checking */
	if ( NULL == s )
	{
		fprintf(stderr,"[%s:%d]: queue not initialized\n",__FILE__, __LINE__);
		return s;
	}

	if ( ((NULL == s->head) && (NULL != s->tail)) || ((NULL != s->head) && (NULL == s->tail)) )
	{
		fprintf(stderr, "[%s:%d]: There is something seriously wrong with your assignment of head/tail to the list.\n",__FILE__, __LINE__);
		return NULL;
	}

	/* 2) To prepare a new element to be inserted */
	p = malloc(sizeof(struct queue_element));
	if ( NULL == p )
	{
		fprintf(stderr, "[%s:%d]: malloc() has failed\n", __FILE__, __LINE__);
		return s;
	}

	memcpy(&(p->req), req, sizeof(request));
	p->next = NULL;
	p->prev = NULL;

	/* 3.a) To insert the new element (alone) */
	if ( NULL == s->head && NULL == s->tail )
	{
		s->head = s->tail = p;
		return s;
	}

	/* 3.b) To insert the new element (at the beginning) */
	u = s->head;
	if ( p->req.block_id < u->req.block_id )
	{
		p->next = u;
		u->prev = p;
		s->head = p;
		return s;
	}

	/* 3.c) To insert the new element (at the end) */
	u = s->tail;
	if ( p->req.block_id >= u->req.block_id )
	{
		u->next = p;
		p->prev = u;
		s->tail = p;
		return s;
	}

	/* 3.d) To insert the new element (between two) */
	u = s->head;
	v = u->next;
	while (NULL != v)
	{
		if ( (u->req.block_id <= p->req.block_id) && (p->req.block_id < v->req.block_id) )
		{
			u->next = p;
			p->prev = u;
			v->prev = p;
			p->next = v;
			return s;
		}
		u = v;
		v = v->next;
	}

	fprintf(stderr, "[%s:%d]: no place to be inserted, enqueue_ordered has failed\n", __FILE__, __LINE__);
	return NULL;
}

int remove_request ( struct queue* q, request * req )
{
	struct queue_element* u ;

	/* 1) To perform some error checking */
	if ( NULL == q )
	{
		fprintf(stderr, "[%s:%d]: queue not initialized\n", __FILE__, __LINE__);
		return -1;
	}

	if ( ((NULL == q->head) && (NULL != q->tail)) || ((NULL != q->head) && (NULL == q->tail)) )
	{
		fprintf(stderr,"[%s:%d]: There is something seriously wrong with your assignment of head/tail to the list.\n",__FILE__, __LINE__);
		return -1;
	}

	/* 2) Search the element to be removed */
	u = q->head;
	while (NULL != u)
	{
		if ( &(u->req) == req )
		{
			if (u == q->head || u == q->tail)
			{
				if (u == q->head && u == q->tail)
				{
					q->head = q->tail = NULL;
				}
				if (u == q->head)
				{
					q->head = u->next;
					q->head->prev = NULL;
				}
				if (u == q->tail)
				{
					q->tail = u->prev;
					q->tail->next = NULL;
				}
			} else {
				u->prev->next = u->next;
				u->next->prev = u->prev;
			}

			free(u);
			return 1;
		}

		u = u->next;
	}

	fprintf(stderr,"[%s:%d]: Element to be removed was not found in the queue.\n",__FILE__, __LINE__);
	return -1;
}

request * next_request ( struct queue* s, request * req )
{
	struct queue_element* v ;

	v = s->head;
	while (NULL != v)
	{
		if ( &(v->req) == req )
		{
			if (NULL != v->next) return &(v->next->req);

			if (v != s->head) return &(s->head->req);
		}

		v = v->next;
	}

	return NULL;
}
