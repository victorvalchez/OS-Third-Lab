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


/*
PRODUCER THREAD:
- Obtain data extracted from the file.
- Insert data one by one in the circular buffer.
*/


/*
CONSUMER THREAD:
- Obtain (concurrently) the elements inserted in the queue.
- Each extracted element represents a type of machine and the time of use.
- The consumer must calculate the cost and accumulate it until all elements have been processed.
- Return to the main thread the partial cost calculated by each one.
*/


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

    return 0;
}
