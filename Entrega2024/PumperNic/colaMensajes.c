#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NUM_EMPLOYEES 5
#define NUM_CLIENTES 5
#define TAMAÑO_PEDIDO 4
#define PACIENCIA 3
#define KEY ((key_t) (1234))

//Tipos mensajes
#define CLIENTE_COMUN 1
#define CLIENTE_VIP 2

#define COCINAR_HAMBURGUESA 3
#define COCINAR_VEGANO 4
#define COCINAR_PAPAS_FRITAS 5

#define RECIBIR_HAMBURGUESA 6
#define RECIBIR_VEGANO 7
#define RECIBIR_PAPAS_FRITAS 8

typedef struct {
    char pedido[TAMAÑO_PEDIDO]; // Combinación de 'H', 'V', 'P'
    int esVIP;
    int paciencia;
} Cliente;


Cliente cliente;
char pedido[TAMAÑO_PEDIDO];

typedef struct{
    long type;
    char text[TAMAÑO_PEDIDO];
    int cliente;
} msgbuf;

#define MSGBUF_SIZE sizeof(msgbuf) - sizeof(long)
int msgid;

msgbuf mRecibido;
msgbuf mAEnviar;

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

void distribuirPedido(int numCliente, char *pedido) {
    mAEnviar.cliente = numCliente;
    //printf("CLIENTE: %d, PEDIDO: %s\n", numCliente, pedido);
    for (int j = 0; pedido[j] != '\0'; j++) {
        char comida = pedido[j];
        if (comida == 'H') {   
            mAEnviar.type = COCINAR_HAMBURGUESA;
            strcpy(mAEnviar.text, "H");                          
            msgsnd(msgid, &mAEnviar, MSGBUF_SIZE, 0);
        } else if (comida == 'V') {
            mAEnviar.type = COCINAR_VEGANO;
            strcpy(mAEnviar.text, "V");
            msgsnd(msgid, &mAEnviar, MSGBUF_SIZE, 0);
        } else if (comida == 'P') {
            mAEnviar.type = COCINAR_PAPAS_FRITAS;
            strcpy(mAEnviar.text, "P");
            msgsnd(msgid, &mAEnviar, MSGBUF_SIZE, 0);
        }
    }
}

