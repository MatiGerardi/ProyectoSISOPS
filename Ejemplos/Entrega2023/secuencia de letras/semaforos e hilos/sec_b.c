#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

sem_t semA, semB, semC, semD;

void* imprimirA(void* arg){
    while(1){
        sem_wait(&semA);
        printf("A");
        sem_post(&semB);
    }
}
void* imprimirB(void* arg){
    while(1){
        sem_wait(&semB);
        printf("B");
        sem_post(&semC);
        sem_wait(&semB);
        sem_post(&semA);
    }
}

void* imprimirC(void* arg){
    while(1){
        sem_wait(&semC);//primer llamado b, segunda desde d
        sem_post(&semD);
        sem_wait(&semC);//primer señal desde d
        sem_post(&semB);
        sem_wait(&semC);//segunda señal desde b
        printf("C");
        sem_post(&semD);
        
    }
}

void* imprimirD(void* arg){
    while(1){
        sem_wait(&semD);//primer llamado desde c
        sem_post(&semC);
        sem_wait(&semD);//segunda señal desde c
        sem_post(&semC);
        sem_wait(&semD);
        sem_post(&semC);
        sem_wait(&semD);
        printf("D");
        sem_post(&semB);
    }
}

int main() {

    // Inicializar los semáforos
    sem_init(&semA, 0, 1);
    sem_init(&semB, 0, 0);
    sem_init(&semC, 0, 0);
	sem_init(&semD, 0, 0);
    pthread_t threadA, threadB, threadC, threadD;

    // Crear los hilos
    pthread_create(&threadA, NULL, imprimirA, NULL);
    pthread_create(&threadB, NULL, imprimirB, NULL);
    pthread_create(&threadC, NULL, imprimirC, NULL);
    pthread_create(&threadD, NULL, imprimirD, NULL);

    // Esperar a que los hilos terminen (esto normalmente no se hace en un programa de este tipo)
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    pthread_join(threadD, NULL);

    return 0;
}
