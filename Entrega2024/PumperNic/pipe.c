#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NUM_EMPLOYEES 5
#define NUM_CLIENTES 5
#define TAMAÑO_PEDIDO 4
#define PACIENCIA 3

typedef struct {
    char pedido[TAMAÑO_PEDIDO]; // Combinación de 'H', 'V', 'P'
    int esVIP;
    int paciencia;
} Cliente;

Cliente cliente;
char comida; // Para individualizar cada comida

int pipeHamburguesas[2], pipeVegano[2], pipeFritas[2],
pipeClientes[2], pipeClientesVIP[2],
pipeHamRecep[2], pipeVegRecep[2], pipeFritasRecep[2];

void generarPedidos(Cliente* cliente) {
    srand(time(NULL) + getpid()); // Seed random para que cada cliente sea distinto
    int numPedidos = rand() % 3 + 1; // Entre 1 y 3 comidas
    for (int j = 0; j < numPedidos; j++) {
        int tipoPedido = rand() % 3;
        cliente->pedido[j] = (tipoPedido == 0) ? 'H' : (tipoPedido == 1) ? 'V' : 'P';
    }
    cliente->pedido[numPedidos] = '\0'; // Terminar el string
    cliente->esVIP = rand() % 2; // 50% de ser VIP
    cliente->paciencia = rand() % 10; // si cliente.paciencia > PACIENCIA entra en la fila
}

void clientes(int i){
    generarPedidos(&cliente);
            
    printf(ANSI_COLOR_RESET"[Cliente %d] Pedido: %s %s\n", i, cliente.pedido, cliente.esVIP ? "(VIP)" : "" ); //el reset del principio es porque puede puntintarse de rojo
    fflush(stdout);   
    
    // Se mete en su cola correspondiente
    if (cliente.esVIP) {
        close(pipeClientesVIP[0]);
        write(pipeClientesVIP[1], &cliente, sizeof(Cliente));
    } else if (cliente.paciencia > PACIENCIA) {
        close(pipeClientes[0]);
        write(pipeClientes[1], &cliente, sizeof(Cliente));
    } else {
        printf(ANSI_COLOR_RED"[Cliente %d] se fue por falta de paciencia.\n"ANSI_COLOR_RESET, i);
        fflush(stdout);
        exit(0); // Cliente se va y no regresa
    }

    // Espera de su pedido
    // Verifica que tiene su pedido para luego esperar cada cosa
    int tieneH = 0, tieneV = 0, tieneP = 0;
    for (int j = 0; cliente.pedido[j] != '\0'; j++) {
        if (cliente.pedido[j] == 'H') tieneH++;
        else if (cliente.pedido[j] == 'V') tieneV++;
        else if (cliente.pedido[j] == 'P') tieneP++;
    }

    // Espera a que su pedido este listo
    close(pipeHamRecep[1]); close(pipeVegRecep[1]); close(pipeFritasRecep[1]);
    while (tieneH > 0 || tieneV > 0 || tieneP > 0) {
        sleep(2);
        char respuestaH, respuestaV, respuestaP;
        if (tieneH > 0 && read(pipeHamRecep[0], &respuestaH, sizeof(char)) > 0) {
            tieneH--;
        }
        if (tieneV > 0 && read(pipeVegRecep[0], &respuestaV, sizeof(char)) > 0) {
            tieneV--;
        }
        if (tieneP > 0 && read(pipeFritasRecep[0], &respuestaP, sizeof(char)) > 0) {
            tieneP--;
        }
    }
    printf(ANSI_COLOR_GREEN"                        >>>>>>[Cliente %d] se fue con su pedido<<<<<<\n"ANSI_COLOR_RESET, i);
}

void cocinero_hamburguesas(){
    while (1) {
        // Preparar hamburguesa
        read(pipeHamburguesas[0], &comida, sizeof(char));
        sleep(1);
        write(pipeHamRecep[1], "H", sizeof(char));
    }
}

