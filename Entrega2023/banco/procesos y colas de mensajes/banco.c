
#include "tipos.h"

struct msg_buffer{
	long type;
	char text;
	};

int main()
{
	key_t key;
	int msg_id;
	struct msg_buffer message;
	
	key = ftok("/tmp",'A');
	
	msg_id = msgget(key,0666| IPC_CREAT);
	if (msg_id==-1){
		perror("msgget");
		exit(1);
	} 
	
	for(int i =0 ; i<30;i++){
        message.type = CONT_MESA_PRINCIPAL;
        msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);   
     }
     for(int i =0 ; i<15;i++){
        message.type = CONT_MESA_COMUN;
        msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);   
     }
     for(int i =0 ; i<15;i++){
        message.type = CONT_MESA_EMPRESARIAL;
        msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);   
     }
     for(int i =0 ; i<15;i++){
        message.type = CONT_MESA_POLITICO;
        msgsnd(msg_id,&message,sizeof(struct msg_buffer)-sizeof(long),0);   
     }

     
     for(int i =0 ; i<40;i++){
        int pid = fork();
        if(pid==0){
			char * arg[] = {"./clientes",NULL};
			execvp("./clientes",arg);
			} 
     }
     
     sleep(2);
     
     for(int i =0 ; i<3;i++){
        int pid = fork();
        if(pid==0){
			if(i==0){
				char * arg[] = {"./empleadoComun",NULL};
				execvp("./empleadoComun",arg);
			}else{
				char * arg[] = {"./empleadoEmpresarial",NULL};
				execvp("./empleadoEmpresarial",arg);
				}
		} 
     }

	for(int i =0 ; i<43;i++){
		wait(NULL);
	}
	
	return 0;
}

