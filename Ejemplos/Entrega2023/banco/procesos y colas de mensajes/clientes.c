
#include "tipos.h"

struct msg_buffer{
	long type;
	char text ;
	};

int main(int argc, char *argv[]){

	key_t key;
	int msg_id;
	struct msg_buffer message;

	key = ftok("/tmp",'A');

	msg_id = msgget(key,0666);
	if (msg_id==-1){
		perror("msgget");
		exit(1);
	}
	srand(getpid());
	int cliente= rand()% 3;

	if(msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),CONT_MESA_PRINCIPAL,IPC_NOWAIT)!=-1){
		printf("cliente entra a la cola de la mesa principal \n" );
		if(cliente==0){

		    if(msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),CONT_MESA_COMUN,IPC_NOWAIT)!=-1){

                message.type=CONT_MESA_PRINCIPAL;
                msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

                printf("cliente entra a la cola de la mesa para clientes comunes \n");

                message.type=COLA_MESA_COMUN;
                msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

                msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),TURNO_COMUN_INICIO,0);

                msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),TURNO_COMUN_FIN,0);

                printf("cliente comun se retira \n");
                message.type=CONT_MESA_COMUN;
                msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

		    }
		}
		else if(cliente==1){
			if(msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),CONT_MESA_EMPRESARIAL,IPC_NOWAIT)!=-1){

				message.type=CONT_MESA_PRINCIPAL;
				msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

				printf("cliente entra a la cola de la mesa para clientes empresariales \n");

				message.type=COLA_MESA_EMPRESARIAL;
				msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

				msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),TURNO_EMPRESARIAL_INICIO,0);

				msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),TURNO_EMPRESARIAL_FIN,0);

				printf("cliente empresa se retira \n");
				message.type=CONT_MESA_EMPRESARIAL;
				msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);


		    }
		}
		else if(cliente==2){
			if(msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),CONT_MESA_POLITICO,IPC_NOWAIT)!=-1){

				message.type=CONT_MESA_PRINCIPAL;
				msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

				printf("cliente entra a la cola de la mesa para clientes politicos \n");

				message.type=COLA_MESA_POLITICO;
				msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

				msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),TURNO_POLITICO_INICIO,0);

				msgrcv(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),TURNO_POLITICO_FIN,0);

				printf("cliente politico se retira \n");
				message.type=CONT_MESA_POLITICO;
                msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);

					    }
		    }

    }else{
        printf("NO HAY ESPACIO PARA ENTRAR  \n");
        exit(0);
    }

	exit(0);
	return 0;
}

