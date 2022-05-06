
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

#define NUM_CONSUMERS 1

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
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
int total = 0; //Total cost

struct param {
  int id_ini;
  int op;
};

// Cada hilo tiene acceso a su propio dominio de id`s (numero de filas a realizar), donde el nº de operaciones que tendrá asociado será
// floor(num.lineas/num.productores), leerá e insertará de uno en uno
void *producir(void *arg) {
  // Sacar parametros
  struct param *p = arg;

  // Hallamos el descriptor que corresponde al primer indice, para que no se mezclen bloqueamos y desbloqueamos
  if(pthread_mutex_lock(&des) < 0){
    perror("Error de mutex");
    exit(-1);
  }

  descriptorP = fopen(fichero, "r");
  if( descriptorP == NULL){
    perror("Error al abrir fichero");
    exit(-1);
  }
  // Aqui lo que hace es ir linea por linea
  int counter = 0;
  char chr;
  while (counter < p->id_ini) {
    chr = fgetc(descriptorP);
    if (chr == '\n') {
      counter++;
    }
  }
  // Almacenamos la posicion por la que va, de esta manera la podemos recuperar.
  FILE *current = descriptorP;


  if(pthread_mutex_unlock(&des) < 0){
    perror("Error de mutex");
    exit(-1);
  }

  int i, i1, i2 = 0;
  for (int j = p->op; j > 0; j--) {

    // Sacamos los valores a introducir en el buffer del puntero, y salvamos la nueva posicion del mismo.
    if(pthread_mutex_lock(&des) < 0){
      perror("Error de mutex");
      exit(-1);
    }

    descriptorP = current;
    if(fscanf(descriptorP, "%d %d %d", &i, &i1, &i2) < 0){
      perror("Error al extraer datos archivo");
      exit(-1);
    }
    current = descriptorP;


    if(pthread_mutex_unlock(&des) < 0){
      perror("Error de mutex");
      exit(-1);
    }

    struct element temporal = {i1, i2}; //type, time

    if(pthread_mutex_lock(&elements) < 0){
      perror("Error de mutex");
      exit(-1);
    }
    while (queue_full(q))
      if(pthread_cond_wait(&lleno, &elements) < 0){
        perror("Error de variable de condicion");
        exit(-1);
      }


    if(queue_put(q, &temporal) < 0){
      perror("Error al insertar");
      exit(-1);
    }
    if(pthread_cond_signal(&vacio) < 0){
      perror("Error de variable de condicion");
      exit(-1);
    }
    if(pthread_mutex_unlock(&elements) < 0){
      perror("Error de mutex");
      exit(-1);
    }
  }
  pthread_exit(0);
}

void *consumir(int *numValores) {
  //The consumer gets the data from the queue and then calculates the total cost
  struct element data;
  // Bucle de todo mientras no se hallan leido todas las ops esperadas.
  for (int k = 0; k < *numValores; k++) {

    if(pthread_mutex_lock(&elements) < 0){
      perror("Error de mutex");
      exit(-1);
    }
    //To wait while the queue does not have any element
    while (queue_empty(q))
      if(pthread_cond_wait(&vacio, &elements) < 0){
        perror("Error de variable de condicion");
        exit(-1);
      }

    struct element *data = queue_get(q);
    if(data==NULL){
      perror("Error al extraer");
      exit(-1);
    }

    switch (data->type) {
    case 1:
      total += 1 * data->time;

      break;
    case 2:
      total += 3 * data->time;

      break;
    case 3:
      total += 10 * data->time;

      break;
    default:
      perror("Valor no valido");
    }
    if(pthread_cond_signal(&lleno) < 0){
      perror("Error de variable de condicion");
      exit(-1);
    }
    if(pthread_mutex_unlock(&elements) < 0){
      perror("Error de mutex");
      exit(-1);
    }
  }
  pthread_exit(0);
}
// CALCULA LAS LINEAS QUE HAY EN EL ARCHIVO INTRODUCIDO
int calculo_lineas(const char filename[]) {
  FILE *file = fopen(filename, "r");
  if(file == NULL){
    perror("Error al abrir fichero");
    exit(-1);
  }
  char chr;
  int lines = 0;
  while (!feof(file)) {
    chr = fgetc(file);
    if (chr == '\n') {
      lines++;
    }
  }
  fclose(file);
  return lines;
}

