#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"


//To create a queue
queue *queue_init(int size) {
  	queue *q = (queue *)malloc(sizeof(queue));
	//set values to default state
  	q->size = size;
  	q->leng = 0;
  	q->head = 0;
  	q->tail = 0;
	//we define the necessesary memory by using de size and the sizeof the element (time,type)
	q->data = malloc(q->size * sizeof(struct element));
  return q;
}

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



//To check queue state
int queue_empty(queue *q) {
  if (q->leng == 0){
    return -1;
	}
  return 0;
}

int queue_full(queue *q){
    if (q->size == q->leng){
    	return -1;
	}
    return 0;
}


//To destroy the queue and free the resources
int queue_destroy(queue *q){
	free(q->data);
	free(q);
	//we liberate the dinamic memory used
    return 0;
}
