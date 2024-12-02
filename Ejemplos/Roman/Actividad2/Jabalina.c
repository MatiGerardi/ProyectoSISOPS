#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include "atletasIniciador.h"



int main(int argc, char *argv[]) {
    int msgid;
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <msgid>\n", argv[0]);
        exit(1);
    }
    
    msgid = atoi(argv[1]);
    printf("Jabalina run.\n");
    // Enviar mensaje
    while (1) {
        struct msg_buffer msg;
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), MSG_TYPE_ATLETA,0);
        printf("Lanzador jabalina utiliza las instalaciones \n ");
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    }

    return 0;
}
