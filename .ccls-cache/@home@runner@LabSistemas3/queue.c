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

	queue * q = (queue *)malloc(sizeof(queue));

    q -> size = size;
    q -> head = queue -> size = 0;
    q -> tail = capacity - 1;
    q -> array = (int*)malloc(q -> capacity * sizeof(int));

	return q;
}

/*
//set values to default state
q->size = size;
q->leng = 0;
q->head = 0;
q->tail = 0;
//we define the necessesary memory by using de size and the sizeof the element (time,type)
q->data = malloc(q->size * sizeof(struct element));
*/


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
	
	return 0;
}


// To check if the queue is full (return 1) or not (return 0).
int queue_full(queue *q){
	
	return 0;
}


// To destroy the queue and free the assigned resources.
int queue_destroy(queue *q){
	return 0;
}
