
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


// ESTO QUÉ ES??? -> Cambiar nombre y explicar !!!!
FILE *descriptorP
const char *file


// !!!! STRUCTURE que hay que usar pero no sé para qué sirve  ????????
struct opers {
  int op1;
  int op2;
};



void *producer(void * param) {
    /*
    · PRODUCER THREAD:
    - Obtain data extracted from the file.
    - Insert data one by one in the circular buffer.
    */



    // (CODE HERE)

    

    // We LOCK the mutex desc and check if there is any error.
    if (pthread_mutex_lock(&desc) < 0) {
        perror("[ERROR] Error while locking the mutex.");
    	return -1;
    }

    // !!!! CAMBIAR NOMBRE A descriptorP CUANDO SE SEPA LO QUE ES !!!!
    descriptorP = fopen(file, "r");
    if (descriptorP == NULL) {
        perror("[ERROR] Error while opening the file.");
    	return -1;
    }



    // (CODE HERE)




    // We UNLOCK the mutex desc and check if there is any error.
    if (pthread_mutex_unlock(&desc) < 0) {
        perror("[ERROR] Error while unlocking the mutex.");
    	return -1;
    }



    // (INTRODUCE THINGS IN THE CIRCULAR BUFFER)
    

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

    /*
    We define some integer variables that we will use later:
        - num_operations -> Number of operations indicated by the user as argument in the call.
        - num_lines -> Number of lines of the file specified as argument in the call.
        - num_producers -> Number of producers indicated as argument in the call.
        - num_consumers -> Number of consumers indicated as argument in the call.
        - buff_size -> Size of the buffer (queue) given as argument in the call.
    */
    int num_operations, num_lines, num_producers, num_consumers, buff_size;
    
    // We extract the number of producers from the 2nd argument of the call.
    num_producers = atoi(argv[2]);
    if (num_producers <= 0) {
        perror("[ERROR] Number of producers must be at least 1.");
        return(-1);
    }

    // We extract the number of consumers from the 3th argument of the call.
    num_consumers = atoi(argv[3]);
    if (num_consumers <= 0) {
        perror("[ERROR] Number of consumers must be at least 1.");
        return(-1);
    }

    // We extract the size of the buffer from the 4th argument of the call.
    buff_size = atoi(argv[4]);
    if (buff_size <= 0) {
        perror("[ERROR] Buffer size must be at least 1.");
        return(-1);
    }

    // We open the file given as 1st argument and we store it into descriptor variable.
    FILE *descriptor = fopen(argv[1], "r");

    // We check if there was an error while opening the previous file.
    if (descriptor == NULL) {
        perror("[ERROR] Error while opening the file given as argument.");
        return(-1);
    }

    // We extract the number of operations from the file (first line of the file).
    if (fscan(descriptor, "%d", &num_operations) < 0) {
        perror("[ERROR] Error looking for the number of operations.");
        return(-1);
    }

    // Firstly, we calculate the number of operations of the file.
    char chr;
    // End of file (feof) is used to check whether we have checked the entire file to count the lines.
    while(!feof(descriptor)) {
        // Read a character.
        chr = fgetc(descriptor);
        // If the character is a line break, the counter (num_lines) is incremented.
        if (chr == '\n') {
            num_lines++
        }
    }
    if (fclose(descriptor) < 0) {
        perror("[ERROR] Error closing the file descriptor.");
        return(-1);
    }
    
    // We check that the number of operations is less or equal than the total lines.
    // There may NOT be fewer operations in the file than the number of operations.
    // Therefore, operations <= lines
    // Note that num_lines - 1 is done since we do not consider the first line (number of operations).
    int real_lines = num_lines - 1;
    if (num_operations > real_lines) {
        perror("[ERROR] There can not be fewer operations in the file than operations selected.");
        return(-1);
    }

    // We create the buffer which is a queue in this case.
    buffer = queue_init(buff_size);

    // We initialize mutexes to deal with the shared buffer (queue).
    // During initialization, check all them to see if there is any error.
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the mutex.");
        return(-1);
    }
    if (pthread_mutex_init(&desc, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the mutex.");
        return(-1);
    }
    // Condition variable non_full.
    if (pthread_mutex_init(&non_full, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the condition variable.");
        return(-1);
    }
    // Condition variable non_empty.
    if (pthread_mutex_init(&non_empty, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the condition variable.");
        return(-1);
    }


    

    // !!!!!!!!!! ESTO ES DEL VIDEO DE ALEJANDRO !!!!!!!!!! 
    pthread_t threads[num_producers + num_consumers]
    // We initialize the threads needed.

    // For PRODUCERS.
    for(int i = 0; i < num_producers; i++) {
        pthread_create(&(threads[i]), NULL, producer, &i);
    }

    // For CONSUMERS.
    for(int i = 0; i < num_consumers; i++) {
        pthread_create(&(threads[i]), NULL, consumer, &i);
    }





    // (CREATION OF THREADS AND HOW MANY OPERATIONS DO EACH OF THEM)
    





    // The final total cost (sum of partial costs) is printed on screen.
    printf("Total: %d euros.\n", total_cost)

    // We invoke queue_destroy to destroy the queue and free the assigned resources.
    queue_destroy(buffer)

    // We close the descriptors used (checking if there is any error).
    if (fclose(descriptorP) < 0) {
        perror("[ERROR] Error while closing the descriptor.");
        return(-1);
    }
    if (fclose(descriptor) < 0) {
        perror("[ERROR] Error while closing the descriptor.");
        return(-1);
    }
    
    // We destroy all the mutexes and the conditional variables (checking if there is any error).
    if (pthread_mutex_destroy(&mutex) < 0) {
        perror("[ERROR] Error while destroying the mutex.");
        return(-1);
    }
    if (pthread_mutex_destroy(&desc) < 0) {
        perror("[ERROR] Error while destroying the mutex.");
        return(-1);
    }
    if (pthread_cond_destroy(&non_empty) < 0) {
        perror("[ERROR] Error while destroying the conditional variable.");
        return(-1);
    }
    if (pthread_cond_destroy(&non_full) < 0) {
        perror("[ERROR] Error while destroying the conditional variable.");
        return(-1);
    }

    // 0 is returned at the end of the main function.
    return 0;
}
