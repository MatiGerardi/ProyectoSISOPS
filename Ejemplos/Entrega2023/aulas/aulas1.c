#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NUM_ALUMNOS 25
#define NUM_HORAS 13

pthread_mutex_t reserva_mutex; // Mutex para zona critica de de reservas
int reservas[NUM_HORAS]; // Tabla de reservas: 0 = no reservado, 1 = reservado

sem_t consulta; // Vemos si hay alguien consultando

void inicializarTablaDeReservas() {
    for (int i = 0; i < NUM_HORAS; i++) {
        reservas[i] = 0;
    }
}

int reservarHoraAleatoria() {
    int hora = rand() % NUM_HORAS;
    return hora;
}

void realizarReserva(int alumno) {
    int hora = reservarHoraAleatoria();
    pthread_mutex_lock(&reserva_mutex);
    if (reservas[hora] == 0) {
        reservas[hora] = 1;
        printf("Alumno %d reservo la hora %d\n", alumno, hora + 9);
    } else {
        printf("Alumno %d no pudo reservar la hora %d\n", alumno, hora + 9);
    }
    pthread_mutex_unlock(&reserva_mutex);
}

void realizarConsulta(int alumno) {
    int hora = reservarHoraAleatoria();
    //thread_mutex_lock(&reserva_mutex);
    if (reservas[hora] == 0) {
        printf("Alumno %d consulto y la hora %d esta disponible\n", alumno, hora + 9);
    } else {
        printf("Alumno %d consulto y la hora %d esta reservada\n", alumno, hora + 9);
    }
    //pthread_mutex_unlock(&reserva_mutex);
}

void realizarCancelacion(int alumno) {
    int hora = reservarHoraAleatoria();
    pthread_mutex_lock(&reserva_mutex);
    if (reservas[hora] == 1) {
        reservas[hora] = 0;
        printf("Alumno %d cancelo la reserva de la hora %d\n", alumno, hora + 9);
    } else {
        printf("Alumno %d no pudo cancelar la reserva de la hora %d\n", alumno, hora + 9);
    }
    pthread_mutex_unlock(&reserva_mutex);
}

void *alumno(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 4; i++) {
        int operacion = rand() % 4; // 0: Reserva 1: Reserva, 2: Consulta, 3: Cancelacion
        if (operacion == 0 || operacion == 1) {
            realizarReserva(id);
        } else if (operacion == 2) {
            if (sem_trywait(&consulta) == 0) {
                realizarConsulta(id);
                sem_post(&consulta); // Permitir que otros alumnos puedan consultar
            } else {
                printf("Alumno %d no pudo realizar la consulta\n", id);
            }
        } else if (operacion == 3) {
            realizarCancelacion(id);
        }
        sleep(1); // Tiempo de operación
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    inicializarTablaDeReservas();
    pthread_mutex_init(&reserva_mutex, NULL);
    
    sem_init(&consulta, 0, 1);

    pthread_t alumnos[NUM_ALUMNOS];
    int alumno_ids[NUM_ALUMNOS];

    for (int i = 0; i < NUM_ALUMNOS; i++) {
        alumno_ids[i] = i;
        pthread_create(&alumnos[i], NULL, alumno, &alumno_ids[i]);
    }

    for (int i = 0; i < NUM_ALUMNOS; i++) {
        pthread_join(alumnos[i], NULL);
    }

    pthread_mutex_destroy(&reserva_mutex);

    return 0;
}
