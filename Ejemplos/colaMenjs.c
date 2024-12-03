#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY ((key_t) (1234))

typedef struct{
    long type;
    char text[TAMAÑO_PEDIDO];
    int cliente;
} msgbuf;

#define MSGBUF_SIZE sizeof(msgbuf) - sizeof(long)
int msgid;
msgbuf colaMensajes;

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
        if (pidCli == 0) {// Proceso hijo
            //RECIBIR
            //msgrcv(msgid, &colaMensajes, MSGBUF_SIZE, COCINAR_PAPAS_FRITAS, 0);

            //ENVIAR
            //msgsnd(msgid, &colaMensajes, MSGBUF_SIZE, 0);
            exit(0);
        }
    }
    
    for (int i = 0; i < NUM_EMPLOYEES+NUM_CLIENTES; i++) {
        wait(NULL);
    }

    return 0;
}