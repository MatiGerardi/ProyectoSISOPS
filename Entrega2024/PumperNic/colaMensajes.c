#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

#define NUM_EMPLOYEES 5
#define NUM_CLIENTES 2
#define COLA_CLIENTES 10

#define CLIENTE_COMUN 1
#define CLIENTE_VIP 2
#define HAMBURGUESA 3
#define VEGANO 4
#define PAPAS_FRITAS 5

// Esta bien?
sem_t cola_normal, cola_vip;

typedef struct {
    char pedido[4];
    int esVIP;
} Cliente;

struct msgbuf {
    long type;
    char text[4];
};

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
    /*ket_t key; // llave 
    struct msgbuf message; // mensaje a encolar
    int msgid; // id de la cola

    key = ftok("/tmp", 'a');
    msgid = msgget(key_cola, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }*/

    int msgidHamburguesas, msgidVegano, msgidFritas, msgidDistribucion, msgidClientes, msgidClientesVIP; // id colas
    key_t keyHamburguesas = 1234, keyVegano = 1235, keyFritas = 1236, keyDistribucion = 1237, keyClientes = 1238, keyClientesVIP = 1239;
    struct msgbuf message; // mensaje a encolar

    // Crear colas de mensajes
    msgidHamburguesas = msgget(keyHamburguesas, IPC_CREAT | 0666);
    msgidVegano = msgget(keyVegano, IPC_CREAT | 0666);
    msgidFritas = msgget(keyFritas, IPC_CREAT | 0666);
    msgidDistribucion = msgget(keyDistribucion, IPC_CREAT | 0666);
    msgidClientes = msgget(keyClientes, IPC_CREAT | 0666);
    msgidClientesVIP = msgget(keyClientesVIP, IPC_CREAT | 0666);

    sem_init(&cola_normal, 0, COLA_CLIENTES);
    sem_init(&cola_vip, 0, COLA_CLIENTES);

    Cliente cliente;
    char pedido[4];

    // Crear procesos clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pid_t pidCli = fork();
        if (pidCli < 0) {
            perror("Error al crear proceso Cliente");
            exit(1);
        } else if (pidCli == 0) {
            // Proceso hijo
            generarPedidos1(&cliente);
           
            printf("[Cliente %d] Pedido: %s %s\n", i, cliente.pedido, cliente.esVIP ? "(VIP)" : "");
            
            if (cliente.esVIP && sem_trywait(&cola_vip) == 0){ //hacerlo con IPC_NOWAIT, si tira error es porque la cola esta llena
                message.type = CLIENTE_VIP;
                strcpy(message.text, cliente.pedido);
                msgsnd(msgidClientesVIP, &message, sizeof(char), 0); 
                sem_wait(&cola_vip);          
            } else if (sem_trywait(&cola_normal) == 0) { // SI no es VIP es NORMAL
                message.type = CLIENTE_COMUN;
                strcpy(message.text, cliente.pedido);
                msgsnd(msgidClientes, &message, sizeof(char), 0);
                sem_wait(&cola_normal);
            } else{
                cliente.esVIP ? sem_post(&cola_vip) : sem_post(&cola_normal);
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
            while (tieneH > 0 || tieneV > 0 || tieneP > 0) {
                printf("                [Clinete: %d] esperando ...\n", i);
                if (tieneH > 0 && msgrcv(msgidHamburguesas, &message, sizeof(char), HAMBURGUESA, 0) != -1) { // con 0 en vez de IPC_NOWAIT se bloquea hasta que llegue un mensaje del tipo
                    tieneH--;
                    printf("                        [Cliente %d] recibio Ham\n", i);
                }
                if (tieneV > 0 && msgrcv(msgidVegano, &message, sizeof(char), VEGANO, 0) != -1) {
                    tieneV--;
                    printf("                        [Cliente %d:] recibio Veg\n", i);
                }
                if (tieneP > 0 && msgrcv(msgidFritas, &message, sizeof(char), PAPAS_FRITAS, 0) != -1) {
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

    for (int i = 0; i < NUM_EMPLOYEES; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // Proceso hijo
            if (i == 0) { // Proceso de Hamburguesas
                while (1) {
                    if (msgrcv(msgidHamburguesas, &message, sizeof(char), HAMBURGUESA, 0) != -1) {
                        printf("    [Empleado 1] Preparando hamburguesa\n");
                        fflush(stdout);
                        //sleep(1);
                        message.type = HAMBURGUESA;
                        message.text[0] = 'H';
                        msgsnd(msgidDistribucion, &message, sizeof(char), 0);
                        
                    }
                }
            } else if (i == 1) { // Proceso Vegano
                while (1) {
                    if (msgrcv(msgidVegano, &message, sizeof(char), VEGANO, 0) != -1) {
                        printf("    [Empleado 2] Preparando vegano\n");
                        fflush(stdout);
                        //sleep(1);
                        message.type = VEGANO;
                        message.text[0] = 'V';
                        msgsnd(msgidDistribucion, &message, sizeof(char), 0);
                    }
                }
            } else if (i == 2) { // Proceso de Papas Fritas
                while (1) {
                    if (msgrcv(msgidFritas, &message, sizeof(char), PAPAS_FRITAS, 0) != -1) {
                        printf("    [Empleado 3] Preparando papas fritas\n");
                        fflush(stdout);
                        //sleep(1);
                        message.type = PAPAS_FRITAS;
                        message.text[0] = 'P';
                        msgsnd(msgidDistribucion, &message, sizeof(char), 0);
                    }
                }
            } else if (i == 4) { // Proceso de Distribucion
                printf("    -- se crea Empleado 4\n");
                while (1) {
                    if (msgrcv(msgidClientesVIP, &message, sizeof(char), CLIENTE_VIP, 0) != -1) {
                        printf("    ------ (empleado admin vip)\n");
                        strcpy(pedido, message.text);
                        for (int j = 0; pedido[j] != '\0'; j++) {
                            char comida = pedido[j];
                            printf("    [Admin] distribuye %c\n", comida);
                            if (comida == 'H') {
                                message.text[0] = 'H';
                                msgsnd(msgidHamburguesas, &message, sizeof(char), 0);
                            } else if (comida == 'V') {
                                message.text[0] = 'V';
                                msgsnd(msgidVegano, &message, sizeof(char), 0);
                            } else if (comida == 'P') {
                                message.text[0] = 'P';
                                msgsnd(msgidFritas, &message, sizeof(char), 0);
                            }
                        }
                        printf("[Admin] Distribuyo un pedido VIP\n");
                    } else if (msgrcv(msgidClientesVIP, &message, sizeof(char), CLIENTE_VIP, 0) != -1) {
                        printf("    ------ (empleado admin normal)\n");
                        strcpy(pedido, message.text);
                        for (int j = 0; pedido[j] != '\0'; j++) {
                            char comida = pedido[j];
                            printf("    [Admin] distribuye %c\n", comida);
                            if (comida == 'H') {
                                message.text[0] = 'H';
                                msgsnd(msgidHamburguesas, &message, sizeof(char), 0);
                            } else if (comida == 'V') {
                                message.text[0] = 'V';
                                msgsnd(msgidVegano, &message, sizeof(char), 0);
                            } else if (comida == 'P') {
                                message.text[0] = 'P';
                                msgsnd(msgidFritas, &message, sizeof(char), 0);
                            }
                        }
                        printf("[Admin] Distribuyo un pedido NORMAL\n");
                    }
                }
                exit(0);
            }
        }
    }

    // Proceso principal
    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES+NUM_CLIENTES; i++) {
        wait(NULL);
    }

    // Eliminar colas de mensajes
    // Es necesario?
    msgctl(msgidHamburguesas, IPC_RMID, NULL);
    msgctl(msgidVegano, IPC_RMID, NULL);
    msgctl(msgidFritas, IPC_RMID, NULL);
    msgctl(msgidDistribucion, IPC_RMID, NULL);
    msgctl(msgidClientes, IPC_RMID, NULL);
    msgctl(msgidClientesVIP, IPC_RMID, NULL);

    return 0;
}