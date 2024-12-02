#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t start_semaphore;
sem_t rueda_semaphore;
sem_t chasis_semaphore;
sem_t motor_semaphore;
sem_t paint_semaphore;

void* operario1(void* arg) {
    while (1) {
        sem_wait(&start_semaphore);
        
        printf("Operario 1: Rueda armada\n");
        sem_post(&rueda_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario2(void* arg) {
    while (1) {
        sem_wait(&rueda_semaphore);
        sem_wait(&rueda_semaphore);
        printf("Operario 2: Chasis armado\n");
        sem_post(&chasis_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario3(void* arg) {
    while (1) {
        sem_wait(&chasis_semaphore);
        printf("Operario 3: Motor agregado\n");
        sem_post(&motor_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario4(void* arg) {
    while (1) {
        sem_wait(&motor_semaphore);
        if (rand() % 2 == 0) {
            printf("Operario 4: Moto pintada de verde\n");
        } else {
            printf("Operario 4: Moto pintada de rojo\n");
        }
        sem_post(&paint_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario5(void* arg) {
    while (1) {
        sem_wait(&motor_semaphore);
        if (rand() % 2 == 0) {
            printf("Operario 5: Moto pintada de verde\n");
        } else {
            printf("Operario 5: Moto pintada de rojo\n");
        }
        sem_post(&paint_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario6(void* arg) {
    while (1) {
        sem_wait(&paint_semaphore);
        sem_post(&start_semaphore);
        sem_post(&start_semaphore);
        sem_wait(&paint_semaphore);
        printf("Operario 6: Equipamiento extra agregado\n");
        sem_post(&start_semaphore);
        sem_post(&start_semaphore);
        sleep(1);
    }
    return NULL;
}

int main() {
    sem_init(&start_semaphore, 0, 2); // Inicializar en 2 para permitir la creación de dos ruedas
    sem_init(&rueda_semaphore, 0, 0);
    sem_init(&chasis_semaphore, 0, 0);
    sem_init(&motor_semaphore, 0, 0);
    sem_init(&paint_semaphore, 0, 0);

    pthread_t threads[6];
    pthread_create(&threads[0], NULL, operario1, NULL);
    pthread_create(&threads[1], NULL, operario2, NULL);
    pthread_create(&threads[2], NULL, operario3, NULL);
    pthread_create(&threads[3], NULL, operario4, NULL);
    pthread_create(&threads[4], NULL, operario5, NULL);
    pthread_create(&threads[5], NULL, operario6, NULL);

    for (int i = 0; i < 6; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&start_semaphore);
    sem_destroy(&rueda_semaphore);
    sem_destroy(&chasis_semaphore);
    sem_destroy(&motor_semaphore);
    sem_destroy(&paint_semaphore);

    return 0;
}