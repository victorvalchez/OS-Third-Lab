
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
int total_cost = 0;

// Mutex to access shared buffer.
pthread_mutex_t mutex;
pthread_mutex_t desc;

// Synchronization variable -> Can we add more elements?
pthread_cond_t non_full;
// Synchronization variable -> Can we remove elements?
pthread_cond_t non_empty;


// To get the descriptor of the file being processed
FILE *descProducer;  
const char *file; // To pass the file being processed argv[1] to this character

// Structure which is the buffer (queue) defined in queue.c.
struct queue *buffer;

// This structure contains the number of operations and the starting line for each producer and consumer
struct params{
  int operations;
  int init;
};

struct producers_params {
  int initial_id;
  int operations;
};

void *producer(void *arg){ //Get the initial id and the number of operations for each producer
	// Get the corresponding id and operations in the structure
  	struct producers_params *p = arg;

	//Get the descriptor of the initial line
  	if(pthread_mutex_lock(&desc) < 0){
    	perror("[ERROR] Error while locking the mutex.");
    	exit(-1);
  	}
	//Get descriptor
  	descProducer = fopen(file, "r");
  	if(descProducer == NULL){
    	perror("[ERROR] Error while opening the file.");
    	exit(-1);
  	}

	// Now, until we arrive to the line that the producer has to start on we keep looking
  	int counter = 0;
  	char character;
  	while (counter < p->initial_id) {
    	character = fgetc(descProducer);  //Read the whole line until you find a next line jump
    	if (character == '\n') {
      		counter++;
    	}
  	}
  	// Store the current position to get it again later (everytime fgetc is executed it moves to the next posisition in the file)
  	FILE *current = descProducer;

  	if(pthread_mutex_unlock(&desc) < 0){
    	perror("[ERROR] Error while unlocking the mutex.");
    	exit(-1);
  	}

	//We create these variables to store the current's line operation id, type and machine
	int id, type, time = 0;
  	for (int i = p->operations; i > 0; i--) {
    	// Get the values from the line and store the new position in the file
    	if(pthread_mutex_lock(&desc) < 0){
      		perror("[ERROR] Error while locking the mutex.");
      		exit(-1);
    	}
		// We restore the current line of the file (just in case it was lost)
    	descProducer = current;
		//Store the corresponding values in the variables created before
    	if(fscanf(descProducer, "%d %d %d", &id, &type, &time) < 0){
      		perror("[ERROR] Error while getting the values from file.");
      		exit(-1);
    	}
		//Get again the current position (as it changes with fscanf)
    	current = descProducer;
	
    	if(pthread_mutex_unlock(&desc) < 0){
      		perror("[ERROR] Error while unlocking the mutex.");
      		exit(-1);
    	}

		//Store the read data from the line to put it in the queue (buffer)
		struct element current_data = {type, time}; //type, time

    	if(pthread_mutex_lock(&mutex) < 0){
      		perror("[ERROR] Error while locking the mutex.");
      		exit(-1);
    	}
		//Check if the queue is full and wait for a consumer to signal non_full
    	while (queue_full(buffer)){
      		if(pthread_cond_wait(&non_full, &mutex) < 0){
        		perror("[ERROR] Error while checking condition.");
        		exit(-1);
			}
      	}
		//Add the current line's data to the buffer
		if(queue_put(buffer, &current_data) < 0){
      		perror("[ERROR] Error while inserting data.");
      		exit(-1);
    	}
		// In case it ws empty and the consumers were waiting for it to not be empty, signal non_empty
    	if(pthread_cond_signal(&non_empty) < 0){
      		perror("[ERROR] Error while checking condition.");
      		exit(-1);
    	}
    	if(pthread_mutex_unlock(&mutex) < 0){
      		perror("[ERROR] Error while unlocking the mutex.");
      		exit(-1);
    	}
  	}
	// Exit the current thread
  	pthread_exit(0);
}
	

