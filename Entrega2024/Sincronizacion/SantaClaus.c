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
pthread_mutex_t renoMutex;
pthread_mutex_t elfoMutex;

void* santa(void* arg) {
    while (1) {
        sem_wait(&santaSem);
        // Verificar si todos los renos han regresado
        if (sem_trywait(&contadorRenos) != 0) {
            //puede pasar que hay 8 renos que regresaron, y hay tres elfos esperando para ser 
            //atendidos, pero como el contador de renos es cero los va a atender santa, pero en realidad
            // falta un reno para poder ser atendidos
            //quizas puedo usar un semaforo mas, o algo nose
            // o inicializar el semaforo en 9 y siempre mandar la señal a santa para que se despierte y si el try wait da dif de cero ahi santa se despierta
            
                // Preparar el trineo
            pthread_mutex_lock(&renoMutex);
            printf("Santa está preparando el trineo.\n");
            fflush(stdout);
            for (int i = 0; i < NUM_RENOS; i++) {
                sem_post(&renoSem);
            }
            for (int i = 0; i < NUM_RENOS-1; i++) {
                sem_post(&contadorRenos); // Resetear el contador de renos
            }
            pthread_mutex_unlock(&renoMutex);
        }
        else {
            // Restaurar el valor del semáforo contadorRenos
                sem_post(&contadorRenos);
            // Verificar si tres elfos necesitan ayuda
                if (sem_trywait(&contadorElfos) != 0) {
                    // Ayudar a los elfos
                    pthread_mutex_lock(&elfoMutex);
                    printf("Santa está ayudando a los elfos.\n");
                    fflush(stdout);
                    for (int i = 0; i < 3; i++) {
                        sem_post(&elfoSem);
                    }
                    for (int i = 0; i < 2; i++) {
                        sem_post(&contadorElfos); // Resetear el contador de elfos
                    }
                    pthread_mutex_unlock(&elfoMutex);
                }
                else {
                    // Restaurar el valor del semáforo contadorElfos
                        sem_post(&contadorElfos);
                }
            }
    }
    return NULL;
}

void* reno(void* arg) {
    while (1) {
        sleep(rand() % 5 + 1); // Simular el regreso de los renos con un tiempo aleatorio
        pthread_mutex_lock(&renoMutex);
        if (sem_trywait(&contadorRenos) == 0) {
            printf("Reno regresando.\n");
            fflush(stdout);
        }
        else{
                printf("Último reno regresado, despertando a Santa.\n");
                fflush(stdout);
                sem_post(&santaSem);
            }
        pthread_mutex_unlock(&renoMutex);
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
            pthread_mutex_lock(&elfoMutex);
            if (sem_trywait(&contadorElfos) == 0) {
                printf("Elfo necesita ayuda.\n");
                fflush(stdout);
            }
            else{
                    printf("Tres elfos necesitan ayuda, despertando a Santa.\n");
                    fflush(stdout);
                    sem_post(&santaSem);
            }
            pthread_mutex_unlock(&elfoMutex);
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
    sem_destroy(&renoSem);
    sem_destroy(&elfoSem);
    sem_destroy(&contadorRenos);
    sem_destroy(&contadorElfos);
    pthread_mutex_destroy(&renoMutex);
    pthread_mutex_destroy(&elfoMutex);

    return 0;
}