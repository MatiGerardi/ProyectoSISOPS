#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

sem_t cola_mesa_comun,cont_mesa_comun,cola_mesa_empresas,cont_mesa_empresas,cont_mesa_principal,cola_mesa_politicos,cont_mesa_politicos;
sem_t turno_comun_inicio,turno_empresa_inicio,turno_politico_inicio,turno_comun_fin,turno_empresa_fin,turno_politico_fin;


void * fun_empleado_comun(){
	while(1){
        int id;
        id=pthread_self();
        
            if(sem_trywait(&cola_mesa_politicos)==0){   
                printf("empleado %d atendiendo a politico \n",id);
                sem_post(&turno_politico_inicio);         
                sleep(3);
                printf("empleado %d termino de atender politico atendido\n",id);
                sem_post(&turno_politico_fin); 
            }
            else{
                sem_wait(&cola_mesa_comun);  
                 printf("empleado %d atendiendo a comun\n",id);   
                sem_post(&turno_comun_inicio);         
                sleep(3);
                printf("empleado %d termino de atender a comun \n",id);
                sem_post(&turno_comun_fin);
            }
	
        }
}
void * fun_empleado_empresas(){
	while(1){
            int id;
            id=pthread_self();
            
            if(sem_trywait(&cola_mesa_politicos)==0){   
                printf("empleado %d atendiendo a politico \n",id);
                sem_post(&turno_politico_inicio);        
                
                sleep(3);
                printf("empleado %d termino de atender politico atendido\n",id);
                sem_post(&turno_politico_fin); 
            }
            else{
                sem_wait(&cola_mesa_empresas);    
                printf("empleado %d atendiendo a empresario\n",id); 
                sem_post(&turno_empresa_inicio);         
                
                sleep(3);
                printf("empleado %d termino de atender a empresario \n",id);
                sem_post(&turno_empresa_fin);
            }
	
        }
}
void * fun_cliente(){

    int id;
    id=pthread_self();
        // Intenta decrementar el semáforo cont_mesa_principal sin bloquear. Si el semáforo es mayor que 0, el cliente entra a la cola de la mesa principal.
        // else el cliente sale de la mesa principal porque no hay espacio.
        if (sem_trywait(&cont_mesa_principal)==0){ 
                        
            printf("cliente %d entra a la cola de la mesa principal \n",id );
            int cliente=rand()% 3; //se determina aleatoriamente su tipo (rand() % 3).
                           
            if(cliente==0){//Cliente comun
                //Intenta entrar a la mesa de clientes comunes         
                if(sem_trywait(&cont_mesa_comun)==0){  
                    sem_post(&cont_mesa_principal);
                    //Incrementa el semáforo cont_mesa_principal, indicando que un cliente ha dejado la mesa principal.     
                    
                    printf("cliente %d entra a la cola de la mesa para clientes comunes \n",id);
                    sem_post(&cola_mesa_comun);         
                    //Incrementa el semáforo cola_mesa_comun, indicando que un cliente ha entrado en la cola de la mesa común.

                    sem_wait(&turno_comun_inicio);         
                    //El cliente espera hasta que sea su turno para ser atendido en la mesa común.

                    sem_wait(&turno_comun_fin);
                    //El cliente espera hasta que termine su turno en la mesa común.

                    printf("cliente %d comun se retira \n",id);
                    sem_post(&cont_mesa_comun);
                    //Incrementa el semáforo cont_mesa_comun, indicando que un cliente ha dejado la mesa común.
                }
            }
            else if(cliente==1){ //Cliente empresarial
                if(sem_wait(&cont_mesa_empresas)==0){ //Intenta entrar a la mesa de clientes empresariales
                    printf("cliente %d entra a la cola de la mesa para clientes empresariales \n",id);
                    sem_post(&cont_mesa_principal); //Libera lugar de la mesa principal 
                    sem_post(&cola_mesa_empresas); //Entra a la cola de la mesa empresarial
                    
                    sem_wait(&turno_empresa_inicio); //El cliente espera hasta que sea su turno para ser atendido en la mesa empresarial       
 
                    sem_wait(&turno_empresa_fin); //El cliente espera hasta que termine su turno en la mesa empresarial
                    printf("cliente %d empresa se retira \n",id);
                    sem_post(&cont_mesa_empresas); // Libera lugar de la mesa empresarial
                    
                }
            }
            else if(cliente==2){

                if(sem_wait(&cont_mesa_politicos)==0){ 
                    printf("cliente %d entra a la cola de la mesa para clientes politicos \n",id);
                    sem_post(&cont_mesa_principal); 
                    sem_post(&cola_mesa_politicos);  
                    
                    sem_wait(&turno_politico_inicio);             

                    sem_wait(&turno_politico_fin);
                    printf("cliente %d politico se retira \n",id);
                    sem_post(&cont_mesa_politicos);
                }
            }
		}else{
            printf("NO HAY ESPACIO PARA ENTRAR  \n");
        }
   
    
   
    return 0;
}

int main (){
    pthread_t empleados[3],clientes[50];

    sem_init(&cola_mesa_comun, 0, 0);
    sem_init(&cola_mesa_empresas, 0, 0);
    sem_init(&cola_mesa_politicos, 0, 0);

    sem_init(&cont_mesa_principal, 0, 30);
    sem_init(&cont_mesa_comun, 0, 15);
    sem_init(&cont_mesa_empresas, 0, 15);
    sem_init(&cont_mesa_politicos, 0, 15);

    sem_init(&turno_comun_inicio, 0, 0);
    sem_init(&turno_empresa_inicio, 0, 0);
    sem_init(&turno_politico_inicio, 0, 0);
    sem_init(&turno_comun_fin, 0, 0);
    sem_init(&turno_empresa_fin, 0, 0);
    sem_init(&turno_politico_fin, 0, 0);
    
    

    pthread_create(&(empleados[0]),NULL,(void*)fun_empleado_comun,NULL);
    pthread_create(&(empleados[1]),NULL,(void*)fun_empleado_empresas,NULL);
    pthread_create(&(empleados[2]),NULL,(void*)fun_empleado_empresas,NULL);
    
    for(int i =0 ; i<50;i++){
        pthread_create(&(clientes[i]),NULL,(void*)fun_cliente,NULL);
        }

    pthread_join(empleados[0],NULL);
    pthread_join(empleados[1],NULL);
    pthread_join(empleados[2],NULL);
    for(int i =0 ; i<50;i++){
        pthread_join(clientes[i],NULL);
        }
        

    }

