#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_ALUMNOS 25
#define NUM_HORAS 13

#define RESERVA1 0
#define RESERVA2 1
#define CONSULTA 2
#define CANCELACION 3

struct shared_data {
    int reservas[NUM_HORAS];
    sem_t consulta;
    sem_t semaforo;
};

struct alumno_args {
    int id;
    struct shared_data *shared;
};

void inicializarTablaDeReservas(struct shared_data *shared) {
    for (int i = 0; i < NUM_HORAS; i++) {
        shared->reservas[i] = 0;
    }
}

int reservarHoraAleatoria() {
    int hora = rand() % NUM_HORAS;
    return hora;
}

void realizarReserva(int alumno, struct shared_data *shared) {
    int hora = reservarHoraAleatoria();
    sem_wait(&shared->semaforo);
    if (shared->reservas[hora] == 0) {
        shared->reservas[hora] = 1;
        printf("Alumno %d reservo la hora %d\n", alumno, hora + 9);
    } else {
        printf("Alumno %d no pudo reservar la hora %d\n", alumno, hora + 9);
    }
    sem_post(&shared->semaforo);
}

void realizarConsulta(int alumno, struct shared_data *shared) {
    int hora = reservarHoraAleatoria();
    sem_wait(&shared->semaforo);
    if(sem_trywait(&shared->consulta) == 0){
		if (shared->reservas[hora] == 0) {
			printf("Alumno %d consulto y la hora %d esta disponible\n", alumno, hora + 9);
		}else {
			printf("Alumno %d consulto y la hora %d esta reservada\n", alumno, hora + 9);
		}
	}
    
    sem_post(&shared->semaforo);
}

void realizarCancelacion(int alumno, struct shared_data *shared) {
    int hora = reservarHoraAleatoria();
    sem_wait(&shared->semaforo);
    if (shared->reservas[hora] == 1) {
        shared->reservas[hora] = 0;
        printf("Alumno %d cancelo la reserva de la hora %d\n", alumno, hora + 9);
    } else {
        printf("Alumno %d no pudo cancelar la reserva de la hora %d\n", alumno, hora + 9);
    }
    sem_post(&shared->semaforo);
}

void *alumno(void *arg) {
    struct alumno_args *alumno_arg = (struct alumno_args *)arg;
    int id = alumno_arg->id;
    struct shared_data *shared = alumno_arg->shared;
    for (int i = 0; i < 4; i++) {
        int operacion;
        operacion = rand() % 4; // 0: Reserva, 1: Reserva, 2: Consulta, 3: Cancelacion
        if (operacion == RESERVA1 || operacion == RESERVA2) {
            realizarReserva(id, shared);
        } else if (operacion == CONSULTA) {
            realizarConsulta(id, shared);
        } else if (operacion == CANCELACION) {
            realizarCancelacion(id, shared);
        }
        sleep(1); // Tiempo de operación
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    // Crear la memoria compartida para la estructura que contiene la tabla de reservas y los semáforos
    int shmid = shmget(IPC_PRIVATE, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // Adjuntar la memoria compartida al espacio de direcciones del proceso
    struct shared_data *shared = (struct shared_data *)shmat(shmid, NULL, 0);
    if (shared == (struct shared_data *)(-1)) {
        perror("shmat");
        exit(1);
    }

    // Inicializar los semáforos
    if (sem_init(&shared->semaforo, 1, 1) != 0) {
        perror("sem_init");
        exit(1);
    }

    if (sem_init(&shared->consulta, 1, 1) != 0) {
        perror("sem_init");
        exit(1);
    }

    inicializarTablaDeReservas(shared);

    pthread_t alumnos[NUM_ALUMNOS];
    struct alumno_args alumno_args[NUM_ALUMNOS];

    for (int i = 0; i < NUM_ALUMNOS; i++) {
        alumno_args[i].id = i;
        alumno_args[i].shared = shared;
        pthread_create(&alumnos[i], NULL, alumno, &alumno_args[i]);
    }

    for (int i = 0; i < NUM_ALUMNOS; i++) {
        pthread_join(alumnos[i], NULL);
    }

    // Esperar a que todos los procesos hijos (alumnos) terminen
    for (int i = 0; i < NUM_ALUMNOS; i++) {
        wait(NULL);
    }

    // Desvincular la memoria compartida y destruir los semáforos
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    sem_destroy(&shared->semaforo);
    sem_destroy(&shared->consulta);

    return 0;
}
