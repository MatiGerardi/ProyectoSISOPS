#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_EMPLOYEES 5
#define NUM_CLIENTES 10
#define COLA_CLIENTES 10

// Esta bien?
sem_t cola_normal, cola_vip;

typedef struct {
    char pedido[4]; // Combinación de 'H', 'V', 'P'
    int esVIP;
} Cliente;

int pipeHamburguesas[2], pipeVegano[2], pipeFritas[2], pipeDistribucion[2],
    pipeClientes[2], pipeClientesVIP[2],
    pipeHamRecep[2], pipeVegRecep[2], pipeFritasRecep[2];
Cliente clientes[NUM_CLIENTES];

void generarPedidos(Cliente clientes[]) {
    srand(time(NULL));
    for (int i = 0; i < NUM_CLIENTES; i++) {
        int numPedidos = rand() % 3 + 1; // Entre 1 y 3 pedidos
        for (int j = 0; j < numPedidos; j++) {
            int tipoPedido = rand() % 3;
            if (tipoPedido == 0) {
                clientes[i].pedido[j] = 'H';
            } else if (tipoPedido == 1) {
                clientes[i].pedido[j] = 'V';
            } else {
                clientes[i].pedido[j] = 'P';
            }
        }
        clientes[i].pedido[numPedidos] = '\0'; // Terminar la cadena de pedidos
        clientes[i].esVIP = rand() % 2; // 50% de probabilidad de ser VIP
        printf("(generar)Cliente %d: Pedido: %s %s\n", i + 1, clientes[i].pedido, clientes[i].esVIP ? "(VIP)" : "");
    }
}

int main() {
    pid_t pid[NUM_EMPLOYEES];
    
    sem_init(&cola_normal, 0, COLA_CLIENTES);
    sem_init(&cola_vip, 0, COLA_CLIENTES);

    // Crear pipes
    pipe(pipeHamburguesas);
    pipe(pipeVegano);
    pipe(pipeFritas);
    pipe(pipeDistribucion);
    pipe(pipeHamRecep);
    pipe(pipeVegRecep);
    pipe(pipeFritasRecep);
    pipe(pipeClientes);
    pipe(pipeClientesVIP);

    // Crear procesos clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error al crear proceso Cliente");
            exit(1);
        } else if (pid == 0) {
            // Proceso hijo
            generarPedidos(&clientes[i]);

            // Se mete en su cola correspondiente
            if (clientes[i].esVIP && sem_trywait(&cola_vip) == 0) {
                sem_wait(&cola_vip);
                close(pipeClientesVIP[1]);
                write(pipeClientesVIP[0], &clientes[i], sizeof(Cliente));
            } else if (clientes[i].esVIP && sem_trywait(&cola_normal) == 0) {
                sem_wait(&cola_normal);
                close(pipeClientes[1]);
                write(pipeClientes[0], &clientes[i], sizeof(Cliente));
            } else{
                clientes[i].esVIP ? printf(">>>>Cliente %d: se fue. Cola VIP llena\n", i) : printf(">>>>Cliente %d: se fue. Cola NORMAL llena\n", i);
                exit(0);
            }

            // Espera su pedido
            // Cuenta cuantos pedidos tiene de cada tipo
            int tieneH = 0, tieneV = 0, tieneP = 0;
            for (int j = 0; clientes[i].pedido[j] != '\0'; j++) {
                if (clientes[i].pedido[j] == 'H') tieneH++;
                else if (clientes[i].pedido[j] == 'V') tieneV++;
                else if (clientes[i].pedido[j] == 'P') tieneP++;
            }

            // Espera a recibir sus pedidos
            while (tieneH > 0 || tieneV > 0 || tieneP > 0) {
                char respuestaH, respuestaV, respuestaP;
                read(pipeHamRecep[0], &respuestaH, sizeof(char));
                if (respuestaH == 'H' && tieneH > 0) {
                    tieneH--;
                    printf("Cliente %d: recibio Ham\n", i);
                } else if (respuestaV == 'V' && tieneV > 0) {
                    tieneV--;
                    printf("Cliente %d: recibio Veg\n", i);
                } else if (respuestaP == 'P' && tieneP > 0) {
                    tieneP--;
                    printf("Cliente %d: recibio Fritas\n", i);
                }
            }
            clientes[i].esVIP ? sem_post(&cola_vip) : sem_post(&cola_normal);
            printf(">>>>Cliente %d: se fue con su pedido\n", i);
            exit(0);
        }
    }

    // Crear procesos empleados
    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        pid[i] = fork();
        if (pid[i] < 0) {
            perror("Error al crear proceso Empledos");
            exit(EXIT_FAILURE);
        }
        if (pid[i] == 0) {
            // Código para cada proceso hijo
            if (i == 0) { // Proceso de Hamburguesas
                close(pipeHamburguesas[1]);
                close(pipeHamRecep[0]);
                while (1) {
                    // Preparar hamburguesa
                    char pedido;
                    read(pipeHamburguesas[0], &pedido, sizeof(char));
                    printf("Preparando hamburguesa\n");
                    sleep(1);
                    write(pipeHamRecep[1], "H", sizeof(char));
                }
            } else if (i == 1) { // Proceso de Menú Vegano
                close(pipeVegano[1]);
                close(pipeVegRecep[0]);
                while (1) {
                    // Preparar menú vegano
                    char pedido;
                    read(pipeVegano[0], &pedido, sizeof(char));
                    printf("Preparando menú vegano\n");
                    sleep(1);
                    write(pipeVegRecep[1], "V", sizeof(char));
                }
            } else if (i == 2) { // Proceso de Papas Fritas 1
                close(pipeFritas[1]);
                close(pipeFritasRecep[0]);
                while (1) {
                    // Preparar papas fritas
                    char pedido;
                    read(pipeFritas[0], &pedido, sizeof(char));
                    printf("Preparando papas fritas\n");
                    sleep(1);
                    write(pipeFritasRecep[1], "P", sizeof(char));
                }
            } else if (i == 3) { // Proceso de Papas Fritas 2
                close(pipeFritas[1]);
                close(pipeFritasRecep[0]);
                while (1) {
                    // Preparar papas fritas
                    char pedido;
                    read(pipeFritas[0], &pedido, sizeof(char));
                    printf("Preparando papas fritas\n");
                    sleep(1);
                    write(pipeFritasRecep[1], "P", sizeof(char));
                }
            } else if (i == 4) { // Proceso de Distribucion
                char pedido;
                close(pipeDistribucion[1]);
                while (read(pipeClientesVIP[0], &pedido, sizeof(Cliente)) > 0) {
                    // Hace falta cerrar el del hijo?
                    for (int j = 0; pedido != '\0'; j++) {
                        char pedido = clientes[i].pedido[j];
                        if (pedido == 'H') {
                            write(pipeHamburguesas[1], &pedido, sizeof(char));
                        } else if (pedido == 'V') {
                            write(pipeVegano[1], &pedido, sizeof(char));
                        } else if (pedido == 'P') {
                            write(pipeFritas[1], &pedido, sizeof(char));
                        }
                    }
                }
            }
            exit(0);
        }
    }

    // Proceso principal
    // Revisar que se cierran todos
    close(pipeHamburguesas[0]);
    close(pipeVegano[0]);
    close(pipeFritas[0]);
    close(pipeDistribucion[0]);
    close(pipeHamRecep[1]);
    close(pipeVegRecep[1]);
    close(pipeFritasRecep[1]);
    close(pipeClientes[0]);
    close(pipeClientesVIP[0]);

    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        wait(NULL);
    }

    return 0;
}