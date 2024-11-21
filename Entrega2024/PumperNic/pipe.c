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
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NUM_EMPLOYEES 5
#define NUM_CLIENTES 10
#define COLA_CLIENTES 1
#define TAMAÑO_PEDIDO 4

typedef struct {
    char pedido[TAMAÑO_PEDIDO]; // Combinación de 'H', 'V', 'P'
    int esVIP;
} Cliente;

Cliente cliente;
char comida; // Para individualizar cada comida

void generarPedidos1(Cliente* cliente) {
    srand(time(NULL) + getpid()); // Seed random para que cada cliente sea distinto
    int numPedidos = rand() % 3 + 1; // Entre 1 y 3 comidas
    for (int j = 0; j < numPedidos; j++) {
        int tipoPedido = rand() % 3;
        cliente->pedido[j] = (tipoPedido == 0) ? 'H' : (tipoPedido == 1) ? 'V' : 'P';
    }
    cliente->pedido[numPedidos] = '\0'; // Terminar el string
    cliente->esVIP = rand() % 2; // 50% de ser VIP
}

void cocinero_hamburguesas_simple(){
    while (1) {
        // Preparar hamburguesa
        read(pipeHamburguesas[0], &comida, sizeof(char));
        // ~ printf("            [Empleado 0] Preparando hamburguesa\n");
        fflush(stdout);
        sleep(1);
        write(pipeHamRecep[1], "H", sizeof(char));
    }
}

void cocinero_vegano(){
    while (1) {
        // Preparar menú vegano
        read(pipeVegano[0], &comida, sizeof(char));
        //~ printf("            [Empleado 1] Preparando menú vegano\n");
        //~ fflush(stdout);
        sleep(1);
        write(pipeVegRecep[1], "V", sizeof(char));
    }
}

void cocinero_papas(int id){
    while (1) {
        // Preparar papas fritas
        read(pipeFritas[0], &comida, sizeof(char));
        //~ printf("            [Empleado 3] Preparando papas fritas\n");
        //~ fflush(stdout);
        sleep(1);
        write(pipeFritasRecep[1], "P", sizeof(char));
    }
}

void administrador(){
    while (1) {
        while(read(pipeClientesVIP[0], &cliente, sizeof(Cliente)) > 0){
            //~ printf("------ (entra empleado admin vip)\n");
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
                //~ printf("    [Admin] distribuye %c\n", comida);
            }
            //~ printf("        [Admin] Distribuyo un pedido VIP\n");
        } 
        if (read(pipeClientes[0], &cliente, sizeof(Cliente)) > 0) {
            //~ printf("------ (entra empleado admin normal)\n");
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
                //~ printf("    [Admin] distribuye %c\n", comida);
            }                            
            //~ printf("        [Admin] Distribuyo un pedido NORMAL\n");
        }
    }
}

