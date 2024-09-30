#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_EMPLOYEES 4

sem_t sem_burger, sem_vegan, sem_fries;
pthread_mutex_t mutex_order = PTHREAD_MUTEX_INITIALIZER;

void prepare_burger() {
    // Código para preparar hamburguesa
    printf("Preparando hamburguesa...\n");
    sleep(1); // Simula el tiempo de preparación
}

void prepare_vegan() {
    // Código para preparar menú vegano
    printf("Preparando menú vegano...\n");
    sleep(1); // Simula el tiempo de preparación
}

void prepare_fries() {
    // Código para preparar papas fritas
    printf("Preparando papas fritas...\n");
    sleep(1); // Simula el tiempo de preparación
}

void receive_order(int pipe_fd) {
    char buffer[256];
    read(pipe_fd, buffer, sizeof(buffer));
    printf("Pedido recibido: %s\n", buffer);
}

void* employee_thread(void* arg) {
    int id = *(int*)arg;
    if (id == 0) {
        sem_wait(&sem_burger);
        prepare_burger();
        sem_post(&sem_burger);
    } else if (id == 1) {
        sem_wait(&sem_vegan);
        prepare_vegan();
        sem_post(&sem_vegan);
    } else {
        sem_wait(&sem_fries);
        prepare_fries();
        sem_post(&sem_fries);
    }
    return NULL;
}

int main() {
    int pipe_fd[2];
    pid_t pids[NUM_EMPLOYEES];
    pthread_t threads[NUM_EMPLOYEES];
    int ids[NUM_EMPLOYEES] = {0, 1, 2, 3};

    sem_init(&sem_burger, 0, 1);
    sem_init(&sem_vegan, 0, 1);
    sem_init(&sem_fries, 0, 2); // Dos empleados para papas fritas

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        if ((pids[i] = fork()) == 0) {
            // Código del hijo
            close(pipe_fd[1]); // Cierra el extremo de escritura del pipe PADRE
            pthread_create(&threads[i], NULL, employee_thread, &ids[i]);
            pthread_join(threads[i], NULL);
            receive_order(pipe_fd[0]);
            close(pipe_fd[0]); // Cierra el extremo de lectura del pipe HIJO
            exit(EXIT_SUCCESS);
        }
    }

    // Código del padre
    close(pipe_fd[0]); // Cierra el extremo de lectura del pipe HIJO
    char *order = "Pedido de cliente VIP";
    write(pipe_fd[1], order, sizeof(order));
    close(pipe_fd[1]); // Cierra el extremo de escritura del pipe PADRE

    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        wait(NULL); // Espera a que todos los hijos terminen
    }

    sem_destroy(&sem_burger);
    sem_destroy(&sem_vegan);
    sem_destroy(&sem_fries);

    return 0;
}

/*
Definir Estructuras y Variables Globales:

Crear semáforos para controlar el acceso a las estaciones de trabajo (hamburguesas, menú vegano, papas fritas).
Crear mutexes para proteger el acceso a recursos compartidos.
Modificar el Código del Hijo:

Utilizar semáforos para controlar el acceso a las estaciones de trabajo.
Utilizar mutexes para proteger el acceso a recursos compartidos.
Modificar el Código del Padre:

Crear hilos para manejar la preparación de pedidos.
Utilizar semáforos y mutexes para sincronizar la preparación de pedidos.

EXPLICACION
Semáforos:

sem_burger, sem_vegan, sem_fries: Controlan el acceso a las estaciones de trabajo.
Inicializados con sem_init(), donde sem_fries se inicializa con 2 para permitir que dos empleados trabajen simultáneamente en las papas fritas.
Mutex:

mutex_order: Protege el acceso a recursos compartidos (no utilizado en este ejemplo, pero puede ser útil para futuras expansiones).
Hilos:

employee_thread(): Función que ejecuta cada hilo de empleado.
Utiliza semáforos para controlar el acceso a las estaciones de trabajo.
Código del Hijo:

Crea un hilo para cada empleado y espera a que termine (pthread_join()).
Recibe el pedido del pipe.
Código del Padre:

Envía el pedido a través del pipe.
Espera a que todos los hijos terminen (wait(NULL)).
*/