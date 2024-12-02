

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // Para usar sleep
#include <time.h>   // Para generar números aleatorios




#define NUM_RENOS 9
#define NUM_ELFOS 10




sem_t santaSem;
sem_t habilitarRenos;
sem_t habilitarElfos;
sem_t contadorRenos;
sem_t contadorElfos;
pthread_mutex_t renoMutex;
pthread_mutex_t elfoMutex;




void* santa(void* arg) {
    while (1) {
        sem_wait(&santaSem);
        // Verificar si todos los renos han regresado
        pthread_mutex_lock(&renoMutex);
        if (sem_trywait(&contadorRenos) != 0) {
            if(sem_trywait(&habilitarRenos) != 0){
                printf("Santa está preparando el trineo.\n");
                fflush(stdout);
                for (int i = 0; i < NUM_RENOS-1; i++) {
                    sem_post(&contadorRenos); // Resetear el contador de renos
                }
                printf("Todos los renos se engancharon al trineo.\n");
                fflush(stdout);
                sem_post(&habilitarRenos);
                pthread_mutex_unlock(&renoMutex);
            }
            else{
                sem_post(&habilitarRenos);
                pthread_mutex_unlock(&renoMutex);
            }




        }
        else {
                sem_post(&contadorRenos);
                pthread_mutex_unlock(&renoMutex);
            // Verificar si tres elfos necesitan ayuda
                pthread_mutex_lock(&elfoMutex);
                if (sem_trywait(&contadorElfos) != 0) {
                    if(sem_trywait(&habilitarElfos) != 0){
                        // Ayudar a los elfos
                        printf("Santa está ayudando a los elfos.\n");
                        fflush(stdout);
                        for (int i = 0; i < 2; i++) {
                            sem_post(&contadorElfos); // Resetear el contador de elfos
                        }
                        printf("Todos los elfos recibieron ayuda de Santa.\n");
                        fflush(stdout);
                        sem_post(&habilitarElfos);
                        pthread_mutex_unlock(&elfoMutex);
                    }
                    else{
                        sem_post(&habilitarElfos);
                        pthread_mutex_unlock(&elfoMutex);
                    }
                }
                else{
                    sem_post(&contadorElfos);
                    pthread_mutex_unlock(&elfoMutex);
                }
            }
    }
    return NULL;
}




void* reno(void* arg) {
    while (1) {
        sleep(rand() % 5 + 1); // Simular el regreso de los renos con un tiempo aleatorio
        sem_wait(&habilitarRenos);
        pthread_mutex_lock(&renoMutex);
        if (sem_trywait(&contadorRenos) == 0) {
            printf("Reno regresando.\n");
            fflush(stdout);
            sem_post(&habilitarRenos);
        }
        else{
                printf("Último reno regresado, despertando a Santa.\n");
                fflush(stdout);
                sem_post(&santaSem);
            }
        pthread_mutex_unlock(&renoMutex);
    }
    return NULL;
}




void* elfo(void* arg) {
    while (1) {
        sleep(rand() % 5 + 1); // Simular el tiempo de trabajo de los elfos con un tiempo aleatorio
        if (rand() % 2 == 0) { // Simular que un elfo necesita ayuda con una probabilidad del 50%
            sem_wait(&habilitarElfos);
            pthread_mutex_lock(&elfoMutex);
            if (sem_trywait(&contadorElfos) == 0) {
                printf("Elfo necesita ayuda.\n");
                fflush(stdout);
                sem_post(&habilitarElfos);
            }
            else{
                    printf("Tres elfos necesitan ayuda, despertando a Santa.\n");
                    fflush(stdout);
                    sem_post(&santaSem);
            }
            pthread_mutex_unlock(&elfoMutex);
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
    sem_init(&habilitarRenos, 0, 1);
    sem_init(&habilitarElfos, 0, 1);
    sem_init(&contadorRenos, 0, 8);  // Inicializar en 8 para los renos
    sem_init(&contadorElfos, 0, 2);  // Inicializar en 2 para los elfos
    pthread_mutex_init(&renoMutex, NULL); // Inicializar el mutex para los renos
    pthread_mutex_init(&elfoMutex, NULL); // Inicializar el mutex para los elfos




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
    sem_destroy(&contadorRenos);
    sem_destroy(&contadorElfos);
    sem_destroy(&habilitarRenos);
    sem_destroy(&habilitarElfos);
    pthread_mutex_destroy(&renoMutex);
    pthread_mutex_destroy(&elfoMutex);




    return 0;
}




