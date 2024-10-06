#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_EMPLOYEES 5
#define NUM_CLIENTES 50

typedef struct {
    char pedido[3]; // Combinación de 'H', 'V', 'P'
    int esVIP;
} Cliente;

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
    }

}

void moverVIPsAlPrincipio(Cliente clientes[], int numClientes) {
    // Declaración de la función que toma un arreglo de clientes y el número total de clientes.
    
    int i = 0, j = numClientes - 1;
    // Inicialización de dos índices: 'i' al principio del arreglo y 'j' al final del arreglo.
    
    while (i < j) {        
        while (i < numClientes && clientes[i].esVIP) i++;
        // Incrementa 'i' hasta encontrar un cliente que no sea VIP o hasta llegar al final del arreglo.
        
        while (j >= 0 && !clientes[j].esVIP) j--;
        // Decrementa 'j' hasta encontrar un cliente que sea VIP o hasta llegar al principio del arreglo.
        
        if (i < j) {// Si 'i' es menor que 'j', intercambia los clientes en las posiciones 'i' y 'j'.
            //SWAP
            Cliente temp = clientes[i];
            clientes[i] = clientes[j];
            clientes[j] = temp;
        }
    }
}

int main() {
    int pipeHamburguesas[2], pipeVegano[2], pipePapas1[2], pipePapas2[2], pipeDespacho[2];
    int pipeStatusPapas1[2], pipeStatusPapas2[2]; // Pipes para verificar el estado de los empleados de papas fritas
    pid_t pid[NUM_EMPLOYEES];
    Cliente clientes[NUM_CLIENTES];

    // Generar pedidos de clientes
    generarPedidos(clientes);

    // Mover clientes VIP al principio del arreglo
    moverVIPsAlPrincipio(clientes, NUM_CLIENTES);

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

    for (int i = 0; i < NUM_CLIENTES; i++) {
        printf("Cliente %d: Pedido: %s %s\n", i + 1, clientes[i].pedido, clientes[i].esVIP ? "(VIP)" : "");
        for (int j = 0; clientes[i].pedido[j] != '\0'; j++) {
            char pedido = clientes[i].pedido[j];
            if (pedido == 'H') {
                write(pipeHamburguesas[1], &pedido, sizeof(char));
            } else if (pedido == 'V') {
                write(pipeVegano[1], &pedido, sizeof(char));
            } else /*if (pedido == 'P') { //Problema
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
                        j--; // Reintentar el mismo pedido
                        continue;
                    }
                }
            }*/
        }
    }

    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        wait(NULL);
    }

    return 0;
}