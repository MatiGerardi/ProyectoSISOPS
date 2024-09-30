#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_EMPLOYEES 5

void prepare_burger() {
    printf("Empleado preparando hamburguesa...\n");
    sleep(2); // Simula el tiempo de preparación
}

void prepare_vegan() {
    printf("Empleado preparando menú vegano...\n");
    sleep(3); // Simula el tiempo de preparación
}

void prepare_fries() {
    printf("Empleado preparando papas fritas...\n");
    sleep(1); // Simula el tiempo de preparación
}

void receive_order(int pipe_fd) {
    char buffer[256];
    read(pipe_fd, buffer, sizeof(buffer));
    printf("Empleado recibiendo pedido: %s\n", buffer);
}

int main() {
    int pipe_fd[2];
    pid_t pids[NUM_EMPLOYEES];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        if ((pids[i] = fork()) == 0) {
            // Código del hijo
            close(pipe_fd[1]); // Cierra el extremo de escritura del pipe
            if (i == 0) {
                prepare_burger();
            } else if (i == 1) {
                prepare_vegan();
            } else {
                prepare_fries();
            }
            receive_order(pipe_fd[0]);
            close(pipe_fd[0]); // Cierra el extremo de lectura del pipe
            exit(EXIT_SUCCESS);
        }
    }

    // Código del padre
    close(pipe_fd[0]); // Cierra el extremo de lectura del pipe
    char *order = "Pedido de cliente VIP";
    write(pipe_fd[1], order, sizeof(order));
    close(pipe_fd[1]); // Cierra el extremo de escritura del pipe

    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        wait(NULL); // Espera a que todos los hijos terminen
    }

    return 0;
}
