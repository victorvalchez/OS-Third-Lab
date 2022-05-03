#include "queue.h"
#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//Creacion de mutex, variables condición y descriptor de fichero:
//ring controla el buffer
//des controla el descriptor de fichero
pthread_mutex_t elements;
pthread_mutex_t des;
pthread_cond_t lleno;
pthread_cond_t vacio;
struct queue *q;
const char *fichero;
FILE *descriptorP;
int total = 0;

struct param {
  int id_ini;
  int op;
};

// Mutex to access shared buffer.
pthread_mutex_t mutex;

// Synchronization variable -> Can we add more elements?
pthread_cond_t non_full;
// Synchronization variable -> Can we remove elements?
pthread_cond_t non_empty;


/*
PRODUCER THREAD:
- Obtain data extracted from the file.
- Insert data one by one in the circular buffer.
*/

void producer() {

    pthread_exit(0);
}


/*
CONSUMER THREAD:
- Obtain (concurrently) the elements inserted in the queue.
- Each extracted element represents a type of machine and the time of use.
- The consumer must calculate the cost and accumulate it until all elements have been processed.
- Return to the main thread the partial cost calculated by each one.
*/
int consumer(int *consumers) {
    
}


/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) {

    // Read the input arguments.
    // Load the data from the file provided into memory.
    // Distribute the file load equally among the number of producers threads indicated.
    // Create PRODUCERS.
    // Create CONSUMERS.
    // Wait until execution of PRODUCERS finish.
    // End of execution to CONSUMERS.
    // Show the total calculated cost.


    // We check if the number of arguments is correct.
	if (argc != 4) {
    	perror("[ERROR] Invalid number of arguments");
    	return -1;
  	}

    // We open the file given as argument.
    


    // SLIDE 37 - CONCURRENCY [LECTURE 2]
    pthread_t producers_thread, consumers_thread;
    
    pthread_mutex_init(&mutex, NULL);
    
    pthread_cond_init(&non_full, NULL);
    pthread_cond_init(&non_empty, NULL);
    
    pthread_create(&producers_thread, NULL, producer, NULL);
    pthread_create(&consumers_thread, NULL, consumer, NULL);
    
    pthread_join(producers_thread, NULL);
    pthread_join(consumers_thread, NULL);
    
    pthread_mutex_destroy(&mutex);
    
    pthread_cond_destroy(&non_full);
    pthread_cond_destroy(&non_empty);

    

    return 0;
}