void cocinero_vegano(){
    while (1) {
        // Preparar menú vegano
        read(pipeVegano[0], &comida, sizeof(char));
        sleep(1);
        write(pipeVegRecep[1], "V", sizeof(char));
    }
}

void cocinero_papas(int id){
    while (1) {
        // Preparar papas fritas
        read(pipeFritas[0], &comida, sizeof(char));
        sleep(1);
        write(pipeFritasRecep[1], "P", sizeof(char));
    }
}

void administrador(){
    while (1) {
        // Revisa si hay VIP primero
        while(read(pipeClientesVIP[0], &cliente, sizeof(Cliente)) > 0){
            printf(ANSI_COLOR_MAGENTA"      [ADMIN] tiene el pedido VIP: %s\n"ANSI_COLOR_RESET, cliente.pedido);
            fflush(stdout);
            for (int j = 0; cliente.pedido[j] != '\0'; j++) {
                char comida = cliente.pedido[j];
                if (comida == 'H') {
                    write(pipeHamburguesas[1], &comida, sizeof(char));
                } else if (comida == 'V') {
                    write(pipeVegano[1], &comida, sizeof(char));
                } else if (comida == 'P') {
                    write(pipeFritas[1], &comida, sizeof(char));
                }
            }
            sleep(2);
        } 
        // Una vez que no hay VIP mira si hay normales
        if (read(pipeClientes[0], &cliente, sizeof(Cliente)) > 0) {
            printf(ANSI_COLOR_MAGENTA"      [ADMIN] tiene el pedido: %s\n"ANSI_COLOR_MAGENTA, cliente.pedido);
            fflush(stdout);
            for (int j = 0; cliente.pedido[j] != '\0'; j++) {
                char comida = cliente.pedido[j];
                if (comida == 'H') {
                    write(pipeHamburguesas[1], &comida, sizeof(char));
                } else if (comida == 'V') {
                    write(pipeVegano[1], &comida, sizeof(char));
                } else if (comida == 'P') {
                    write(pipeFritas[1], &comida, sizeof(char));
                }
            }    
            sleep(2);                        
        }
        sleep(1);
    }
}

