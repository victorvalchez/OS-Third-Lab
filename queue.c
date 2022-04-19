/*
Queue functions implementations:
https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
*/


#include "queue.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// To create a queue and reserve the size specified as a parameter.
queue* queue_init(int size){

	// Create the queue.
	queue * q = (queue *)malloc(sizeof(queue));

	// Assign by default values to the variables of the queue.
	q -> head = 0;
    q -> tail = size - 1;
    q -> size = size;
	q -> capacity = 0;
    q -> elements = malloc(q -> size * sizeof(struct element));

	return q;
}


// To ENQUEUE an element if possible or wait if not.
int queue_put(queue *q, struct element* x) {
	return 0;
}


// To DEQUEUE an element if possible or wait if not.
struct element* queue_get(queue *q) {
	struct element* element;
	
	return element;
}


// To check if the queue is empty (return 1) or not (return 0).
int queue_empty(queue *q){
	
	// Check if the queue is empty (capacity = 0).
	return (q -> capacity == 0);
}


// To check if the queue is full (return 1) or not (return 0).
int queue_full(queue *q){
	
	// Check if the queue is full (size = capacity).
	return (q -> size == q -> capacity);
}


// To destroy the queue and free the assigned resources.
int queue_destroy(queue *q){

	// !!! REVISAR ESTO !!!

	// Free the assigned resources.
	free(q -> elements);
	// Destroy the queue.
	free(q);

	return 0;
}
