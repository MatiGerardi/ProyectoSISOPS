#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CLIENTES 50
#define MAX_COLA = 20 //capacidad maxima para cada cola
#define HAM_SIMPLE = 1
#define VEGAN = 2
#define FRTAS = 3
#define NO_PEDIDO = 0 //Representa que no quiere un pedido de algun tipo

pthread_t clientes[NUM_CLIENTES];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int pipe_fd[2];

void prepare_burger() {
    printf("Empleado preparando hamburguesa...\n");
    sleep(2); // Simula el tiempo de preparación
}

void prepare_vegan() {
    printf("Empleado preparando menú vegano...\n");
    sleep(3); // Simula el tiempo de preparación
}

void prepare_fries() {
    printf("Empleado preparando papas fritas...\n");
    sleep(1); // Simula el tiempo de preparación
}

void* fun_cliente(void* arg) {
    int pedido[4];

    // Cliente espera en la cola
    pthread_mutex_lock(&mutex);
    printf("Cliente %d esperando en la cola...\n", id);
    pthread_mutex_unlock(&mutex);

    // Espera a ser atendido
    read(pipe_fd[0], buffer, sizeof(buffer));
    printf("Cliente %d atendido: %s\n", id, buffer);
    //Decide el pedido
    for(int i = 0; i < 3; i++){
        pedido[i] = rand() % 4; //El random arranca en 0
    }

    // Enviar mensaje a través del pipe
    write(pipe_fd[1], pedido, 1);
    mutex_lock(&mutex);
    printf("Cliente %d está esperando\n", id);

    // Esperar a ser atendido
    read(pipe_fd[0], &msg, 1);
    printf("Cliente %d ha sido atendido\n", id);

    // Cliente desocupa la cola
    pthread_mutex_lock(&mutex);
    printf("Cliente %d desocupando la cola...\n", id);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void atender_cliente(int id) {
    char mensaje[256];
    sprintf(mensaje, "Pedido del cliente %d listo", id);
    write(pipe_fd[1], mensaje, sizeof(mensaje));
}

void* recepcion(void* arg) {
    char msg;
    for (int i = 0; i < NUM_CLIENTES; i++) {
        // Leer mensaje del pipe
        read(pipe_fd[0], &msg, 1);
        printf("Recepción atendiendo a un cliente...\n");

        // Distribucion de tareas
        for(int i = 0; i < 3; i++){
            if(pedido[i] == HAM_SIMPLE){
                prepare_burger();
            }else if(pedido[i] == VEGAN){
                prepare_vegan();
            }else if(pedido[i] == FRTAS){
                prepare_fries();
            }

         } 
        

        // Enviar mensaje de liberación al cliente
        write(pipe_fd[1], &msg, 1);
    }
    return NULL;
}

int main() {
    pid_t pid_hamburguesas, pid_vegan, pid_fritas, pid_recepcion;
    pthread_t clientes[NUM_CLIENTES];
    int ids[NUM_CLIENTES];

    // Crear pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Crear hilos de clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        ids[i] = i + 1;
        pthread_create(&(clientes[i]), NULL, fun_cliente, &ids[i]);
    }

    // Crear proceso de recepción
    if ((pid_recepcion = fork()) == 0) {
        pthread_t hilo_recepcion;
        pthread_create(&hilo_recepcion, NULL, recepcion, NULL);
        pthread_join(hilo_recepcion, NULL);
        exit(0);
    }

    // Crear proceso para hamburguesas
    if ((pid_hamburguesas = fork()) == 0) {
        pthread_t hilo_hamburguesas;
        pthread_create(&hilo_hamburguesas, NULL, preparar_hamburguesas, NULL);
        pthread_join(hilo_hamburguesas, NULL);
        exit(0);
    }

    // Crear proceso para menú vegano
    if ((pid_vegan = fork()) == 0) {
        pthread_t hilo_vegan;
        pthread_create(&hilo_vegan, NULL, preparar_vegan, NULL);
        pthread_join(hilo_vegan, NULL);
        exit(0);
    }

    // Crear proceso para papas fritas
    if ((pid_fritas = fork()) == 0) {
        pthread_t hilo_fritas1, hilo_fritas2;
        pthread_create(&hilo_fritas1, NULL, preparar_papas, NULL);
        pthread_create(&hilo_fritas2, NULL, preparar_papas, NULL);
        pthread_join(hilo_fritas1, NULL);
        pthread_join(hilo_fritas2, NULL);
        exit(0);
    }

    // Esperar a que todos los clientes terminen
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pthread_join(clientes[i], NULL);
    }

    // Cerrar el pipe
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    return 0;
}