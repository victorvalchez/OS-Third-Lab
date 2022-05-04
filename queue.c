
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "queue.h"


// To create a queue and reserve the size specified as a parameter.
queue *queue_init(int size) {
	// Create the queue.
  	queue *q = (queue *)malloc(sizeof(queue));

	//Asign the default values.
  	q -> elements = malloc(size * sizeof(struct element));
  	q -> size = size;
  	q -> length = 0;
  	q -> head = 0;
  	q -> tail = 0;
  	return q;
}

// To ENQUEUE an element if possible or wait if not.
// NOTE: Enqueue operation is done in the head of the queue.
int queue_put(queue *q, struct element *elem){
  	if (queue_full(q) == 0) {
    	q -> elements[q -> head] = *elem;
        // To be inside the correct size of the queue.
    	q -> head = (q -> head + 1) % q->size;  
    	q -> length = q -> length + 1;
    	return 0;
  	}
  	return -1;
}

// To DEQUEUE an element if possible or wait if not.
// NOTE: Dequeue operation is done in the tail of the queue.
struct element *queue_get(queue *q) {
  	struct element *elem;
  	if (queue_empty(q) == 0) {
    	elem = &(q -> elements[q -> tail]);
    	q -> tail = (q -> tail + 1) % q -> size;
    	q -> length = q->length - 1;
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
  	if (q -> length == q->size)
    	return 1;
  	return 0;
}

// To destroy the queue and free the assigned resources.
int queue_destroy(queue *q) {
  	free(q -> elements);
	free(q);
    return 0;
}
