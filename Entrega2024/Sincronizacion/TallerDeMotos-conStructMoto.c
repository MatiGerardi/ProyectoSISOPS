#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct {
    int ruedas;
    int chasis;
    int motor;
    char paint[6];
    int equipamiento_extra;
} Moto;

typedef struct {
    Moto *moto;
    int moto_count;
    sem_t start_semaphore;
    sem_t rueda_semaphore;
    sem_t chasis_semaphore;
    sem_t motor_semaphore;
    sem_t paint_semaphore;
    sem_t equipamiento_extra_semaphore;
} TallerDeMotos;

void* operario1(void* arg) {
    TallerDeMotos *taller = (TallerDeMotos*) arg;
    while (1) {
        sem_wait(&taller->start_semaphore);
        taller->moto = (Moto*) malloc(sizeof(Moto));
        taller->moto_count++;
        taller->moto->ruedas = 0;
        
        taller->moto->ruedas++;
        printf("Operario 1: Primera rueda armada\n");
        sem_post(&taller->rueda_semaphore);
        sleep(1);
        
        taller->moto->ruedas++;
        printf("Operario 1: Segunda rueda armada\n");
        sem_post(&taller->rueda_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario2(void* arg) {
    TallerDeMotos *taller = (TallerDeMotos*) arg;
    while (1) {
        sem_wait(&taller->rueda_semaphore);
        sem_wait(&taller->rueda_semaphore);
        taller->moto->chasis = 1;
        printf("Operario 2: Chasis armado\n");
        sem_post(&taller->chasis_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario3(void* arg) {
    TallerDeMotos *taller = (TallerDeMotos*) arg;
    while (1) {
        sem_wait(&taller->chasis_semaphore);
        taller->moto->motor = 1;
        printf("Operario 3: Motor agregado\n");
        sem_post(&taller->motor_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario4(void* arg) {
    TallerDeMotos *taller = (TallerDeMotos*) arg;
    while (1) {
        sem_wait(&taller->motor_semaphore);
        if (rand() % 2 == 0) {
            snprintf(taller->moto->paint, sizeof(taller->moto->paint), "verde");
        } else {
            snprintf(taller->moto->paint, sizeof(taller->moto->paint), "rojo");
        }
        printf("Operario 4: Moto pintada de %s\n", taller->moto->paint);
        sem_post(&taller->paint_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario5(void* arg) {
    TallerDeMotos *taller = (TallerDeMotos*) arg;
    while (1) {
        sem_wait(&taller->motor_semaphore);
        if (rand() % 2 == 0) {
            snprintf(taller->moto->paint, sizeof(taller->moto->paint), "verde");
        } else {
            snprintf(taller->moto->paint, sizeof(taller->moto->paint), "rojo");
        }
        printf("Operario 5: Moto pintada de %s\n", taller->moto->paint);
        sem_post(&taller->paint_semaphore);
        sleep(1);
    }
    return NULL;
}

void* operario6(void* arg) {
    TallerDeMotos *taller = (TallerDeMotos*) arg;
    while (1) {
        sem_wait(&taller->paint_semaphore);
        if (taller->moto_count % 2 == 0) {
            taller->moto->equipamiento_extra = 1;
            printf("Operario 6: Equipamiento extra agregado\n");
        }
        printf("Moto completa\n");
        sem_post(&taller->equipamiento_extra_semaphore);
        free(taller->moto);
        sem_post(&taller->start_semaphore);
        sleep(1);
    }
    return NULL;
}

int main() {
    TallerDeMotos taller;
    taller.moto_count = 0;
    sem_init(&taller.start_semaphore, 0, 1);
    sem_init(&taller.rueda_semaphore, 0, 0);
    sem_init(&taller.chasis_semaphore, 0, 0);
    sem_init(&taller.motor_semaphore, 0, 0);
    sem_init(&taller.paint_semaphore, 0, 0);
    sem_init(&taller.equipamiento_extra_semaphore, 0, 0);

    pthread_t threads[6];
    pthread_create(&threads[0], NULL, operario1, &taller);
    pthread_create(&threads[1], NULL, operario2, &taller);
    pthread_create(&threads[2], NULL, operario3, &taller);
    pthread_create(&threads[3], NULL, operario4, &taller);
    pthread_create(&threads[4], NULL, operario5, &taller);
    pthread_create(&threads[5], NULL, operario6, &taller);

    for (int i = 0; i < 6; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&taller.start_semaphore);
    sem_destroy(&taller.rueda_semaphore);
    sem_destroy(&taller.chasis_semaphore);
    sem_destroy(&taller.motor_semaphore);
    sem_destroy(&taller.paint_semaphore);
    sem_destroy(&taller.equipamiento_extra_semaphore);

    return 0;
}