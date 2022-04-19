#ifndef HEADER_FILE
#define HEADER_FILE


struct element {
  int type; //Machine type
  int time; //Using time
};

typedef struct queue {
	//clasic settings needed for queues  
	int leng;
	int size;
	int head;
	int tail;
	//pointer to struct where data is saved 
	struct element *data;
}queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);

#endif
