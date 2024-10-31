#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NUM_EMPLOYEES 5
#define NUM_CLIENTES 10
#define COLA_CLIENTES 2
#define TAMAÑO_PEDIDO 4

#define CLIENTE_COMUN 1
#define CLIENTE_VIP 2
#define HAMBURGUESA 3
#define VEGANO 4
#define PAPAS_FRITAS 5

typedef struct {
    char pedido[TAMAÑO_PEDIDO]; // Combinación de 'H', 'V', 'P'
    int esVIP;
} Cliente;

struct msgbuf {
    long type;
    char text[TAMAÑO_PEDIDO];
};

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

int main() {
    int msgidHamburguesas, msgidVegano, msgidFritas, msgidClientes, msgidClientesVIP; // id colas
    int msgidHamRecep, msgidVegRecep, msgidFritasRecep;
    key_t keyHamburguesas = 1234, keyVegano = 1235, keyFritas = 1236, keyClientes = 1238, keyClientesVIP = 1239;
    key_t keyHamR = 1244, keyVegR = 1245, keyFritasR = 1246;
    struct msgbuf message; // mensaje a recibir
    struct msgbuf messageS; // mensaje a enviar
    strcpy(messageS.text, "");
    
    // Crear colas de mensajes
    msgidHamburguesas = msgget(keyHamburguesas, IPC_CREAT | 0666);
    msgidVegano = msgget(keyVegano, IPC_CREAT | 0666);
    msgidFritas = msgget(keyFritas, IPC_CREAT | 0666);
    msgidClientes = msgget(keyClientes, IPC_CREAT | 0666);
    msgidClientesVIP = msgget(keyClientesVIP, IPC_CREAT | 0666);
    msgidHamRecep = msgget(keyHamR, IPC_CREAT | 0666);
    msgidVegRecep = msgget(keyVegR, IPC_CREAT | 0666);
    msgidFritasRecep = msgget(keyFritasR, IPC_CREAT | 0666);

    sem_t *cola_normal, *cola_vip;
    // Desvincular semáforos si existen (aseguro la inicialización)
    sem_unlink("/cola_normal");
    sem_unlink("/cola_vip");
    if (sem_unlink("/cola_normal") == -1) perror("Error al desvincular semáforo /cola_normal");
    if (sem_unlink("/cola_vip") == -1) perror("Error al desvincular semáforo /cola_vip");

    // Semaforos compartidos
    cola_normal = sem_open("/cola_normal", O_CREAT, 0644, COLA_CLIENTES);
    if (cola_normal == SEM_FAILED) {
        perror("Error al abrir el semáforo /cola_normal");
        exit(EXIT_FAILURE);
    }

    cola_vip = sem_open("/cola_vip", O_CREAT, 0644, COLA_CLIENTES);
    if (cola_vip == SEM_FAILED) {
        perror("Error al abrir el semáforo /cola_vip");
        sem_close(cola_normal);
        sem_unlink("/cola_normal");
        exit(EXIT_FAILURE);
    }
    
    // VACIAR COLAS PARA ASEGURAR - (si la ejecucion se cortaba quedaban en momoria)
    while (msgrcv(msgidHamburguesas, &message, sizeof(message.text), HAMBURGUESA, IPC_NOWAIT) != -1) {
        //~ printf("SALE: %s\n",message.text);
    }
    while (msgrcv(msgidVegano, &message, sizeof(message.text), VEGANO, IPC_NOWAIT) != -1) {
        //~ printf("SALE: %s\n",message.text);
    }
    while (msgrcv(msgidFritas, &message, sizeof(message.text), PAPAS_FRITAS, IPC_NOWAIT) != -1) {
        //~ printf("SALE: %s\n",message.text);
    }
    while (msgrcv(msgidHamRecep, &message, sizeof(message.text), HAMBURGUESA, IPC_NOWAIT) != -1) {
        //~ printf("SALE: %s\n",message.text);
    }
    while (msgrcv(msgidVegRecep, &message, sizeof(message.text), VEGANO, IPC_NOWAIT) != -1) {
        //~ printf("SALE: %s\n",message.text);
    }
    while (msgrcv(msgidFritasRecep, &message, sizeof(message.text), PAPAS_FRITAS, IPC_NOWAIT) != -1) {
        //~ printf("SALE: %s\n",message.text);
    }
    while (msgrcv(msgidClientesVIP, &message, sizeof(message.text), CLIENTE_VIP, IPC_NOWAIT) != -1) {
        //~ printf("SALE: %s\n",message.text);
    }
    while (msgrcv(msgidClientes, &message, sizeof(message.text), CLIENTE_COMUN, IPC_NOWAIT) != -1) {
        //~ printf("SALE: %s\n",message.text);
    }

    Cliente cliente;
    char pedido[4];

    // Crear procesos clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pid_t pidCli = fork();
        if (pidCli < 0) {
            perror("Error al crear proceso Cliente");
            exit(EXIT_FAILURE);
        } else if (pidCli == 0) {// Proceso hijo
            generarPedidos1(&cliente);
           
            printf(ANSI_COLOR_RESET"[Cliente %d] Pedido: %s %s\n", i, cliente.pedido, cliente.esVIP ? "(VIP)" : "" );
            fflush(stdout);
            
            // Se meete en su cola corresponiente
            while(1) {
                if (cliente.esVIP && sem_trywait(cola_vip) == 0){
                    messageS.type = CLIENTE_VIP;
                    strcpy(messageS.text, cliente.pedido);
                    msgsnd(msgidClientesVIP, &messageS, sizeof(messageS.text), 0);
                    break;
                } else if (!cliente.esVIP && sem_trywait(cola_normal) == 0) { // SI no es VIP es NORMAL
                    messageS.type = CLIENTE_COMUN;
                    strcpy(messageS.text, cliente.pedido);
                    msgsnd(msgidClientes, &messageS, sizeof(messageS.text), 0);
                    break;
                } else{
                    cliente.esVIP ? printf(ANSI_COLOR_RED"X[Cliente %d] se fue. Cola VIP llena\n"ANSI_COLOR_RESET, i) : printf(ANSI_COLOR_RED"X[Cliente %d] se fue. Cola NORMAL llena\n"ANSI_COLOR_RESET, i);
                    fflush(stdout);
                    sleep(3);
                    cliente.esVIP ? printf(ANSI_COLOR_YELLOW"-->[Cliente %d] vuele mas tarde. Cola VIP\n"ANSI_COLOR_RESET, i) : printf(ANSI_COLOR_YELLOW"-->[Cliente %d] vuelve mas tarde. Cola NORMAL\n"ANSI_COLOR_RESET, i);
                    fflush(stdout);
                }
            }

            // Espera su pedido
            // Verifica que tiene su pedido para luego esperar cada cosa
            int tieneH = 0, tieneV = 0, tieneP = 0;
            for (int j = 0; cliente.pedido[j] != '\0'; j++) {
                if (cliente.pedido[j] == 'H') tieneH++;
                else if (cliente.pedido[j] == 'V') tieneV++;
                else if (cliente.pedido[j] == 'P') tieneP++;
            }
            //~ printf("El cliente: %d tiene: H: %d, V: %d, P: %d <<<<<<<<<<<<<<<\n", i,tieneH, tieneV, tieneP);
       
            // Espera a que su pedido este listo
            while (tieneH > 0 || tieneV > 0 || tieneP > 0) {
                //~ printf("                [Clinete: %d] esperando ...\n", i);
                sleep(2);
                if (tieneH > 0 && msgrcv(msgidHamRecep, &message, sizeof(message.text), HAMBURGUESA, IPC_NOWAIT) != -1) {
                    tieneH--;
                    //~ printf("<<<                        [Cliente %dH] recibio: %s\n", i, message.text);
                }
                if (tieneV > 0 && msgrcv(msgidVegRecep, &message, sizeof(message.text), VEGANO, IPC_NOWAIT) != -1) {
                    tieneV--;
                    //~ printf("<<<                        [Cliente %dV] recibio: %s\n", i, message.text);
                }
                if (tieneP > 0 && msgrcv(msgidFritasRecep, &message, sizeof(message.text), PAPAS_FRITAS, IPC_NOWAIT) != -1) {
                    tieneP--;
                    //~ printf("<<<                        [Cliente %dP] recibio: %s\n", i, message.text);
                }
            }
            cliente.esVIP ? sem_post(cola_vip) : sem_post(cola_normal);
            printf(ANSI_COLOR_GREEN"                        >>>>>>[Cliente %d] se fue con su pedido<<<<<<\n" ANSI_COLOR_RESET, i);
            exit(0);
        }
    }

    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // Codigo para cada proceso hijo
            if (i == 0) { // Proceso de Hamburguesas
                while (1) {
                    msgrcv(msgidHamburguesas, &message, sizeof(message.text), HAMBURGUESA, 0);
                    //~ printf("    [Empleado 0] Preparando hamburguesa: %s\n", message.text);
                    //~ fflush(stdout);
                    sleep(1);
                    messageS.type = HAMBURGUESA;
                    strcpy(messageS.text, "H");
                    msgsnd(msgidHamRecep, &messageS, sizeof(message.text), 0);
                }
            } else if (i == 1) { // Proceso Vegano
                while (1) {
                    msgrcv(msgidVegano, &message, sizeof(message.text), VEGANO, 0);
                    //~ printf("    [Empleado 1] Preparando vegano: %s\n", message.text);
                    //~ fflush(stdout);
                    sleep(1);
                    messageS.type = VEGANO;
                    strcpy(messageS.text, "V");
                    msgsnd(msgidVegRecep, &messageS, sizeof(message.text), 0);
                }
            } else if (i == 2) { // Proceso de Papas Fritas
                while (1) {
                    msgrcv(msgidFritas, &message, sizeof(message.text), PAPAS_FRITAS, 0);
                    //~ printf("    [Empleado 2] Preparando papas fritas: %s\n", message.text);
                    //~ fflush(stdout);
                    sleep(1);
                    messageS.type = PAPAS_FRITAS;
                    strcpy(messageS.text, "P");
                    msgsnd(msgidFritasRecep, &messageS, sizeof(message.text), 0);
                }
            }else if (i == 3) { // Proceso de Papas Fritas
                while (1) {
                    msgrcv(msgidFritas, &message, sizeof(message.text), PAPAS_FRITAS, 0);
                    //~ printf("    [Empleado 3] Preparando papas fritas\n");
                    //~ fflush(stdout);
                    sleep(1);
                    messageS.type = PAPAS_FRITAS;
                    strcpy(messageS.text, "H");
                    msgsnd(msgidFritasRecep, &messageS, sizeof(message.text), 0);
                }
            } else if (i == 4) { // Proceso de Distribucion
                //~ printf("    -- se crea Empleado 4\n");
                while (1) {
                    while (msgrcv(msgidClientesVIP, &message, sizeof(cliente.pedido), CLIENTE_VIP, IPC_NOWAIT) != -1) {
                        //~ printf("    ------ (empleado admin vip)\n");
                        printf(ANSI_COLOR_MAGENTA"      [ADMIN] tiene el pedidoVIP: %s\n"ANSI_COLOR_RESET, message.text);
                        fflush(stdout);
                        strcpy(pedido, message.text);
                        for (int j = 0; pedido[j] != '\0'; j++) {
                            char comida = pedido[j];
                            //~ strcpy(messageS.text, comida);
                            if (comida == 'H') {   
                                messageS.type = HAMBURGUESA;
                                strcpy(messageS.text, "H");                            
                                msgsnd(msgidHamburguesas, &messageS, sizeof(message.text), 0);
                                //~ printf("    [Admin] distribuye %c\n", comida);
                            } else if (comida == 'V') {
                                messageS.type = VEGANO;
                                strcpy(messageS.text, "V");  
                                msgsnd(msgidVegano, &messageS, sizeof(message.text), 0);
                                //~ printf("    [Admin] distribuye %c\n", comida);
                            } else if (comida == 'P') {
                                messageS.type = PAPAS_FRITAS;
                                strcpy(messageS.text, "P");  
                                msgsnd(msgidFritas, &messageS, sizeof(message.text), 0);
                                //~ printf("    [Admin] distribuye %c\n", comida);
                            }
                        }
                        //~ printf("[Admin] Distribuyo un pedido VIP\n");
                    }
                    if (msgrcv(msgidClientes, &message, sizeof(cliente.pedido), CLIENTE_COMUN, IPC_NOWAIT) != -1) {
                        //~ printf("    ------ (empleado admin normal)\n");
                        printf(ANSI_COLOR_MAGENTA"      [ADMIN] tiene el pedido: %s\n" ANSI_COLOR_RESET, message.text);
                        fflush(stdout);
                        strcpy(pedido, message.text);
                        for (int j = 0; pedido[j] != '\0'; j++) {
                            char comida = pedido[j];
                            //~ strcpy(messageS.text, comida);
                            if (comida == 'H') {   
                                messageS.type = HAMBURGUESA;
                                strcpy(messageS.text, "H");                          
                                msgsnd(msgidHamburguesas, &messageS, sizeof(message.text), 0);
                                //~ printf("    [Admin] distribuye %c\n", comida);
                            } else if (comida == 'V') {
                                messageS.type = VEGANO;
                                strcpy(messageS.text, "V");
                                msgsnd(msgidVegano, &messageS, sizeof(message.text), 0);
                                //~ printf("    [Admin] distribuye %c\n", comida);
                            } else if (comida == 'P') {
                                messageS.type = PAPAS_FRITAS;
                                strcpy(messageS.text, "P");
                                msgsnd(msgidFritas, &messageS, sizeof(message.text), 0);
                                //~ printf("    [Admin] distribuye %c\n", comida);
                            }
                        }
                        //~ printf("[Admin] Distribuyo un pedido NORMAL\n");
                    }
                }
                exit(0);
            }
        }
    }
    
    // --- no llega a ejecutarse esta parte por el while(1) del Admin
    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES+NUM_CLIENTES; i++) {
        wait(NULL);
    }

    // Eliminar colas de mensajes
    msgctl(msgidHamburguesas, IPC_RMID, NULL);
    msgctl(msgidVegano, IPC_RMID, NULL);
    msgctl(msgidFritas, IPC_RMID, NULL);
    msgctl(msgidClientes, IPC_RMID, NULL);
    msgctl(msgidClientesVIP, IPC_RMID, NULL);
    msgctl(msgidHamRecep, IPC_RMID, NULL);
    msgctl(msgidVegRecep, IPC_RMID, NULL);
    msgctl(msgidFritasRecep, IPC_RMID, NULL);

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