int main() {
    pipe(pipeHamburguesas);
    pipe(pipeVegano);
    pipe(pipeFritas);
    pipe(pipeHamRecep);
    pipe(pipeVegRecep);
    pipe(pipeFritasRecep);
    pipe(pipeClientes);
    pipe(pipeClientesVIP);

    if (pipe(pipeHamburguesas) == -1 || 
        pipe(pipeVegano) == -1 || 
        pipe(pipeFritas) == -1 || 
        pipe(pipeClientes) == -1 || 
        pipe(pipeClientesVIP) == -1 || 
        pipe(pipeHamRecep) == -1 || 
        pipe(pipeVegRecep) == -1 || 
        pipe(pipeFritasRecep) == -1) {
        perror("Error al crear uno de los pipes");
        exit(EXIT_FAILURE);
    }

    // Configurar los pipes en modo no bloqueante
    fcntl(pipeClientesVIP[0], F_SETFL, O_NONBLOCK);
    //fcntl(pipeClientes[0], F_SETFL, O_NONBLOCK);
    // El pipe de clintes comunes no se hace No bloque para evitar espera ocupada

    // Crear procesos de Clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        int pidCli = fork();
        if (pidCli < 0) {
            perror("Error al crear proceso Cliente");
            exit(EXIT_FAILURE);
        } else if (pidCli == 0) {
            close(pipeHamburguesas[0]);
            close(pipeHamburguesas[1]);
            close(pipeVegano[0]);
            close(pipeVegano[1]);
            close(pipeFritas[0]);
            close(pipeFritas[1]);
            
            close(pipeClientes[0]);
            close(pipeClientesVIP[0]);
            close(pipeHamRecep[1]);
            close(pipeVegRecep[1]);
            close(pipeFritasRecep[1]);
            clientes(i);
            exit(0);
        }
    }
    
    // Crear procesos empleados
    // Proceso de Hamburguesas
    pid_t cocinero_ham_p = fork();
    if (cocinero_ham_p == 0) { 
        close(pipeVegano[0]);
        close(pipeVegano[1]);
        close(pipeFritas[0]);
        close(pipeFritas[1]);
        close(pipeClientes[0]);
        close(pipeClientes[1]);
        close(pipeClientesVIP[0]);
        close(pipeClientesVIP[1]);
        close(pipeVegRecep[0]);
        close(pipeVegRecep[1]);
        close(pipeFritasRecep[0]);
        close(pipeFritasRecep[1]);
        
        close(pipeHamburguesas[1]);
        close(pipeHamRecep[0]);
        cocinero_hamburguesas();
        exit(0);
    }
    // Proceso de Menu Vegano
    pid_t cocinero_veg_p = fork(); 
    if (cocinero_veg_p == 0) {
        close(pipeHamburguesas[0]);
        close(pipeHamburguesas[1]);
        close(pipeFritas[0]);
        close(pipeFritas[1]);
        close(pipeClientes[0]);
        close(pipeClientes[1]);
        close(pipeClientesVIP[0]);
        close(pipeClientesVIP[1]);
        close(pipeHamRecep[0]);
        close(pipeHamRecep[1]);
        close(pipeFritasRecep[0]);
        close(pipeFritasRecep[1]);

        close(pipeVegano[1]);
        close(pipeVegRecep[0]);
        cocinero_vegano(1);
        exit(0);
    }
    // Proceso de Papas Fritas 1
    pid_t cocinero_fritas1_p = fork();
    if (cocinero_fritas1_p == 0) { 
        close(pipeHamburguesas[0]);
        close(pipeHamburguesas[1]);
        close(pipeVegano[0]);
        close(pipeVegano[1]);
        close(pipeClientes[0]);
        close(pipeClientes[1]);
        close(pipeClientesVIP[0]);
        close(pipeClientesVIP[1]);
        close(pipeHamRecep[0]);
        close(pipeHamRecep[1]);
        close(pipeVegRecep[0]);
        close(pipeVegRecep[1]);

        close(pipeFritas[1]);
        close(pipeFritasRecep[0]);
        cocinero_papas(1);
        exit(0);
    }
    // Proceso de Papas Fritas 2
    pid_t cocinero_fritas2_p = fork();
    if (cocinero_fritas2_p == 0) { 
        close(pipeHamburguesas[0]);
        close(pipeHamburguesas[1]);
        close(pipeVegano[0]);
        close(pipeVegano[1]);
        close(pipeClientes[0]);
        close(pipeClientes[1]);
        close(pipeClientesVIP[0]);
        close(pipeClientesVIP[1]);
        close(pipeHamRecep[0]);
        close(pipeHamRecep[1]);
        close(pipeVegRecep[0]);
        close(pipeVegRecep[1]);

        close(pipeFritas[1]);
        close(pipeFritasRecep[0]);
        cocinero_papas(2);
        exit(0);
    }
    // Proceso de Distribucion
    pid_t admin_p = fork(); 
    if (admin_p == 0) { 
        close(pipeHamRecep[0]);
        close(pipeHamRecep[1]);
        close(pipeVegRecep[0]);
        close(pipeVegRecep[1]);
        close(pipeFritasRecep[0]);
        close(pipeFritasRecep[1]);

        close(pipeClientesVIP[1]); close(pipeClientes[1]);
        close(pipeHamburguesas[0]); close(pipeVegano[0]); close(pipeFritas[0]);
        administrador();
        exit(0);
    }

    // --- no llega a ejecutarse esta parte por el pipe bloqueadnte del Admin
    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES + NUM_CLIENTES; i++) {
        wait(NULL);
    }

    kill(cocinero_ham_p, SIGKILL);
    kill(cocinero_veg_p, SIGKILL);
    kill(cocinero_fritas1_p, SIGKILL);
    kill(cocinero_fritas2_p, SIGKILL);
    kill(admin_p, SIGKILL);

    return 0;
}