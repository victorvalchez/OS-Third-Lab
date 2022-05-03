#include "queue.h"
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    FILE *descriptor = fopen("file.txt", "r");
  if(descriptor == NULL){
    perror("Error al abrir fichero");
    exit(-1);
  }

  int numVal;
  if(fscanf(descriptor, "%d", &numVal) < 0){
    perror("Error al extraer datos archivo");
    exit(-1);
  }
  printf("%d", numVal);
}