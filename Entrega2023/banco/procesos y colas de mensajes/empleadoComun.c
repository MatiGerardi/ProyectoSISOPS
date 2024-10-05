
#include "tipos.h"

struct msg_buffer{
	long type;
	char text;
	};

int main(int argc, char *argv[])
{
	key_t key;
	int msg_id;
	struct msg_buffer message;

	key = ftok("/tmp",'A');

	msg_id = msgget(key,0666);
	if (msg_id==-1){
		perror("msgget");
		exit(1);
	}

        while(1){

        if(msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),COLA_MESA_POLITICO,IPC_NOWAIT)!=-1){
                printf("empleado atendiendo a politico \n");
                message.type=TURNO_POLITICO_INICIO;
                msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

                sleep(3);
                printf("empleado termino de atender politico atendido\n");
                message.type=TURNO_POLITICO_FIN;
                msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);
            }
            else{
                msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),COLA_MESA_COMUN,0);

                message.type=TURNO_COMUN_INICIO;
                printf("empleado atendiendo a comun\n");
                msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);


                sleep(1);
                printf("empleado termino de atender a comun \n");

                message.type=TURNO_COMUN_FIN;
                msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);
            }

        }

	exit(0);
	return 0;
}

