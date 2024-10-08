#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_EMPLOYEES 5

int main() {
    int pipeHamburguesas[2], pipeVegano[2], pipePapas1[2], pipePapas2[2], pipeDespacho[2];
    int pipeStatusPapas1[2], pipeStatusPapas2[2]; // Pipes para verificar el estado de los empleados de papas fritas
    pid_t pid[NUM_EMPLOYEES];

    // Crear pipes
    pipe(pipeHamburguesas);
    pipe(pipeVegano);
    pipe(pipePapas1);
    pipe(pipePapas2);
    pipe(pipeDespacho);
    pipe(pipeStatusPapas1);
    pipe(pipeStatusPapas2);

    // Crear procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        pid[i] = fork();
        if (pid[i] < 0) {
            perror("Error al crear proceso");
            exit(EXIT_FAILURE);
        }
        if (pid[i] == 0) {
            // Código para cada proceso hijo
            if (i == 0) { // Proceso de Hamburguesas
                close(pipeHamburguesas[1]);
                while (1) {
                    // Preparar hamburguesa
                    char pedido;
                    read(pipeHamburguesas[0], &pedido, sizeof(char));
                    printf("Preparando hamburguesa\n");
                    write(pipeDespacho[1], "H", sizeof(char));
                }
            } else if (i == 1) { // Proceso de Menú Vegano
                close(pipeVegano[1]);
                while (1) {
                    // Preparar menú vegano
                    char pedido;
                    read(pipeVegano[0], &pedido, sizeof(char));
                    printf("Preparando menú vegano\n");
                    write(pipeDespacho[1], "V", sizeof(char));
                }
            } else if (i == 2) { // Proceso de Papas Fritas 1
                close(pipePapas1[1]);
                close(pipeStatusPapas1[0]);
                while (1) {
                    // Preparar papas fritas
                    char pedido;
                    read(pipePapas1[0], &pedido, sizeof(char));
                    printf("Preparando papas fritas (Empleado 1)\n");
                    write(pipeDespacho[1], "P", sizeof(char));
                    write(pipeStatusPapas1[1], "L", sizeof(char)); // Indicar que está libre
                }
            } else if (i == 3) { // Proceso de Papas Fritas 2
                close(pipePapas2[1]);
                close(pipeStatusPapas2[0]);
                while (1) {
                    // Preparar papas fritas
                    char pedido;
                    read(pipePapas2[0], &pedido, sizeof(char));
                    printf("Preparando papas fritas (Empleado 2)\n");
                    write(pipeDespacho[1], "P", sizeof(char));
                    write(pipeStatusPapas2[1], "L", sizeof(char)); // Indicar que está libre
                }
            } else if (i == 4) { // Proceso de Despacho
                close(pipeDespacho[1]);
                while (1) {
                    // Despachar pedido
                    char pedido;
                    read(pipeDespacho[0], &pedido, sizeof(char));
                    printf("Despachando pedido: %c\n", pedido);
                }
            }
            exit(0);
        }
    }

    // Proceso principal
    close(pipeHamburguesas[0]);
    close(pipeVegano[0]);
    close(pipePapas1[0]);
    close(pipePapas2[0]);
    close(pipeDespacho[0]);
    close(pipeStatusPapas1[1]);
    close(pipeStatusPapas2[1]);

    while (1) {
        // Recibir pedido
        char pedido;
        scanf(" %c", &pedido);
        if (pedido == 'H') {
            write(pipeHamburguesas[1], &pedido, sizeof(char));
        } else if (pedido == 'V') {
            write(pipeVegano[1], &pedido, sizeof(char));
        } else if (pedido == 'P') {
            // Verificar si el empleado 1 está libre
            char status;
            read(pipeStatusPapas1[0], &status, sizeof(char));
            if (status == 'L') {
                write(pipePapas1[1], &pedido, sizeof(char));
            } else {
                // Si el empleado 1 está ocupado, asignar al empleado 2
                read(pipeStatusPapas2[0], &status, sizeof(char));
                if (status == 'L') {
                    write(pipePapas2[1], &pedido, sizeof(char));
                } else {
                    // Si ambos están ocupados, esperar y reintentar
                    printf("Ambos empleados de papas fritas están ocupados. Reintentando...\n");
                    continue;
                }
            }
        } else {
        }
    }

    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        wait(NULL);
    }

    return 0;
}