int main(int argc, const char *argv[]) {
  //Control de errores de los datos iniciales
  if (argc > 4) {
    perror("Numero de argumentos invalido");
    return -1;
  }

  FILE *descriptor = fopen(argv[1], "r");
  if(descriptor == NULL){
    perror("Error al abrir fichero");
    exit(-1);
  }
  // NUMVAL TIENE EL NUMERO DE OPERACIONES QUE HAY QUE HACER
  int numVal;
  if(fscanf(descriptor, "%d", &numVal) < 0){
    perror("Error al extraer datos archivo");
    exit(-1);
  }
  // ESTO COMPRUEBA SI LAS OPERACIONES QUE PONEN EN LA PRIMERA LINEA (500) SON IGUALES AL NUMERO DE LINEAS QUE HAY (OPERACIONES)
  int numLin = calculo_lineas(argv[1]);
  if (numVal > (numLin - 1)) {
    perror("Error: Se indica un numero de operaciones incorrecto");
    return -1;
  }
// COMPROBACION DE PRODUCTORES
  int productores = atoi(argv[2]);
  if (productores <= 0) {
    perror("Error: Numero invalido de productores.");
    return -1;
  }
  //SIZE DEL BUFFER ES NUESTRO ARGV 4
  int size = atoi(argv[3]);
  if (size <= 0) {
    perror("Error: Tamaño invalido.");
    return -1;
  }

//INICIALIZAMOS LA QUEUE CON EL SIZE DADO
  q = queue_init(size);
  //PARA INICIAR LOS MUTEXES DE ESCRITURA DE LA QUEUE
  if(pthread_mutex_init(&elements, NULL) < 0){ // RING ES ELEMENTS
    perror("Error inicializar variable de condicion");
    exit(-1);
  }
  if(pthread_mutex_init(&des, NULL) < 0){
    perror("Error inicializar variable de condicion");
    exit(-1);
  }
  if(pthread_cond_init(&lleno, NULL) < 0){
    perror("Error inicializar mutex");
    exit(-1);
  }
  if(pthread_cond_init(&vacio, NULL) < 0){
    perror("Error inicializar mutex");
    exit(-1);
  }
  //Creamos los hilos y establecemos el nº de operaciones que hará cada uno
  int operaciones = floor((numVal / productores));  // DARLE EL NUMEOR DE OPERACIONES IGUAL A CADA PRODUCTOR
  int id_inicio = 1;
  pthread_t hilosP[productores];  // CREAR TANTOS HILOS COMO PRODUCTORES HAYA, HACER TAMBIEN CON LOS CONSUMIDORES
  //En este solo hay un consumidor, CAMBIAR A VARIOS CONSUMIDORES
  pthread_t hiloC;
  if(pthread_create(&hiloC, NULL, (void *)consumir, &numVal) < 0){  // NO SE POR QUE SE PONEN EL NUMVAL AL FINAL
    perror("Error al crear hilo");
    exit(-1);
  }
  //Ejecución de las operaciones
  int i;
  fichero = malloc(sizeof(char[strlen(argv[1])]));  //PRIMERO ALOCAMOS EL SIZE DEL FICHERO Y LUEGO LO PASAMOS
  fichero = argv[1];
  struct param args[productores];
  for (i = 0; i < (productores - 1); i++) { //LOOP PARA CREAR TANTOS HILOS COMO PRODUCTORES
    args[i].op = operaciones;
    args[i].id_ini = id_inicio;

    if(pthread_create(&hilosP[i], NULL, (void *)producir, &args[i]) < 0){
      perror("Error al crear hilo");
      exit(-1);
    }

    id_inicio += operaciones;
  }
  int op_ultimo = numVal - (i * operaciones);
  args[productores - 1].op = op_ultimo;
  args[productores - 1].id_ini = id_inicio;

  //Control de errores de operaciones y valores finales
  if(pthread_create(&hilosP[productores - 1], NULL, (void *)producir, &args[productores - 1]) < 0){
    perror("Error al crear hilo");
    exit(-1);
  }

  for (int i = 0; i < productores; i++) {
    if(pthread_join(hilosP[i], NULL) < 0){
      perror("Error al esperar al hilo");
      exit(-1);
    }
  }
  if(pthread_join(hiloC, NULL) < 0){
    perror("Error al esperar al hilo");
    exit(-1);
  }

  printf("Total: %i €.\n", total);
  queue_destroy(q);
  if(pthread_mutex_destroy(&des) < 0){
    perror("Error al destruir mutex");
    exit(-1);
  }
  if(pthread_mutex_destroy(&elements) < 0){
    perror("Error al destruir mutex");
    exit(-1);
  }
  if(  pthread_cond_destroy(&lleno) < 0){
    perror("Error al destruir variable condicion");
    exit(-1);
  }
  if(pthread_cond_destroy(&vacio) < 0){
    perror("Error al destruir variable condicion");
    exit(-1);
  }
  if(fclose(descriptorP) < 0){
    perror("Error al cerrar descriptor");
    exit(-1);
  }
  if(fclose(descriptor) < 0){
    perror("Error al cerrar descriptor");
    exit(-1);
  }
  return 0;
}