int main() {
    int pipeHamburguesas[2], pipeVegano[2], pipeFritas[2], pipeDistribucion[2],
    pipeClientes[2], pipeClientesVIP[2],
    pipeHamRecep[2], pipeVegRecep[2], pipeFritasRecep[2];
 
    sem_t *cola_normal, *cola_vip;
    
    pipe(pipeHamburguesas);
    pipe(pipeVegano);
    pipe(pipeFritas);
    pipe(pipeDistribucion);
    pipe(pipeHamRecep);
    pipe(pipeVegRecep);
    pipe(pipeFritasRecep);
    pipe(pipeClientes);
    pipe(pipeClientesVIP);

    if (pipe(pipeHamburguesas) == -1 || 
        pipe(pipeVegano) == -1 || 
        pipe(pipeFritas) == -1 || 
        pipe(pipeDistribucion) == -1 || 
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
    fcntl(pipeClientes[0], F_SETFL, O_NONBLOCK);

    // Crear procesos de Clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        int pidCli = fork();
        //~ sleep(2);
        if (pidCli < 0) {
            perror("Error al crear proceso Cliente");
            exit(EXIT_FAILURE);
        } else if (pidCli == 0) {// Proceso hijo
            generarPedidos1(&cliente);
            
            printf(ANSI_COLOR_RESET"[Cliente %d] Pedido: %s %s\n", i, cliente.pedido, cliente.esVIP ? "(VIP)" : "" ); //el reset del principio es porque puede puntintarse de rojo
            fflush(stdout);
            
            // Se mete en su cola correspondiente
            while (1) {
                if (cliente.esVIP && sem_trywait(cola_vip) == 0) { // Si es VIP
                    close(pipeClientesVIP[0]);
                    write(pipeClientesVIP[1], &cliente, sizeof(Cliente));
                    break;
                } else if (!cliente.esVIP && sem_trywait(cola_normal) == 0) { // SI no es VIP es NORMAL
                    close(pipeClientes[0]);
                    write(pipeClientes[1], &cliente, sizeof(Cliente));
                    break;
                } else{
                    cliente.esVIP ? printf(ANSI_COLOR_RED"X[Cliente %d] se fue. Cola VIP llena\n"ANSI_COLOR_RESET, i) : printf(ANSI_COLOR_RED"X[Cliente %d] se fue. Cola NORMAL llena\n"ANSI_COLOR_RESET, i);
                    fflush(stdout);
                    sleep(3);
                    cliente.esVIP ? printf(ANSI_COLOR_YELLOW"-->[Cliente %d] vuele mas tarde. Cola VIP\n"ANSI_COLOR_RESET, i) : printf(ANSI_COLOR_YELLOW"-->[Cliente %d] vuelve mas tarde. Cola NORMAL\n"ANSI_COLOR_RESET, i);
                    fflush(stdout);
                }
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
                //printf("                [Clinete: %d] esperando ...\n", i);
                sleep(2);
                char respuestaH, respuestaV, respuestaP;
                if (tieneH > 0 && read(pipeHamRecep[0], &respuestaH, sizeof(char)) > 0) {
                    tieneH--;
                    //~ printf("                        [Cliente %d] recibio Ham\n", i);
                }
                if (tieneV > 0 && read(pipeVegRecep[0], &respuestaV, sizeof(char)) > 0) {
                    tieneV--;
                    //~ printf("                        [Cliente %d:] recibio Veg\n", i);
                }
                if (tieneP > 0 && read(pipeFritasRecep[0], &respuestaP, sizeof(char)) > 0) {
                    tieneP--;
                    //~ printf("                        [Cliente %d] recibio Fritas\n", i);
                }
            }
            cliente.esVIP ? sem_post(cola_vip) : sem_post(cola_normal);
            printf(ANSI_COLOR_GREEN"                        >>>>>>[Cliente %d] se fue con su pedido<<<<<<\n"ANSI_COLOR_RESET, i);
            exit(0);
        }
    }
    
    // Crear procesos empleados
    pid_t cocinero_ham_p = fork();
    if (cocinero_ham_p == 0) { // Proceso de Hamburguesas
        close(pipeHamburguesas[1]);
        cocinero_hamburguesas_simple();
        exit(0);
    }
    pid_t cocinero_veg_p = fork(); 
    if (cocinero_veg_p == 0) { // Proceso de Menú Vegano
        close(pipeVegano[1]);
        cocinero_vegano(i);
        exit(0);
    }
    pid_t cocinero_fritas1_p = fork();
    if (cocinero_fritas1_p == 0) { // Proceso de Papas Fritas 1
        close(pipeFritas[1]);
        cocinero_papas(1);
        exit(0);
    }
    pid_t cocinero_fritas2_p = fork();
    if (cocinero_fritas2_p == 0) { // Proceso de Papas Fritas 2
        close(pipeFritas[1]);
        cocinero_papas(2);
        exit(0);
    }
    pid_t cocinero_admin_p = fork(); 
    if (cocinero_admin_p == 0) { // Proceso de Distribucion
        close(pipeClientesVIP[1]); close(pipeClientes[1]);
        close(pipeHamburguesas[0]); close(pipeVegano[0]); close(pipeFritas[0]);
        administrador();
        exit(0);
    }

    // --- no llega a ejecutarse esta parte por el while(1) del Admin
    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES + NUM_CLIENTES; i++) {
        wait(NULL);
    }
    
    // Cerrar semaforos
    sem_close(cola_normal);
    sem_close(cola_vip);
    if (sem_close(cola_normal) == -1) perror("Error al cerrar semáforo cola_normal");
    if (sem_close(cola_vip) == -1) perror("Error al cerrar semáforo cola_vip");
    sem_unlink("/cola_normal");
    sem_unlink("/cola_vip");
    if (sem_unlink("/cola_normal") == -1) perror("Error al desvincular semáforo /cola_normal");
    if (sem_unlink("/cola_vip") == -1) perror("Error al desvincular semáforo /cola_vip");

    return 0;
}