/*
void *producer(struct params*argv) {
    
    · PRODUCER THREAD:
    - Obtain data extracted from the file.
    - Insert data one by one in the circular buffer.
    

    // We create a new element that will be enqueued in the circular buffer.
	struct element new_element;  //This corresponds to a structure having the machine type and the time of use

    
	for (int i = argv -> op1; i < argv -> op2; i++) {

        // We assign the machine type and the time of use to the new_element structure.
		new_element -> machine_type = op1array[i];
		new_element -> time_of_use = op2array[i];

        // We LOCK the mutex and check if there is any error.
		if (pthread_mutex_lock(&mutex) < 0){
			perror("[ERROR] Error while locking the mutex.");
    	    return(-1);
		}
        
        // We try to access the queue but it blocks the running thread if it is full.
		while (queue_full(buffer) == 1){
			if (pthread_cond_wait(&non_full, &mutex) < 0) {
        			perror("[ERROR] Error in conditional variable non_full while waiting.");
    	            return(-1);
      			}
		}

        // We enqueue the new element created with the machine type and the time of use and check if there is any error.
		if (queue_put(buffer, &new_element) < 0) {
      			perror("[ERROR] Error while enqueue.");
    	        return(-1);
    		}

        // We unlock one or more threads suspended in the condition variable non_empty.
		if (pthread_cond_signal(&non_empty) < 0) {
			perror("[ERROR] Error in conditional variable non_empty while signal.");
    	    return(-1);
		}
        
        // We UNLOCK the mutex and check if there is any error.
		if (pthread_mutex_unlock(&mutex) < 0) {
			perror("[ERROR] Error while unlocking the mutex.");
    	    return(-1);		
		}
		
	}
	pthread_exit(0);
}
*/


/*
void *producer(void * param) {

    · PRODUCER THREAD:
    - Obtain data extracted from the file.
    - Insert data one by one in the circular buffer.




    // (CODE HERE)

    

    // We LOCK the mutex desc and check if there is any error.
    if (pthread_mutex_lock(&desc) < 0) {
        perror("[ERROR] Error while locking the mutex.");
    	return -1;
    }

    // !!!! CAMBIAR NOMBRE A descProducer CUANDO SE SEPA LO QUE ES !!!!
    descProducer = fopen(file, "r");
    if (descProducer == NULL) {
        perror("[ERROR] Error while opening the file.");
    	return -1;
    }



    // (CODE HERE)




    // We UNLOCK the mutex desc and check if there is any error.
    if (pthread_mutex_unlock(&desc) < 0) {
        perror("[ERROR] Error while unlocking the mutex.");
    	return(-1);
    }



    // (INTRODUCE THINGS IN THE CIRCULAR BUFFER)
    

    pthread_exit(0);
    // Line that should not be executed if pthread_exit(0) works as expected.
    return NULL;
}
*/


