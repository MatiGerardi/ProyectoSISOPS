#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define CANTCORREDORES 4
#define CANTMARTILLO 4
#define CANTJABALINA 4

#define MSG_TYPE_ATLETA 


int main(){
    int msgid;

    // Generar la clave de la cola de mensajes usando ftok
    key_t key = ftok("/tmp", 'A');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // Crear la cola de mensajes
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // Crear procesos corredor
    for(int i=0;i<CANTCORREDORES;i++){
        pid_t corredorP = fork();
        if (corredorP == 0) {
            // Proceso hijo 1
            execl("./Corredor", "Corredor", (char *) &msgid, NULL);
            perror("execl");
            exit(1);
        }
    }
    
    for(int i=0;i<CANTMARTILLO;i++){
        pid_t jabalinaP = fork();
        if (jabalinaP == 0) {
            // Proceso hijo 1
            execl("./Jabalina", "Jabalina", (char *) &msgid, NULL);
            perror("execl");
            exit(1);
        }
    }
    for(int i=0;i<CANTJABALINA;i++){
        pid_t martilloP = fork();
        if (martilloP == 0) {
            // Proceso hijo 1
            execl("./Martillo", "Martillo", (char *) &msgid, NULL);
            perror("execl");
            exit(1);
        }
    }
    struct msg_buffer mensaje;
    mensaje.type = MSG_TYPE_ATLETA;
    msgsnd(msgid, &mensaje, sizeof(struct msg_buffer )- sizeof(long), 0);
    
    return 0;


}

