#ifndef HEADER_FILE
#define HEADER_FILE


// Structure representing an element (operation) of the queue (buffer).
struct element {
    // Machine type (common node, computation node or super-computer).
    int machine_type; 
    // Time of use (in minutes).
    int time_of_use; 
};


// Structure representing the queue (buffer).
typedef struct queue {
    // Elements of the queue.
    struct element *elements;
    // Length, head, tail and total size of the queue.
    int length, head, tail, size;
} queue;


queue *queue_init(int size);
int queue_destroy(queue *q);
int queue_put(queue *q, struct element *elem);
struct element *queue_get(queue *q);
int queue_empty(queue *q);
int queue_full(queue *q);

#endif
