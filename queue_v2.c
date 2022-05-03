#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "queue.h"
// QUEUE DE RAUL

/*
Queue functions implementations:
https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
*/


// To create a queue and reserve the size specified as a parameter.
queue *queue_init(int size) {
	// Create the queue
  	queue *q = (queue *)malloc(sizeof(queue));

	//Asign the values
  	q -> elements = malloc(size * sizeof(struct element));
  	q -> size = size;  // Capacity
  	q -> length = 0;
  	q -> head = 0;
  	q -> tail = 0;
  	return q;
}

/*
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)
                  % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}
*/

// To ENQUEUE an element if possible or wait if not.
int queue_put(queue *q, struct element *elem){
  	if (queue_full(q) == 0) {
    	q -> elements[q -> tail] = *elem;
    	q -> tail = (q -> tail + 1) % q -> size;
    	q -> length = q -> length + 1;
    	return 0;
  	}
  	return -1;
}

/*
// To Enqueue an element
int queue_put(queue *q, struct element* x) {
	int next;
	next = q->head+1;

	if (next >= q->size){
		next=0;
	}

	if ((q->leng)==q->size){
		return -1;
	}

	q->leng = q->leng+1;
	q->data[q->head]=*x;
	q->head = next;

	//printf("Entrando %d,%d\n", q->data->type, q->data->time);
	return 0;
}


// To Dequeue an element.
struct element *queue_get(queue *q) {
  	struct element *elem;
  	if (queue_empty(q) == 0) {
    		elem = &(q->data[q->tail]);
		if (q->tail+1 == q->size){
			q->tail=0;
		}
		else{
			q->tail = q->tail+1;
		}
    		q->leng = q->leng - 1;
  	}
	//printf("Saliendo %d,%d\n", elem->type, elem->time);
  	return elem;
}
*/

// To DEQUEUE an element if possible or wait if not.
struct element *queue_get(queue *q) {
  	struct element *elem;
  	if (queue_empty(q) == 0) {
    	elem = &(q -> elements[q -> head]);
    	q -> head = (q -> head + 1) % q -> size;
    	q -> length = q -> length - 1;
  	}
  	return elem;
}

// To check if the queue is empty (return 1) or not (return 0).
int queue_empty(queue *q) {
  	if (q -> length == 0)
    	return 1;
  	return 0;
}

// To check if the queue is full (return 1) or not (return 0).
int queue_full(queue *q) {
  	if (q -> length == q -> size)
    	return 1;
  	return 0;
}

// To destroy the queue and free the assigned resources.
int queue_destroy(queue *q) {
  	free(q -> elements);
	free(q);
  return 0;
}