#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // Para usar sleep
#include <time.h>   // Para generar números aleatorios

#define NUM_RENOS 9
#define NUM_ELFOS 10

sem_t santaSem;
sem_t renoSem;
sem_t elfoSem;
sem_t contadorRenos;
sem_t contadorElfos;
pthread_mutex_t mutex;

void* santa(void* arg) {
    while (1) {
        sem_wait(&santaSem);
        pthread_mutex_lock(&mutex);
        int valorRenos, valorElfos;
        sem_getvalue(&contadorRenos, &valorRenos);
        sem_getvalue(&contadorElfos, &valorElfos);
        if (valorRenos == NUM_RENOS) {
            // Preparar el trineo
            printf("Santa está preparando el trineo.\n");
            fflush(stdout);
            for (int i = 0; i < NUM_RENOS; i++) {
                sem_post(&renoSem);
            }
            for (int i = 0; i < NUM_RENOS; i++) {
                sem_wait(&contadorRenos); // Resetear el contador de renos
            }
        } else if (valorElfos == 3) {
            // Ayudar a los elfos
            printf("Santa está ayudando a los elfos.\n");
            fflush(stdout);
            for (int i = 0; i < 3; i++) {
                sem_post(&elfoSem);
            }
            for (int i = 0; i < 3; i++) {
                sem_wait(&contadorElfos); // Resetear el contador de elfos
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* reno(void* arg) {
    while (1) {
        sleep(rand() % 5 + 1); // Simular el regreso de los renos con un tiempo aleatorio
        pthread_mutex_lock(&mutex);
        sem_post(&contadorRenos);
        int valorRenos;
        sem_getvalue(&contadorRenos, &valorRenos);
        printf("Reno regresando. Total de renos regresados: %d\n", valorRenos);
        fflush(stdout);
        if (valorRenos == NUM_RENOS) {
            printf("Último reno regresado, despertando a Santa.\n");
            fflush(stdout);
            sem_post(&santaSem);
        }
        pthread_mutex_unlock(&mutex);
        sem_wait(&renoSem);
        // Engancharse al trineo
        printf("Reno enganchándose al trineo.\n");
        fflush(stdout);
    }
    return NULL;
}

void* elfo(void* arg) {
    while (1) {
        sleep(rand() % 5 + 1); // Simular el tiempo de trabajo de los elfos con un tiempo aleatorio
        if (rand() % 2 == 0) { // Simular que un elfo necesita ayuda con una probabilidad del 50%
            pthread_mutex_lock(&mutex);
            sem_post(&contadorElfos);
            int valorElfos;
            sem_getvalue(&contadorElfos, &valorElfos);
            printf("Elfo necesita ayuda. Total de elfos esperando: %d\n", valorElfos);
            fflush(stdout);
            if (valorElfos == 3) {
                printf("Tres elfos necesitan ayuda, despertando a Santa.\n");
                fflush(stdout);
                sem_post(&santaSem);
            }
            pthread_mutex_unlock(&mutex);
            sem_wait(&elfoSem);
            // Recibir ayuda de Santa
            printf("Elfo recibiendo ayuda de Santa.\n");
            fflush(stdout);
        }
    }
    return NULL;
}

int main() {
    srand(time(NULL)); // Inicializar la semilla para números aleatorios

    pthread_t hiloSanta;
    pthread_t hilosRenos[NUM_RENOS];
    pthread_t hilosElfos[NUM_ELFOS];

    sem_init(&santaSem, 0, 0);
    sem_init(&renoSem, 0, 0);
    sem_init(&elfoSem, 0, 0);
    sem_init(&contadorRenos, 0, 0);
    sem_init(&contadorElfos, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&hiloSanta, NULL, santa, NULL);
    for (int i = 0; i < NUM_RENOS; i++) {
        pthread_create(&hilosRenos[i], NULL, reno, NULL);
    }
    for (int i = 0; i < NUM_ELFOS; i++) {
        pthread_create(&hilosElfos[i], NULL, elfo, NULL);
    }

    pthread_join(hiloSanta, NULL);
    for (int i = 0; i < NUM_RENOS; i++) {
        pthread_join(hilosRenos[i], NULL);
    }
    for (int i = 0; i < NUM_ELFOS; i++) {
        pthread_join(hilosElfos[i], NULL);
    }

    sem_destroy(&santaSem);
    sem_destroy(&renoSem);
    sem_destroy(&elfoSem);
    sem_destroy(&contadorRenos);
    sem_destroy(&contadorElfos);
    pthread_mutex_destroy(&mutex);

    return 0;
}