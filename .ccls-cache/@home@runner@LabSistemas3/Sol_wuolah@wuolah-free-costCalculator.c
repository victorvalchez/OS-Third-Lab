
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

#define NUM_CONSUMERS 1

//gcc -Wall -g -o costCalculator costCalculator.c
//gcc -Wall -g -o queue queue.c

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */

pthread_mutex_t mutex; 	  // mutex de acceso al buffer compartido 
pthread_cond_t no_lleno;   // controla el llenado del buffer 
pthread_cond_t no_vacio;   // controla el vaciado del buffer 
struct queue *circbuffer;
const char *pathfile;
int *op1array;
int *op2array;
//int total = 0; devolver en hilo
struct opers {
  int op1;
  int op2;
};


void *producerf(struct opers *argv){
	//seguimos el proceso enseñado en clase
	struct element inserts;
	for (int i=argv->op1; i<argv->op2; i++){
		//insert into queue if allowed
		inserts.type=op1array[i];
		inserts.time=op2array[i];
		if (pthread_mutex_lock(&mutex)<0){
			perror("Error lock mutex");
			exit(-1); 
		}   	
		while (queue_full(circbuffer)==-1){  	// acceder al buffer, si buffer lleno, se bloquea
			if(pthread_cond_wait(&no_lleno, &mutex) < 0){
        			perror("Error cond wait no lleno");
        			exit(-1);
      			}
		}
		if(queue_put(circbuffer, &inserts) < 0){
      			perror("Error queue put");
     	 		exit(-1);
    		}
		if (pthread_cond_signal(&no_vacio)<0){ //buffer no vacio 
			perror("Error cond signal no vacio");
        		exit(-1);		
		}
		if (pthread_mutex_unlock(&mutex)<0){
			perror("Error unlock mutex");
        		exit(-1);		
		}//desbloqueamos mutex
		
	}
	pthread_exit(0);
}
void *consumerf(int *opsnum){
	int *total;
	total=malloc(sizeof(int));
	//seguimos el proceso enseñado en clase
	struct element consops;
	for (int i=0; i<*opsnum; i++){
		if (pthread_mutex_lock(&mutex)<0){
			perror("Error lock mutex");
			exit(-1); 
		}  
		//acceder al buffer, si buffer vacio, se bloquea       
		while (queue_empty(circbuffer)==-1){      
			pthread_cond_wait(&no_vacio, &mutex); 
		}
		// we use the -> format cause we have a pointer
		struct element *consops = queue_get(circbuffer);
		if (consops->time <0){
			printf("Error in time format\n");
			//if negative number we change it to positive
		}
		else if (consops->type==1){
			*total=*total+((consops->time)*1);
		}
		else if (consops->type==2){
			*total=*total+((consops->time)*3);
		}
		else if (consops->type==3){
			*total=*total+((consops->time)*10);
		}
		else{
			printf("Error in type format\n");
			//we dont exit cause we need to keep processing data
			
		}
		if (pthread_cond_signal(&no_lleno)<0){ // buffer no lleno 
			perror("Error cond signal no lleno ");
        		exit(-1);		
		}
		if (pthread_mutex_unlock(&mutex)<0){
			perror("Error unlock mutex");
        		exit(-1);		
		}//desbloqueamos mutex		
	}
	pthread_exit((void *)total);
}
int main (int argc, const char * argv[] ) {
	//we define the ints/char to be used
	int ops, lines=0;
	int producers, size;
	int prodnum, cont, index;
	char segment;
	pathfile=argv[1];
	int d0,d1,d2;
	//first we need to check that values given are valid
	if (argc != 4) {
    		perror("Error: Invalid number of arguments");
    	return -1;
  	}
	//in order to use fscanf we need to use fopen instead of open
	//it is also faster using fopen and easier
	FILE *desc = fopen(argv[1], "r"); //'r' mode is equivalent to read only mode in open
	//FILE type is needed to use fopen
  	if(desc == NULL){
    		perror("Error fopen");
    		return -1;
  	}
	//we take the first number of the file to check validity
  	if(fscanf(desc, "%d", &ops) < 0){
    		printf("Error fscanf\n");
    		return -1;
  	}
	//ops must be a valid number, we check it now
	//we cannot use fscanf 
  	while (!feof(desc)) {
    		segment = fgetc(desc);
    		if (segment == '\n') {
			//when \n found means a line was read
      			lines=lines+1;
    		}
  	}
	//number of ops must be > than lines -1 in the file
  	if ((lines - 1) < ops) {
   	 	printf("Error: Ops number incorrect\n");
    		return -1;
  	}
	//we get the size and nº of producers for further use 
  	if ((producers = atoi(argv[2])) <= 0) {
    		printf("Invalid producers amount\n");
    		return -1;
  	}
	
	//obtain the size value for the buffer(queue)
  	if ((size = atoi(argv[3])) <= 0) {
    		printf("Invalid size\n");
    		return -1;
  	}

	rewind(desc);
	if(fscanf(desc, "%d", &ops) < 0){
    		perror("Error fscanf");
    		return -1;
  	}//we rewind and reread the first number so we can now insert values into the global vector
	op1array=(int *)malloc(ops*sizeof(int)); //prepare memory for the global vectors to be used
	op2array=(int *)malloc(ops*sizeof(int));
	// we get the values for storage in the global arrays
	for (int i=0; i<ops; i++) {
		if(fscanf(desc, "%d %d %d", &d0, &d1, &d2) < 0){
      			perror("Error al extraer datos archivo");
      			return -1;
    		}
		op1array[i]=d1;
		op2array[i]=d2;
		//printf("%d,%d\n",op1array[i],op2array[i]);
	}//store in global int array with 2*size cells the numbers needed
	if (fclose(desc)<0){
		perror("Error closing desc");
		return -1;
	}

	//init of the mutex and conditions to be used
	if (pthread_mutex_init(&mutex, NULL)<0){
		perror("init mutex error");
    		return -1;
	}
	if (pthread_cond_init(&no_lleno, NULL)<0){
		perror("init cond error");
    		return -1;
	}
	if (pthread_cond_init(&no_vacio, NULL)<0){
		perror("init cond error");
    		return -1;
	}

	//we distribute first the work for all the producers
	
  	pthread_t cons;
	circbuffer = queue_init(size);
  	if(pthread_create(&cons, NULL, (void *)consumerf, &ops) < 0){
    		perror("Error al crear hilo");
    		return -1;
  	}
	struct opers arrai[producers];
	pthread_t prod[producers];
	//no need of using floor cause int divisions result in ints wich equals to floor func
	prodnum=ops/producers;
	cont=ops-(prodnum*producers);
	index=0;
	//use two global array to store the information needed
	for (int i=0; i<producers; i++){
		
		if (cont==0){
			arrai[i].op1=index;
			arrai[i].op2=index+prodnum;
			index=index+prodnum;
		}
		else{//to redistribute the cont values we use a simple algorithm
			arrai[i].op1=index;
			arrai[i].op2=index+prodnum+1;
			cont=cont-1;
			index=index+prodnum+1;
		}
		if(pthread_create(&prod[i], NULL, (void *)producerf, &arrai[i]) < 0){
      			perror("Error creating thread");
      			return -1;
		}
	}
	int *total_main;
	total_main=malloc(sizeof(int));
	//we wait for all the threads to finish 
  	if(pthread_join(cons, (void **)&total_main) < 0){
    		perror("Error waiting for consumer thread");
    		exit(-1);
  	}
	total_main=(int *)total_main;
	for (int i = 0; i < producers; i++) {
    		if(pthread_join(prod[i], NULL) < 0){
      		perror("Error waiting for producer thread");
      		return -1;
    		}
  	}
	//destruction of the mutex and conditions used
	if (pthread_mutex_destroy(&mutex)<0){
		perror("destroy mutex error");	
    		return -1;
	}
	if (pthread_cond_destroy(&no_lleno)<0){
		perror("destroy cond error");	
    		return -1;
	}
	if (pthread_cond_destroy(&no_vacio)<0){
		perror("destroy cond error");	
    		return -1;
	}
	//destruction of the queue used (serves as a free)
	queue_destroy(circbuffer);
	free(op1array);
	free(op2array);
	//we free the two global arrays that were used
	printf("Total: %i €.\n", *total_main);
    	return 0;
}




















