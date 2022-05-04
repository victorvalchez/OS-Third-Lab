
#ifndef HEADER_FILE
#define HEADER_FILE

struct element {
    // Machine type (common node, computation node or super-computer)
    int machine_type; 
    // Time of use (in minutes).
    int time_of_use; 
};

typedef struct queue {
    int length;
    // BORRAR ESTE COMENT Y EL DE ABAJO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    struct element *elements;  //Cambiar ring en el otro archivo por elements
    // Head of the queue.
    int head;
    // Tail of the queue.
    int tail;
    // Total size of the queue.
    int size;
} queue;

queue *queue_init(int size);
int queue_destroy(queue *q);
int queue_put(queue *q, struct element *elem);
struct element *queue_get(queue *q);
int queue_empty(queue *q);
int queue_full(queue *q);

#endif
