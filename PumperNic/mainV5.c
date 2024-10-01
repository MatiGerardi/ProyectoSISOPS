#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_EMPLOYEES 5

int main() {
    int pipeHamburguesas[2], pipeVegano[2], pipePapas1[2], pipePapas2[2], pipeDespacho[2];
    pid_t pid[NUM_EMPLOYEES];

    // Crear pipes
    pipe(pipeHamburguesas);
    pipe(pipeVegano);
    pipe(pipePapas1);
    pipe(pipePapas2);
    pipe(pipeDespacho);

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
            } else if (i == 2 || i == 3) { // Procesos de Papas Fritas
                int pipePapas[2] = (i == 2) ? pipePapas1 : pipePapas2;
                close(pipePapas[1]);
                while (1) {
                    // Preparar papas fritas
                    char pedido;
                    read(pipePapas[0], &pedido, sizeof(char));
                    printf("Preparando papas fritas\n");
                    write(pipeDespacho[1], "P", sizeof(char));
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

    while (1) {
        // Recibir pedido
        char pedido;
        scanf(" %c", &pedido);
        if (pedido == 'H') {
            write(pipeHamburguesas[1], &pedido, sizeof(char));
        } else if (pedido == 'V') {
            write(pipeVegano[1], &pedido, sizeof(char));
        } else if (pedido == 'P') {
            write(pipePapas1[1], &pedido, sizeof(char));
        } else if (pedido == 'Q') {
            // Salir
            break;
        }
    }

    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        wait(NULL);
    }

    return 0;
}