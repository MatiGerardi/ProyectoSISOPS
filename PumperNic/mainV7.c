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

void* empleadoPapas1(void* arg) {
    char pedido;
    while (read(pipePapas1[0], &pedido, sizeof(char)) > 0) {
        // Procesar pedido
        sem_post(&semEmpleado1); // Marcar como libre
    }
    return NULL;
}

void* empleadoPapas2(void* arg) {
    char pedido;
    while (read(pipePapas2[0], &pedido, sizeof(char)) > 0) {
        // Procesar pedido
        sem_post(&semEmpleado2); // Marcar como libre
    }
    return NULL;
}

int main() {
    int pipeHamburguesas[2], pipeVegano[2], pipePapas1[2], pipePapas2[2], pipeDespacho[2];
    pid_t pid[NUM_EMPLOYEES];
    Cliente clientes[NUM_CLIENTES];

    sem_t semEmpleado1, semEmpleado2;
    pthread_t threadEmpleado1, threadEmpleado2;

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

    // Inicializar semáforos
    sem_init(&semEmpleado1, 0, 1); // Empleado 1 libre
    sem_init(&semEmpleado2, 0, 1); // Empleado 2 libre

    // Crear hilos
    pthread_create(&threadEmpleado1, NULL, empleadoPapas1, NULL);
    pthread_create(&threadEmpleado2, NULL, empleadoPapas2, NULL);


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
            } else if (pedido == 'P') {
                if (sem_trywait(&semEmpleado1) == 0) {
                    write(pipePapas1[1], &pedido, sizeof(char));
                } else if (sem_trywait(&semEmpleado2) == 0) {
                    write(pipePapas2[1], &pedido, sizeof(char));
                } else {
                    printf("Ambos empleados de papas fritas están ocupados. Reintentando...\n");
                    j--; // Reintentar el mismo pedido
                    continue;
                }
            }
        }
    }

    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        wait(NULL);
    }

    // Esperar a que terminen los hilos
    pthread_join(threadEmpleado1, NULL);
    pthread_join(threadEmpleado2, NULL);

    // Destruir semáforos
    sem_destroy(&semEmpleado1);
    sem_destroy(&semEmpleado2);

    return 0;
}