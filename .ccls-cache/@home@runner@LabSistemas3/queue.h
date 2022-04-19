#define HEADER_FILE
#ifndef HEADER_FILE


// Structure to represent an element.
struct element {
    // Machine type.
	int type;
    // Using time.
	int time;
};

// Structure to represent a queue.
typedef struct queue {
	// Variables for the head, tail and size of the queue.
    int head, tail, size;
	
	// int leng;

	// Pointer to the elements of the queue.
	struct element *elements
} queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);

#endif
