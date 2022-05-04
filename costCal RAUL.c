
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


// Integer variable to store the total cost asked as a result.
int total_cost;

// Mutex to access shared buffer.
pthread_mutex_t mutex;
pthread_mutex_t desc;

// Synchronization variable -> Can we add more elements?
pthread_cond_t non_full;
// Synchronization variable -> Can we remove elements?
pthread_cond_t non_empty;



void *producer(void * param) {
    /*
    · PRODUCER THREAD:
    - Obtain data extracted from the file.
    - Insert data one by one in the circular buffer.
    */

    pthread_exit(0);
    // Line that should not be executed if pthread_exit(0) works as expected.
    return NULL;
}



int *consumer(int * param) {
     /*
    · CONSUMER THREAD:
    - Obtain (concurrently) the elements inserted in the queue.
    - Each extracted element represents a type of machine and the time of use.
    - The consumer must calculate the cost and accumulate it until all elements have been processed.
    - Return to the main thread the partial cost calculated by each one.
    */

    pthread_exit(0);
    return NULL;
    
}


/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) {
    /*
    · Read the input arguments.
    · Load the data from the file provided into memory.
    · Distribute the file load equally among the number of producers threads indicated.
    · Create PRODUCERS.
    · Create CONSUMERS.
    · Wait until execution of PRODUCERS finish.
    · End of execution to CONSUMERS.
    · Show the total calculated cost.
    */

    


    // We check if the number of arguments is correct.
	if (argc != 4) {
    	perror("[ERROR] Invalid number of arguments (<file_name> <num_producers> <num_consumers> <buff_size>).");
    	return -1;
  	}

    // We open the file given as argument.





    // ¡¡¡ OBTENER ESTOS DOS PARÁMETROS DEL CALL AL PROGRAMA !!!
    int num_producers;
    int num_consumers;
    pthread_t ths[num_producers + num_consumers]
    // We initialize the threads needed.

    // For PRODUCERS.
    for(int i = 0; i < num_producers; i++) {
        pthread_create(&(ths[i]), NULL, producer, &i);

    }

    // For CONSUMERS.
    for(int i = 0; i < num_consumers; i++) {
        pthread_create(&(ths[i]), NULL, consumer, &i);

    }


    

    


    // We create the buffer which is a queue in this case.
    buffer = queue_init(size);

    // Mutexes to deal with the shared buffer (queue).
    // During initialization, check all them to see if there is any error.
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the mutex.")
        return(-1)
    }
    if (pthread_mutex_init(&desc, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the mutex.")
        return(-1)
    }
    // Condition variable non_full.
    if (pthread_mutex_init(&non_full, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the condition variable.")
        return(-1)
    }
    // Condition variable non_empty.
    if (pthread_mutex_init(&non_empty, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the condition variable.")
        return(-1)
    }




    






    // The final total cost (sum of partial costs) is printed on screen.
    printf("Total: %d euros.\n", total_cost)

    // We invoke queue_destroy to destroy the queue and free the assigned resources.
    queue_destroy(buffer)

    // We close the descriptors used (checking if there is any error).
    if(fclose(descriptorP) < 0) {
        perror("[ERROR] Error while closing the descriptor.")
    }
    if(fclose(descriptor) < 0) {
        perror("[ERROR] Error while closing the descriptor.")
    }
    
    // We destroy all the mutexes and the conditional variables (checking if there is any error).
    if(pthread_mutex_destroy(&mutex) < 0) {
        perror("[ERROR] Error while destroying the mutex.")
    }
    if(pthread_mutex_destroy(&desc) < 0) {
        perror("[ERROR] Error while destroying the mutex.")
    }
    if(pthread_cond_destroy(&non_empty) < 0) {
        perror("[ERROR] Error while destroying the conditional variable.")
    }
    if(pthread_cond_destroy(&non_full) < 0) {
        perror("[ERROR] Error while destroying the conditional variable.")
    }

    // 0 is returned at the end of the main function.
    return 0;
}
