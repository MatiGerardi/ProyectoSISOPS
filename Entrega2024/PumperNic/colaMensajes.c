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
#define CLIENTE_VIP 1
#define CLIENTE_COMUN 2

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

msgbuf colaMensajes;

int concatPedidoNumCliente(int pedido, int numCliente) {
    // pedido nunca debe ser 0
    if (numCliente == 0) {
        return pedido * 10;
    }

    int numCliente_temp = numCliente;
    int numCliente_digits = 0;

    // Contar el número de dígitos de numCliente
    while (numCliente_temp != 0) {
        numCliente_temp /= 10;
        numCliente_digits++;
    }

    // Multiplicar pedido por 10 elevado al número de dígitos de numCliente
    int factor = 1;
    for (int i = 0; i < numCliente_digits; i++) {
        factor *= 10;
    }

    // Concatenar pedido y numCliente
    return pedido * factor + numCliente;
}

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
    colaMensajes.cliente = numCliente;
    //printf("CLIENTE: %d, PEDIDO: %s\n", numCliente, pedido);
    for (int j = 0; pedido[j] != '\0'; j++) {
        char comida = pedido[j];
        if (comida == 'H') {   
            colaMensajes.type = COCINAR_HAMBURGUESA;
            strcpy(colaMensajes.text, "H");                          
            msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
        } else if (comida == 'V') {
            colaMensajes.type = COCINAR_VEGANO;
            strcpy(colaMensajes.text, "V");
            msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
        } else if (comida == 'P') {
            colaMensajes.type = COCINAR_PAPAS_FRITAS;
            strcpy(colaMensajes.text, "P");
            msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
        }
    }
}

void clientes(int i){
    generarPedidos(&cliente);
    
    printf(ANSI_COLOR_RESET"[Cliente %d] Pedido: %s %s\n", i, cliente.pedido, cliente.esVIP ? "(VIP)" : "" );
    fflush(stdout);
    
    // Se meete en su cola corresponiente
    if (cliente.esVIP && cliente.paciencia > PACIENCIA) { // Si es VIP
        colaMensajes.type = CLIENTE_VIP;
        colaMensajes.cliente = i;
        strcpy(colaMensajes.text, cliente.pedido);
        msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
    } else if (!cliente.esVIP && cliente.paciencia > PACIENCIA) { // SI no es VIP es NORMAL
        colaMensajes.type = CLIENTE_COMUN;
        colaMensajes.cliente = i;
        strcpy(colaMensajes.text, cliente.pedido);
        msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
    } else{
        cliente.esVIP ? printf(ANSI_COLOR_RED"X[Cliente %d] se fue\n"ANSI_COLOR_RESET, i) : printf(ANSI_COLOR_RED"X[Cliente %d] se fue\n"ANSI_COLOR_RESET, i);
        fflush(stdout);
        exit(0); // Cliente se va y no regresa
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
        int esperarBur = concatPedidoNumCliente(RECIBIR_HAMBURGUESA, i);
        int esperarVeg = concatPedidoNumCliente(RECIBIR_VEGANO, i);
        int esperarPapas = concatPedidoNumCliente(RECIBIR_PAPAS_FRITAS, i);
        sleep(2);
        if (tieneH > 0 && msgrcv(msgid, &colaMensajes, MSGBUF_SIZE, esperarBur, IPC_NOWAIT) != -1) {
            tieneH--;
            //printf("<<<                        [Cliente %dH] recibio: %s, %d\n", i, colaMensajes.text, colaMensajes.cliente);
        }
        if (tieneV > 0 && msgrcv(msgid, &colaMensajes, MSGBUF_SIZE, esperarVeg, IPC_NOWAIT) != -1) {
            tieneV--;
            //printf("<<<                        [Cliente %dV] recibio: %s, %d\n", i, colaMensajes.text, colaMensajes.cliente);
        }
        if (tieneP > 0 && msgrcv(msgid, &colaMensajes, MSGBUF_SIZE, esperarPapas, IPC_NOWAIT) != -1) {
            tieneP--;
            //printf("<<<                        [Cliente %dP] recibio: %s, %d\n", i, colaMensajes.text, colaMensajes.cliente);
        }
    }
    printf(ANSI_COLOR_GREEN"                        >>>>>>[Cliente %d] se fue con su pedido<<<<<<\n" ANSI_COLOR_RESET, i);
}

void cocinero_hamburguesas(){
    while (1) {
        msgrcv(msgid, &colaMensajes, MSGBUF_SIZE, COCINAR_HAMBURGUESA, 0);
        //printf("    [Empleado 0] Preparando hamburguesa: %s, %d\n", colaMensajes.text, colaMensajes.cliente);
        //fflush(stdout);
        sleep(1);
        colaMensajes.type = concatPedidoNumCliente(RECIBIR_HAMBURGUESA, colaMensajes.cliente);
        colaMensajes.cliente = colaMensajes.cliente;
        strcpy(colaMensajes.text, "H");
        msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
    }
}

void cocinero_vegano(){
    while (1) {
        msgrcv(msgid, &colaMensajes, MSGBUF_SIZE, COCINAR_VEGANO, 0);
        //printf("    [Empleado 1] Preparando vegano: %s, %d\n", colaMensajes.text, colaMensajes.cliente);
        //fflush(stdout);
        sleep(1);
        colaMensajes.type = concatPedidoNumCliente(RECIBIR_VEGANO, colaMensajes.cliente);
        colaMensajes.cliente = colaMensajes.cliente;
        strcpy(colaMensajes.text, "V");
        msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
    }
}

void cocinero_papas(int id){
    while (1) {
        msgrcv(msgid, &colaMensajes, MSGBUF_SIZE, COCINAR_PAPAS_FRITAS, 0);
        //printf("    [Empleado %d] Preparando papas fritas: %s, %d\n", id, colaMensajes.text, colaMensajes.cliente);
        //fflush(stdout);
        sleep(1);
        colaMensajes.type = concatPedidoNumCliente(RECIBIR_PAPAS_FRITAS, colaMensajes.cliente);
        colaMensajes.cliente = colaMensajes.cliente;
        strcpy(colaMensajes.text, "P");
        msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
    }
}

void administrador(){
    while (1) {
        if (msgrcv(msgid, &colaMensajes, MSGBUF_SIZE, -2, 0) != -1) { // -2 ==> recibe mensajes tipo 1 y tipo 2, con prioridad al menor
            //~ printf("    ------ (empleado admin vip)\n");
            printf(ANSI_COLOR_MAGENTA"      [ADMIN] tiene el pedido: %s, Cliente: %d\n"ANSI_COLOR_RESET, colaMensajes.text, colaMensajes.cliente);
            fflush(stdout);
            strcpy(pedido, colaMensajes.text);
            int numCliente = colaMensajes.cliente;
            distribuirPedido(numCliente, pedido);
            //~ printf("[Admin] Distribuyo un pedido VIP\n");
            sleep(2);
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

    kill(cocinero_ham_p, SIGKILL);
    kill(cocinero_veg_p, SIGKILL);
    kill(cocinero_fritas1_p, SIGKILL);
    kill(cocinero_fritas2_p, SIGKILL);
    kill(cocinero_admin_p, SIGKILL);

    return 0;
}
