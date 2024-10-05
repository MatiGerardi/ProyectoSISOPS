#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

// Declaración de semáforos
sem_t semA, semB, semC;

// Función para imprimir "A"
void *imprimirA(void *arg) {
    while (1) {
        sem_wait(&semA);
        printf("A");
        sem_post(&semB);
        sem_wait(&semA);
    }
    return NULL;
}

// Función para imprimir "B"
void *imprimirB(void *arg) {
    while (1) {
        sem_wait(&semB);
        printf("B");
        sem_post(&semC);
        sem_post(&semA);
    }
    return NULL;
}

// Función para imprimir "C"
void *imprimirC(void *arg) {
    while (1) {
        sem_wait(&semC);
        sem_wait(&semC);
        printf("C");
        sem_post(&semA);
        sem_post(&semA);
    }
    return NULL;
}

int main() {
    pthread_t threadA, threadB, threadC;

    // Inicialización de semáforos
    sem_init(&semA, 0, 2);
    sem_init(&semB, 0, 0);
    sem_init(&semC, 0, 0);


    // Creación de hilos
    pthread_create(&threadA, NULL, imprimirA, NULL);
    pthread_create(&threadB, NULL, imprimirB, NULL);
    pthread_create(&threadC, NULL, imprimirC, NULL);


    // Espera indefinida para mantener los hilos en ejecución
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);


    return 0;
}
