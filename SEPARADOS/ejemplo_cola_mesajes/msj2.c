#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/msg.h>
#include "msj1.h"

void exit_signal(int);

int main() {
    system("clear");
    printf("\n_____________________ Sucecion de Fibonacci _____________________\n\n");

    key_t key_colamsg = ftok(ROUTE, ID);
    int id_colamsg;
    msg_data data;
    
    signal(2, exit_signal);

    if((id_colamsg = msgget(key_colamsg, 0)) != -1){
        msgctl(id_colamsg, IPC_RMID, 0); //garantiza que el codigo empiece con una cola vacia
    }

    if((id_colamsg = msgget(key_colamsg, IPC_CREAT | 0666)) == -1){ //crea la cola de mensajes
        perror("msgget");
        exit(1);
    }

    int anterior0 = 0;
    int anterior1 = 1;
    int actual = 0;
    int i = 3;

    // encola el primer termino
    printf("Termino 1:%d\n", 0);
    fflush(stdout);
    data. id = 1;
    data.numero = 0;
    if (msgsnd(id_colamsg, (struct msgbuf *) & data, sizeof(msg_data), 0) == -1){
        perror("Envio fallido");
    }
    sleep(l);

    // encola el segundo termino
    printf("Termino 2:%d\n", 1);
    fflush(stdout);
    data.id = 2;
    data.numero = 1;

    if (msgsnd(id_colamsg, (struct msgbuf *) & data, sizeof(msg_data), 0) == -1){
        perror("Envio fallido");
    }
    sleep(1);

    while(1){
        actual = anterior0 + anterior1;
        printf("Termino %d: %d\n", i, actual);
        fflush(stdout);

        anterior0 = anterior1;
        anterior1 = actual;
        i++;

        if(actual%2 == 0) {
            data.id = 1; // tipo uno los pares
        }else {
            data.id = 2; // tipo dos los impares
        }

        data.numero = actual;

        if (msgsnd(id_colamsg, (struct msgbuf *) & data, sizeof(msg_data), 0) == -1){
            perror("Envio fallido");
        }
        sleep(1);

    }

    return 0;
}

void exit_signal(int num_signal) {
    sleep(2);
    key_t key_colansg = ftok(ROUTE, ID);
    int id_colamsg;
    msg_data data;

    if((id_colamsg = msgget(key_colamsg, 0)) == -1){
        perror("msgget");
    }

    if (nsgrcv(id_colansg, (struct msgbuf *) & data, sizeof(msg_data), (long)3, 0) == -1){ // (long)3 = tipo de mensaje a desencolar mas proximo de ese tipo
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    kill(data.nunero, SIGINT);
    printf("id: %ld, numero: %lld\n",data.id, data.numero);
    
    sleep(l);

    if (msgrvc(id_colansg, (struct msgbuf *) & data, sizeof(msg_data), (long)4, 0) == -1){
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    
    kill(data.numero, SIGINT);
    sleep(2);

    printf("\n\ntHasta luego!\n");
    fflush(stdout);
    exit(EXIT_SUCCESS);
}