// Num_operations is the number of operations each consumer has to do
int *consumer(int *num_operations) {
     /*
    · CONSUMER THREAD:
    - Obtain (concurrently) the elements inserted in the queue.
    - Each extracted element represents a type of machine and the time of use.
    - The consumer must calculate the cost and accumulate it until all elements have been processed.
    - Return to the main thread the partial cost calculated by each one.
    */
	// Struct that contains the buffer data to be consumed
	struct element content_read;
	
    /* // Integer variable to store the partial cost (cost of current line being consumed) that will be returned.
    int partial_cost; */
    
    // Loop until operations requested have been processed.
    for (int i = 0; i < *num_operations; i++) {
        // We LOCK the mutex and check if there is any error.
        if (pthread_mutex_lock(&mutex) < 0) {
            perror("[ERROR] Error while locking the mutex.");
    	    return(-1);
        }

        // !!! ESTO CREO QUE NO SERÍA ASÍ PARA N CONSUMIDORES (creo que si porque en los productores es igual)
        // We wait until the queue is empty.
        while (queue_empty(buffer) == 1) {
            if (pthread_cond_wait(&non_empty, &mutex) < 0) {
                perror("[ERROR] Error in the condition variable while waiting.");
    	        return (-1);
            }
        }

        // Structure variable to store the line read from the queue.
        struct element *content_read = queue_get(buffer);

        if (content_read == NULL) {
            perror("[ERROR] Data not found.");
    	    return (-1);
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
                perror("[ERROR] Invalid type of machine.");
    	        return (-1);
        }
		// In case any producer was waiting to produce, we signal that it is not full
        if (pthread_cond_signal(&non_full) < 0) {
            perror("[ERROR] Error in the condition variable while signal.");
    	    return (-1);
        }

        // We UNLOCK the mutex and check if there is any error.
        if (pthread_mutex_unlock(&mutex) < 0) {
            perror("[ERROR] Error while unlocking the mutex.");
    	    return(-1);
        }
    }
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

    // We check if the number of arguments is correct. (5 because the argv[0] is the name of the file)
	if (argc != 5) {
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
    char character;
    // End of file (feof) is used to check whether we have checked the entire file to count the lines.
    while(!feof(descriptor)) {
        // Read a character.
        character = fgetc(descriptor);
        // If the character is a line break, the counter (num_lines) is incremented.
        if (character == '\n') {
            num_lines++;
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

    // We create the buffer which is a queue in this case, with the size introduced.
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



    // ------ CONSUMERS --------
    // Setup initial pointer to 1 (as the line 0.
    init = 1;
    // Structure variable to store the parameters of the thread (operations and initial position).
    struct params consumer_args[num_consumers];

    // We create the threads for the CONSUMERS.
	int i;
    for (i = 0; i < (num_producers - 1); i++ ) {
        // Parameters of the thread.
        consumer_args[i].operations= consumer_operations;
        consumer_args[i].init = init;

        if (pthread_create(&consumer_threads[i], NULL, (void*)consumer, &consumer_args[i]) < 0) {
            perror("[ERROR] Error while creating a consumer thread.");
            return(-1);
        }

        // We put the init (pointer) in the position of the next set of operations that the next producer will insert.
        // In this case, consumer_operations acts as an offset.
        init += consumer_operations;
    }

    // Check how many operations has the last consumer, since the last one has less operations (remainder of floor division).
    int last_consumer_operations = num_operations - (i * consumer_operations);
    consumer_args[num_consumers - 1].operations = last_consumer_operations;
    consumer_args[num_consumers - 1].init = init;

	// Create the thread for the remaining consumer
    if (pthread_create(&consumer_threads[num_consumers - 1], NULL, (void*)consumer, &consumer_args[num_consumers - 1]) < 0) {
        perror("[ERROR] Error while creating the last consumer thread.");
        return(-1);



    // ----- PRODUCERS -----
    // Setup initial pointer to 1.
    init = 1;
    // Structure variable to store the parameters of the thread (operations and initial position).
    struct params
	producer_args[num_producers];
    
    // We create the threads for the PRODUCERS.
	int i;
    for (i = 0; i < (num_producers - 1); i++) {
        // Parameters of the thread.
        producer_args[i].operations = producer_operations;
        producer_args[i].init = init;

        if (pthread_create(&producer_threads[i], NULL, (void*)producer, &producer_args[i]) < 0) {
            perror("[ERROR] Error while creating a producer thread.");
            return(-1);
        }

        // We put the init (pointer) in the position of the next set of operations that the next producer will insert.
        // In this case, producer_operations acts as an offset.
        init += producer_operations;
    }

    // Check how many operations have the last producer, since the last one has less operations.
    int last_producer_operations = num_operations - (i * producer_operations);
    producer_args[num_producers - 1].operations = last_producer_operations;
    producer_args[num_producers - 1].init = init;

    if (pthread_create(&producer_threads[num_producers - 1], NULL, (void*)producer, &producer_args[num_producers - 1]) < 0) {
        perror("[ERROR] Error while creating the last producer thread.");
        return(-1);
    }

    // Loop to wait (using pthread_join) for all the consumer threads.
    for (int i = 0; i < num_consumers; i++) {
        if (pthread_join(consumer_threads[i], NULL) < 0) {
            perror("[ERROR] Error while waiting for a consumer thread.");
            return(-1);
        }
    }

    // Loop to wait (using pthread_join) for all the producer threads.
    for (int i = 0; i < num_producers; i++) {
        if (pthread_join(producer_threads[i], NULL) < 0) {
            perror("[ERROR] Error while waiting for a producer thread.");
            return(-1);
        }
    }
    
    // The final total cost is printed on screen.
    printf("Total: %d euros.\n", total_cost);

    // We invoke queue_destroy to destroy the queue and free the assigned resources.
    queue_destroy(buffer);

    // We close the descriptors used (checking if there is any error).
    if (fclose(descProducer) < 0) {
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
