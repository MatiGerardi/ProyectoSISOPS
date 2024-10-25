#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/msg.h>
#include "msj1.h"

void exit_signal(int);

int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("Error en los argumentos\n");
        exit(1);
    }  

    int tipo = atoi(argv[1]); // desencola pares o impares (tipo 1 o 2)

    if (tipo != 1 && tipo != 2){
        printf("Error en el tipo\n");
        exit(1);
    }

    signal(2, exit_signal);

    key_t key_colamsg = ftok(ROUTE, ID);
    int id_colamsg;
    msg_data data; // variable donde se deposita el mensaje

    if ((id_colamsg = msgget(key_colamsg, 0)) != -1){
        perror("msgget");
        exit(1);
    }

    // envia un mesaje con el pid del proceso (3 o 4)
    data.id = (tipo+2);
    data.numero = getpid();

    if(msgsnd(id_colamsg, (struct msgbuf *) &data, sizeof(msg_data), 0) == -1){
        perror("Envio fallido");
    }

    printf("id: %d, numero: %lld\n", data.id, data.numero);

     if (tipo == 1){
        printf("\t --> %d Terminos Pares: \n\n\t", getpid());
     } else{
        printf("\t --> %d Terminos Impares: \n\n\t", getpid());
     }
     fflush(stdout);

    // desencole infinitamente
     while(1){
        if(tipo == 1) {
            if(msgrcv(id_colamsg, (struct msgbuf *) &data, sizeof(msg_data), (long)1, 0) == -1){ // (long)1 = tipo de mensaje a desencolar mas proximo de ese tipo
                perror("Recepcion fallida");
            } else{
                printf("%lld - ", data.numero);
                fflush(stdout);
            }
        } else {
            if(msgrcv(id_colamsg, (struct msgbuf *) &data, sizeof(msg_data), (long)2, 0) == -1){
                perror("msgrcv");
                exit(1);
            } else{
                printf("%lld - ", data.numero);
                fflush(stdout);
            }
        }
     }
}

void exit_signal(int s){
    printf("\n\n tHasta luego!\n");
    fflush(stdout);
    exit(1);
}