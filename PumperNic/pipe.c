#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>


#define NUM_EMPLOYEES 5
#define NUM_CLIENTES 2
#define COLA_CLIENTES 10


// Esta bien?
sem_t cola_normal, cola_vip;


typedef struct {
    char pedido[4]; // Combinación de 'H', 'V', 'P'
    int esVIP;
} Cliente;


void generarPedidos1(Cliente* cliente) {
    srand(time(NULL) + getpid()); // Seed random number generator
    int numPedidos = rand() % 3 + 1; // Between 1 and 3 items
    for (int j = 0; j < numPedidos; j++) {
        int tipoPedido = rand() % 3;
        cliente->pedido[j] = (tipoPedido == 0) ? 'H' : (tipoPedido == 1) ? 'V' : 'P';
    }
    cliente->pedido[numPedidos] = '\0'; // Null-terminate the string
    cliente->esVIP = rand() % 2; // 50% chance of being VIP
}


int main() {
    int pipeHamburguesas[2], pipeVegano[2], pipeFritas[2], pipeDistribucion[2],
    pipeClientes[2], pipeClientesVIP[2],
    pipeHamRecep[2], pipeVegRecep[2], pipeFritasRecep[2];


    pid_t pid;
    pid_t pidCli;
   
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
    
    Cliente cliente;


    // Crear procesos clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pidCli = fork();
        if (pidCli < 0) {
            perror("Error al crear proceso Cliente");
            exit(1);
        } else if (pidCli == 0) {
            // Proceso hijo
            generarPedidos1(&cliente);
           
            printf("[Cliente %d] Pedido: %s %s\n", i, cliente.pedido, cliente.esVIP ? "(VIP)" : "");


            // Se mete en su cola correspondiente
            if (cliente.esVIP && sem_trywait(&cola_vip) == 0) { // Si es VIP
                sem_wait(&cola_vip);
                close(pipeClientesVIP[0]);
                write(pipeClientesVIP[1], &cliente, sizeof(Cliente));
            } else if (sem_trywait(&cola_normal) == 0) { // SI no es VIP es NORMAL
                sem_wait(&cola_normal);
                close(pipeClientes[0]);
                write(pipeClientes[1], &cliente, sizeof(Cliente));
            } else{
                cliente.esVIP ? printf("        [Cliente %d] se fue. Cola VIP llena\n", i) : printf("        [Cliente %d] se fue. Cola NORMAL llena\n", i);
                exit(0);
            }


            // Espera su pedido
            // Verifica que tiene su pedido para luego esperar cada cosa
            int tieneH = 0, tieneV = 0, tieneP = 0;
            for (int j = 0; cliente.pedido[j] != '\0'; j++) {
                if (cliente.pedido[j] == 'H') tieneH++;
                else if (cliente.pedido[j] == 'V') tieneV++;
                else if (cliente.pedido[j] == 'P') tieneP++;
            }
            //printf("El cliente: %d tiene: H: %d, V: %d, P: %d <<<<<<<<<<<<<<<\n", i,tieneH, tieneV, tieneP);
       
            // Espera a que su pedido este listo
            close(pipeHamRecep[1]); close(pipeVegRecep[1]); close(pipeFritasRecep[1]);
            // tendria que ser un read solo creo--------
            while (tieneH > 0 || tieneV > 0 || tieneP > 0) {
                printf("                [Clinete: %d] esperando ...\n", i);
                char respuestaH, respuestaV, respuestaP;
                read(pipeHamRecep[0], &respuestaH, sizeof(char));
                read(pipeHamRecep[0], &respuestaV, sizeof(char));
                read(pipeHamRecep[0], &respuestaP, sizeof(char));
                if (respuestaH == 'H' && tieneH > 0) {
                    tieneH--;
                    printf("                        [Cliente %d] recibio Ham\n", i);
                } 
                if (respuestaV == 'V' && tieneV > 0) {
                    tieneV--;
                    printf("                        [Cliente %d:] recibio Veg\n", i);
                } 
                if (respuestaP == 'P' && tieneP > 0) {
                    tieneP--;
                    printf("                        [Cliente %d] recibio Fritas\n", i);
                }
                sleep(2);
            }
            cliente.esVIP ? sem_post(&cola_vip) : sem_post(&cola_normal);
            printf("                        >>>>>>[Cliente %d] se fue con su pedido<<<<<<\n", i);
            exit(0);
        }
    }
    
    char pedido;
    // Crear procesos empleados
    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Error al crear proceso Empledos");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            // Código para cada proceso hijo
            if (i == 0) { // Proceso de Hamburguesas
                printf("    -- se crea Empleado 0\n");
                close(pipeHamburguesas[1]);
                while (1) {
                    // Preparar hamburguesa
                    printf("    ------ (empleado hamburguesas)\n");
                    read(pipeHamburguesas[0], &pedido, sizeof(char));
                    printf("    [Empleado 0] Preparando hamburguesa\n");
                    fflush(stdout);
                    //sleep(1);
                    write(pipeHamRecep[1], "H", sizeof(char));
                }
            } else if (i == 1) { // Proceso de Menú Vegano
                printf("    -- se crea Empleado 1\n");
                close(pipeVegano[1]);
                while (1) {
                    // Preparar menú vegano
                    printf("    ------ (empleado vegano)\n");
                    read(pipeVegano[0], &pedido, sizeof(char));
                    printf("    [Empleado 1] Preparando menú vegano\n");
                    fflush(stdout);
                    //sleep(1);
                    write(pipeVegRecep[1], "V", sizeof(char));
                }
                exit(0);
            } else if (i == 2) { // Proceso de Papas Fritas 1
                printf("    -- se crea Empleado 2\n");
                close(pipeFritas[1]);
                while (1) {
                    // Preparar papas fritas
                    printf("    ------ (empleado fritas 1)\n");
                    read(pipeFritas[0], &pedido, sizeof(char));
                    printf("    [Empleado 2] Preparando papas fritas\n");
                    fflush(stdout);
                    //sleep(1);
                    write(pipeFritasRecep[1], "P", sizeof(char));
                }
            } else if (i == 3) { // Proceso de Papas Fritas 2
                printf("    -- se crea Empleado 3\n");
                close(pipeFritas[1]);
                while (1) {
                    // Preparar papas fritas
                    printf("    ------ (empleado fritas 2)\n");
                    read(pipeFritas[0], &pedido, sizeof(char));
                    printf("    [Empleado 3] Preparando papas fritas\n");
                    fflush(stdout);
                    //sleep(1);
                    write(pipeFritasRecep[1], "P", sizeof(char));
                }
            } else if (i == 4) { // Proceso de Distribucion
                printf("    -- se crea Empleado 4\n");
                close(pipeClientesVIP[1]); close(pipeClientes[1]);
                close(pipeHamburguesas[0]); close(pipeVegano[0]); close(pipeFritas[0]);
                while (1) {
                    if (read(pipeClientesVIP[0], &cliente, sizeof(Cliente)) > 0){
                        printf("    ------ (empleado admin vip)\n");
                        pedido = cliente.pedido;
                        for (int j = 0; pedido[j] != '\0'; j++) {
                            char comida = cliente.pedido[j];
                            printf("    [Admin] distribuye %c\n", comida);
                            if (comida == 'H') {
                                write(pipeHamburguesas[1], &comida, sizeof(char));
                            } else if (comida == 'V') {
                                write(pipeVegano[1], &comida, sizeof(char));
                            } else if (comida == 'P') {
                                write(pipeFritas[1], &comida, sizeof(char));
                            }
                            // quitarle la letra en posicion j al pedido

                        }
                        printf("[Admin] Distribuyo un pedido NORMAL\n");
                    } else if (read(pipeClientes[0], &cliente, sizeof(Cliente)) > 0) {
                        printf("    ------ (empleado admin normal)\n");                    
                        pedido = cliente.pedido;
                        for (int j = 0; pedido[j] != '\0'; j++) {
                            char comida = cliente.pedido[j];
                            printf("    [Admin] distribuye %c\n", comida);
                            if (comida == 'H') {
                                write(pipeHamburguesas[1], &comida, sizeof(char));
                            } else if (comida == 'V') {
                                write(pipeVegano[1], &comida, sizeof(char));
                            } else if (comida == 'P') {
                                write(pipeFritas[1], &comida, sizeof(char));
                            }
                        }
                        printf("[Admin] Distribuyo un pedido NORMAL\n");
                    }
                }
                exit(0);
            }
        }
    }


    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES + NUM_CLIENTES; i++) {
        wait(NULL);
    }


    return 0;
}