void clientes(int i){
    generarPedidos(&cliente);
    
    printf(ANSI_COLOR_RESET"[Cliente %d] Pedido: %s %s\n", i, cliente.pedido, cliente.esVIP ? "(VIP)" : "" );
    fflush(stdout);
    
    // Se meete en su cola corresponiente
    while(1) {
        if (cliente.esVIP && cliente.paciencia > PACIENCIA) { // Si es VIP
            mAEnviar.type = CLIENTE_VIP;
            mAEnviar.cliente = i;
            strcpy(mAEnviar.text, cliente.pedido);
            msgsnd(msgid, &mAEnviar, MSGBUF_SIZE, 0);
            break;
        } else if (!cliente.esVIP && cliente.paciencia > PACIENCIA) { // SI no es VIP es NORMAL
            mAEnviar.type = CLIENTE_COMUN;
            mAEnviar.cliente = i;
            strcpy(mAEnviar.text, cliente.pedido);
            msgsnd(msgid, &mAEnviar, MSGBUF_SIZE, 0);
            break;
        } else{
            cliente.esVIP ? printf(ANSI_COLOR_RED"X[Cliente %d] se fue\n"ANSI_COLOR_RESET, i) : printf(ANSI_COLOR_RED"X[Cliente %d] se fue\n"ANSI_COLOR_RESET, i);
            fflush(stdout);
            sleep(3);
            cliente.paciencia++;
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
        if (tieneH > 0 && msgrcv(msgid, &mRecibido, MSGBUF_SIZE, RECIBIR_HAMBURGUESA, IPC_NOWAIT) != -1) {
            tieneH--;
            //printf("<<<                        [Cliente %dH] recibio: %s, %d\n", i, mRecibido.text, mRecibido.cliente);
        }
        if (tieneV > 0 && msgrcv(msgid, &mRecibido, MSGBUF_SIZE, RECIBIR_VEGANO, IPC_NOWAIT) != -1) {
            tieneV--;
            //printf("<<<                        [Cliente %dV] recibio: %s, %d\n", i, mRecibido.text, mRecibido.cliente);
        }
        if (tieneP > 0 && msgrcv(msgid, &mRecibido, MSGBUF_SIZE, RECIBIR_PAPAS_FRITAS, IPC_NOWAIT) != -1) {
            tieneP--;
            //printf("<<<                        [Cliente %dP] recibio: %s, %d\n", i, mRecibido.text, mRecibido.cliente);
        }
    }
    printf(ANSI_COLOR_GREEN"                        >>>>>>[Cliente %d] se fue con su pedido<<<<<<\n" ANSI_COLOR_RESET, i);
}

void cocinero_hamburguesas(){
    while (1) {
        msgrcv(msgid, &mRecibido, MSGBUF_SIZE, COCINAR_HAMBURGUESA, 0);
        //printf("    [Empleado 0] Preparando hamburguesa: %s, %d\n", mRecibido.text, mRecibido.cliente);
        fflush(stdout);
        sleep(1);
        mAEnviar.type = RECIBIR_HAMBURGUESA;
        mAEnviar.cliente = mRecibido.cliente;
        strcpy(mAEnviar.text, "H");
        msgsnd(msgid, &mAEnviar, MSGBUF_SIZE, 0);
    }
}

void cocinero_vegano(){
    while (1) {
        msgrcv(msgid, &mRecibido, MSGBUF_SIZE, COCINAR_VEGANO, 0);
        //printf("    [Empleado 1] Preparando vegano: %s, %d\n", mRecibido.text, mRecibido.cliente);
        fflush(stdout);
        sleep(1);
        mAEnviar.type = RECIBIR_VEGANO;
        mAEnviar.cliente = mRecibido.cliente;
        strcpy(mAEnviar.text, "V");
        msgsnd(msgid, &mAEnviar, MSGBUF_SIZE, 0);
    }
}

void cocinero_papas(int id){
    while (1) {
        msgrcv(msgid, &mRecibido, MSGBUF_SIZE, COCINAR_PAPAS_FRITAS, 0);
        //printf("    [Empleado %d] Preparando papas fritas: %s, %d\n", id, mRecibido.text, mRecibido.cliente);
        fflush(stdout);
        sleep(1);
        mAEnviar.type = RECIBIR_PAPAS_FRITAS;
        mAEnviar.cliente = mRecibido.cliente;
        strcpy(mAEnviar.text, "P");
        msgsnd(msgid, &mAEnviar, MSGBUF_SIZE, 0);
    }
}

void administrador(){
    while (1) {
        while (msgrcv(msgid, &mRecibido, MSGBUF_SIZE, CLIENTE_VIP, IPC_NOWAIT) != -1) {
            //~ printf("    ------ (empleado admin vip)\n");
            printf(ANSI_COLOR_MAGENTA"      [ADMIN] tiene el pedidoVIP: %s, %d\n"ANSI_COLOR_RESET, mRecibido.text, mRecibido.cliente);
            fflush(stdout);
            strcpy(pedido, mRecibido.text);
            int numCliente = mRecibido.cliente;
            distribuirPedido(numCliente, pedido); //probar con &pedido
            //~ printf("[Admin] Distribuyo un pedido VIP\n");
        }
        if (msgrcv(msgid, &mRecibido, MSGBUF_SIZE, CLIENTE_COMUN, IPC_NOWAIT) != -1) {
            //~ printf("    ------ (empleado admin normal)\n");
            printf(ANSI_COLOR_MAGENTA"      [ADMIN] tiene el pedido: %s, %d\n"ANSI_COLOR_RESET, mRecibido.text, mRecibido.cliente);
            fflush(stdout);
            strcpy(pedido, mRecibido.text);
            int numCliente = mRecibido.cliente;
            distribuirPedido(numCliente, pedido);
            //~ printf("[Admin] Distribuyo un pedido NORMAL\n");
        }
    }
}

int main() {
    msgid = msgget(KEY, IPC_CREAT | 0666);
    msgctl(msgid, IPC_RMID, NULL);
    msgid = msgget(KEY, IPC_CREAT | 0666);
    /*Las dos llamadas a msgget junto con la llamada a msgctl aseguran que cualquier 
     * cola de mensajes existente con la clave KEY sea eliminada y se cree una nueva 
     * cola de mensajes vacía*/

    // Crear procesos clientes
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pid_t pidCli = fork();
        if (pidCli < 0) {
            perror("Error al crear proceso Cliente");
            exit(EXIT_FAILURE);
        } else if (pidCli == 0) {// Proceso hijo
            clientes(i);
            exit(0);
        }
    }

    // Codigo para cada empleado
    // Proceso de Hamburguesas
    pid_t cocinero_ham_p = fork();
    if (cocinero_ham_p == 0) { 
        cocinero_hamburguesas();
        exit(0);
    } 
    // Proceso de Menu Vegano
    pid_t cocinero_veg_p = fork();
    if (cocinero_veg_p == 0) { 
        cocinero_vegano();
        exit(0);
    }
    // Proceso de Papas Fritas 1
    pid_t cocinero_fritas1_p = fork();
    if (cocinero_fritas1_p == 0) { 
        cocinero_papas(1);
        exit(0);
    }
    // Proceso de Papas Fritas 2
    pid_t cocinero_fritas2_p = fork();
    if (cocinero_fritas2_p == 0) {
        cocinero_papas(2);
        exit(0);
    } 
    // Proceso de Distribucion
    pid_t cocinero_admin_p = fork();
    if (cocinero_admin_p == 0) {
        administrador();
        exit(0);
    }

    // --- no llega a ejecutarse esta parte por el while(1) del Admin
    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < NUM_EMPLOYEES+NUM_CLIENTES; i++) {
        wait(NULL);
    }

    return 0;
}
