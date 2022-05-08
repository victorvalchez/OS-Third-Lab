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
#include <math.h>


// Declaration of functions for the producer and consumer that will be used.
void *producer(void *arg);
void *consumer(void *arg);

// Integer variable to store the total cost asked as a result.
int total_cost = 0;

// Mutex to access shared buffer.
pthread_mutex_t mutex;
pthread_mutex_t desc;

// Synchronization variable -> Can we add more elements?
pthread_cond_t non_full;
// Synchronization variable -> Can we remove elements?
pthread_cond_t non_empty;


// To get the descriptor of the file being processed.
FILE *descProducer;
// To pass the file being processed argv[1] to this character.
const char *file;

// Structure which is the buffer (queue) defined in queue.c.
struct queue *buffer;

// This structures contain the number of operations and the starting line for each producer and consumer.
struct producer_params {
  int initial_id;
  int operations;
};

// In case of consumers, only the number of operations is considered.
struct consumer_params {
  int operations;
};



/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) {
    /*
    The main function is responsible of several tasks:
    · Read the input arguments.
    · Load the data from the file provided into memory.
    · Distribute the file load equally among the number of producers threads indicated.
    · Create PRODUCERS.
    · Create CONSUMERS.
    · Wait until execution of PRODUCERS finish.
    · End of execution to CONSUMERS.
    · Show the total calculated cost.
    */

    // We check if the number of arguments is correct. (5 because the argv[0] is the name of the file)
	if (argc != 5) {
    	perror("[ERROR] Invalid number of arguments (<file_name> <num_producers> <num_consumers> <buff_size>).\n");
    	return(-1);
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
        perror("[ERROR] Number of producers must be at least 1.\n");
        return(-1);
    }

    // We extract the number of consumers from the 3th argument of the call.
    num_consumers = atoi(argv[3]);
    if (num_consumers <= 0) {
        perror("[ERROR] Number of consumers must be at least 1.\n");
        return(-1);
    }

    // We extract the size of the buffer from the 4th argument of the call.
    buff_size = atoi(argv[4]);
    if (buff_size <= 0) {
        perror("[ERROR] Buffer size must be at least 1.\n");
        return(-1);
    }

    // We open the file given as 1st argument and we store it into descriptor variable.
    FILE *descriptor = fopen(argv[1], "r");

    // We check if there was an error while opening the previous file.
    if (descriptor == NULL) {
        perror("[ERROR] Error while opening the file given as argument.\n");
        return(-1);
    }

    // We extract the number of operations from the file (first line of the file).
    if (fscanf(descriptor, "%d", &num_operations) < 0) {
        perror("[ERROR] Error looking for the number of operations.\n");
        return(-1);
    }

    // Firstly, we calculate the number of operations of the file.
    char character;
    num_lines = 0;
    // End of file (feof) is used to check whether we have checked the entire file to count the lines.
    while(!feof(descriptor)) {
        // Read a character.
        character = fgetc(descriptor);
        // If the character is a line break, the counter (num_lines) is incremented.
        if (character == '\n') {
            num_lines++;
        }
    }
    // We check for errors while closing the descriptor.
    if (fclose(descriptor) < 0) {
        perror("[ERROR] Error closing the file descriptor.\n");
        return(-1);
    }
    
    // We check that the number of operations is less or equal than the total lines.
    // There may NOT be fewer operations in the file than the number of operations.
    // Therefore, operations <= lines
    // Note that num_lines - 1 is done since we do not consider the first line (number of operations).
    int real_lines = num_lines - 1;
    if (num_operations > real_lines) {
        perror("[ERROR] There can not be fewer operations in the file than operations selected.\n");
        return(-1);
    }

    // We create the buffer which is a queue in this case, with the size introduced as argument.
    buffer = queue_init(buff_size);

    // We initialize mutexes to deal with the shared buffer (queue).
    // During initialization, check all them to see if there is any error.
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the mutex.\n");
        return(-1);
    }
    if (pthread_mutex_init(&desc, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the mutex.\n");
        return(-1);
    }
    // Condition variable non_full.
    if (pthread_cond_init(&non_full, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the condition variable.\n");
        return(-1);
    }
    // Condition variable non_empty.
    if (pthread_cond_init(&non_empty, NULL) < 0) {
        perror("[ERROR] Error in the initialization of the condition variable.\n");
        return(-1);
    }

    
    // Index to point to the beginning (initial) line.
    int init;

    // We define the number of operations that each producer must insert into the circular buffer.
    // To avoid problems during distribution, we will use the floor function.
    int consumer_operations = floor((num_operations / num_consumers));
    
    // We define the number of operations that each producer must insert into the circular buffer.
    // To avoid problems during distribution, we will use the floor function.
    int producer_operations = floor((num_operations / num_producers));

    // We define as much threads as producers and consumers we have (array of threads).
    pthread_t consumer_threads[num_consumers];
    pthread_t producer_threads[num_producers];

    // We use malloc to reserve the dynamic memory for storing the file.
    file = malloc(sizeof(char[strlen(argv[1])]));
    file = argv[1];


    // ----- PRODUCERS -----
    // Setup initial pointer to 1.
    init = 1;
    // Structure variable to store the parameters of the thread (operations and initial position).
    struct producer_params producer_args[num_producers];
    
    // We create the threads for the PRODUCERS.
	int j;
    for (j = 0; j < (num_producers - 1); j++) {
        // Parameters of the thread.
        producer_args[j].operations = producer_operations;
        producer_args[j].initial_id = init;

        // Check for errors during thread creation.
        if (pthread_create(&producer_threads[j], NULL, (void*)producer, &producer_args[j]) < 0) {
            perror("[ERROR] Error while creating a producer thread.\n");
            return(-1);
        }

        // We put the init (pointer) in the position of the next set of operations that the next producer will insert.
        // In this case, producer_operations acts as an offset.
        init += producer_operations;
    }

    // Check how many operations have the last producer, since the last one has less operations.
    int last_producer_operations = num_operations - (j * producer_operations);
    producer_args[num_producers - 1].operations = last_producer_operations;
    producer_args[num_producers - 1].initial_id = init;

    // Create the thread for the remaining producer.
    if (pthread_create(&producer_threads[num_producers - 1], NULL, (void*)producer, &producer_args[num_producers - 1]) < 0) {
        perror("[ERROR] Error while creating the last producer thread.\n");
        return(-1);
    }

    // ------ CONSUMERS --------
    // Structure variable to store the parameters of the thread (operations and initial position).
    struct consumer_params consumer_args[num_consumers];

    // We create the threads for the CONSUMERS.
	int i;
    for (i = 0; i < (num_consumers - 1); i++ ) {
        // Parameters of the thread.
        consumer_args[i].operations= consumer_operations;

        // Check for errors during thread creation.
        if (pthread_create(&consumer_threads[i], NULL, (void*)consumer, &consumer_args[i]) < 0) {
            perror("[ERROR] Error while creating a consumer thread.\n");
            return(-1);
        }
    }

    // Check how many operations has the last consumer, since the last one has less operations (remainder of floor division).
    int last_consumer_operations = num_operations - (i * consumer_operations);
    consumer_args[num_consumers - 1].operations = last_consumer_operations;

	// Create the thread for the remaining consumer.
    if (pthread_create(&consumer_threads[num_consumers - 1], NULL, (void*)consumer, &consumer_args[num_consumers - 1]) < 0) {
        perror("[ERROR] Error while creating the last consumer thread.\n");
        return(-1);
    }

    // Wait for both producers and consumers.
    // Loop to wait (using pthread_join) for all the consumer threads.
    for (int i = 0; i < num_consumers; i++) {
        if (pthread_join(consumer_threads[i], NULL) < 0) {
            perror("[ERROR] Error while waiting for a consumer thread.\n");
            return(-1);
        }
    }

    // Loop to wait (using pthread_join) for all the producer threads.
    for (int i = 0; i < num_producers; i++) {
        if (pthread_join(producer_threads[i], NULL) < 0) {
            perror("[ERROR] Error while waiting for a producer thread.\n");
            return(-1);
        }
    }
    
    // The final total cost is printed on screen.
    printf("Total: %d euros.\n", total_cost);

    // We invoke queue_destroy to destroy the queue and free the assigned resources.
    queue_destroy(buffer);

    // We close the descriptors used (checking if there is any error).
    if (fclose(descProducer) < 0) {
        perror("[ERROR] Error while closing the descriptor.\n");
        return(-1);
    }
    
    // We destroy all the mutexes and the conditional variables (checking if there is any error).
    if (pthread_mutex_destroy(&mutex) < 0) {
        perror("[ERROR] Error while destroying the mutex.\n");
        return(-1);
    }
    if (pthread_mutex_destroy(&desc) < 0) {
        perror("[ERROR] Error while destroying the mutex.\n");
        return(-1);
    }
    if (pthread_cond_destroy(&non_empty) < 0) {
        perror("[ERROR] Error while destroying the conditional variable.\n");
        return(-1);
    }
    if (pthread_cond_destroy(&non_full) < 0) {
        perror("[ERROR] Error while destroying the conditional variable.\n");
        return(-1);
    }

    // 0 is returned at the end of the main function.
    return 0;
}



void *producer(void *arg) {
    // Gets the initial id and the number of operations for each producer.
	// Get the corresponding id and operations in the structure.
  	struct producer_params *p = arg;

	// Check for errors while locking the desc mutex.
  	if(pthread_mutex_lock(&desc) < 0) {
    	perror("[ERROR] Error while locking the mutex.\n");
    	exit(-1);
  	}
    
	// Get the descriptor of the initial line.
  	descProducer = fopen(file, "r");

    // Check for errors while opening the file.
  	if(descProducer == NULL) {
    	perror("[ERROR] Error while opening the file.\n");
    	exit(-1);
  	}

	// Now, until we arrive to the line that the producer has to start on we keep looking.
    char character;
  	int counter = 0;
    
    // Read the whole line until you find a next line jump.
  	while (counter < p -> initial_id) {
    	character = fgetc(descProducer);  
    	if (character == '\n') {
      		counter++;
    	}
  	}
    
  	// Store the current position to get it again later (everytime fgetc is executed it moves to the next posisition in the file).
  	FILE *current = descProducer;

    // Check for errors while unlocking the desc mutex.
  	if(pthread_mutex_unlock(&desc) < 0) {
    	perror("[ERROR] Error while unlocking the mutex.\n");
    	exit(-1);
  	}

	// We create these variables to store the current's line operation id, machine_type and time_of_use.
	int id, machine_type, time_of_use = 0;
    
  	for (int i = p -> operations; i > 0; i--) {
        // Check for errors while locking the mutex.
    	if(pthread_mutex_lock(&desc) < 0){
      		perror("[ERROR] Error while locking the mutex.\n");
      		exit(-1);
    	}
        
		// We restore the current line of the file (just in case it was lost).
    	descProducer = current;
        
		// Store the corresponding values in the variables created before.
    	if(fscanf(descProducer, "%d %d %d", &id, &machine_type, &time_of_use) < 0) {
      		perror("[ERROR] Error while getting the values from file.\n");
      		exit(-1);
    	}
        
		// Get again the current position (as it changes with fscanf).
    	current = descProducer;

        // Check for errors while unlocking the desc mutex.
    	if(pthread_mutex_unlock(&desc) < 0){
      		perror("[ERROR] Error while unlocking the mutex.\n");
      		exit(-1);
    	}

		// Store the read data from the line (machine_type and time_of_use) to put it in the queue (buffer).
		struct element current_data = {machine_type, time_of_use};

        // Check for errors while locking the mutex.
    	if (pthread_mutex_lock(&mutex) < 0){
      		perror("[ERROR] Error while locking the mutex.\n");
      		exit(-1);
    	}
        
		// Check if the queue is full and wait for a consumer to signal non_full.
    	while (queue_full(buffer)) {
      		if (pthread_cond_wait(&non_full, &mutex) < 0){
        		perror("[ERROR] Error while checking condition.\n");
        		exit(-1);
			}
      	}
        
		// Add the current line's data to the buffer.
		if (queue_put(buffer, &current_data) < 0) {
      		perror("[ERROR] Error while inserting data.\n");
      		exit(-1);
    	}
        
		// In case it ws empty and the consumers were waiting for it to not be empty, signal non_empty.
    	if (pthread_cond_signal(&non_empty) < 0) {
      		perror("[ERROR] Error while checking condition.\n");
      		exit(-1);
    	}

        // Check for errors while unlocking the mutex.
    	if (pthread_mutex_unlock(&mutex) < 0) {
      		perror("[ERROR] Error while unlocking the mutex.\n");
      		exit(-1);
    	}
  	}
    
	// Exit the current thread.
  	pthread_exit(0);
}



void *consumer(void *arg) {
	// Struct that contains the buffer data to be consumed.
	struct element content_read;
	
    // Struct for the operations
    struct consumer_params *consumers = arg;
    
    // Loop until operations requested have been processed.
    for (int i = 0; i < consumers -> operations; i++) {
        // We LOCK the mutex and check if there is any error.
        if (pthread_mutex_lock(&mutex) < 0) {
            perror("[ERROR] Error while locking the mutex.\n");
    	    exit(-1);
        }

        // We wait until the queue is empty.
        while (queue_empty(buffer) == 1) {
            if (pthread_cond_wait(&non_empty, &mutex) < 0) {
                perror("[ERROR] Error in the condition variable while waiting.\n");
    	        exit (-1);
            }
        }

        // Structure variable to store the line read from the queue.
        struct element *content_read = queue_get(buffer);

        // If no data is found, an error is raised.
        if (content_read == NULL) {
            perror("[ERROR] Data not found.\n");
    	    exit(-1);
        }

        // Switch to see what to do depending on the type of machine of the element of the queue.
        switch (content_read -> machine_type) {
            //The type of machine is: common_node (cost 3€/minute).
            case 1:
                total_cost += 3 * content_read -> time_of_use;
                break;
            //The type of machine is: computation_node (cost 6€/minute).
            case 2:
                total_cost += 6 * content_read -> time_of_use;
                break;
            //The type of machine is: super_computer (cost 15€/minute).
            case 3:
                total_cost += 15 * content_read -> time_of_use;
                break;
            // If the type of machine is not 1, 2 or 3, we consider the value as invalid.
            default:
                perror("[ERROR] Invalid type of machine.\n");
    	        exit(-1);
        }
		// In case any producer was waiting to produce, we signal that it is not full.
        if (pthread_cond_signal(&non_full) < 0) {
            perror("[ERROR] Error in the condition variable while signal.\n");
    	    exit(-1);
        }

        // We UNLOCK the mutex and check if there is any error.
        if (pthread_mutex_unlock(&mutex) < 0) {
            perror("[ERROR] Error while unlocking the mutex.\n");
    	    exit(-1);
        }
    }
    pthread_exit(0);
}
