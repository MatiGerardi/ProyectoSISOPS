#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CORREDORES 4
#define JABALINA 1
#define MARTILLO 1
#define TIME 100000


sem_t semJabalinaOMartillo, semCorredores;
pthread_mutex_t mutexAtletas;

void* corredor(){
    while(1){
        //Seccion de entrada
        pthread_mutex_lock(&mutexAtletas);
        if(sem_trywait(&semCorredores) == -1){
            //Soy el primero el primer corredor, no dejo que entren los atletas de jabalina o martillo
            sem_wait(&semJabalinaOMartillo);                    
        }else{
            //No soy el primero, devuelvo el valor al que estaba
            sem_post(&semCorredores);        
        }
        //Cuento que entro un corredor a la instalaciones
        printf("Entro un corredor\n");
        sem_post(&semCorredores);
        pthread_mutex_unlock(&mutexAtletas);
        

        //EL corredor usa la instalacion
        printf("El corredor %i utiliza la instalacion\n", pthread_self());
        usleep(TIME);
        //seccion de salida
        pthread_mutex_unlock(&mutexAtletas);
        printf("salio un corredor\n");
        //Cuento que salio un correodr de las instalaciones
        sem_wait(&semCorredores);
        if(sem_trywait(&semCorredores) == -1){
            //Soy el ultimo, libero la instalaciones para los atletas de jabalina y martillo
            sem_post(&semJabalinaOMartillo);  
            printf("Salio el ultimo corredor\n");
        }else{
            sem_post(&semCorredores);
        }
        pthread_mutex_unlock(&mutexAtletas);
    }

}

void* martillo(){

    while(1){
        sem_wait(&semJabalinaOMartillo);
        pthread_mutex_lock(&mutexAtletas);
        printf("Salio un martillo\n");
        printf("El lanzador de martillo %i utiliza la instalacion\n",pthread_self());
        usleep(TIME);
        printf("Salio un martillo\n");
        pthread_mutex_unlock(&mutexAtletas);
        sem_post(&semJabalinaOMartillo);
    }

}

void* jabalina(){

    while(1){
        sem_wait(&semJabalinaOMartillo);
        printf("Entro un jabalina\n");
        pthread_mutex_lock(&mutexAtletas);
        printf("El lanzador de jabalina %i  utiliza la instalacion\n",pthread_self());
        usleep(TIME);
        printf("Salio un jabalina\n");
        pthread_mutex_unlock(&mutexAtletas);
        sem_post(&semJabalinaOMartillo);
    }

}

int main(){
    pthread_t corredoresThread[CORREDORES];
    pthread_t martilloThread[MARTILLO];
    pthread_t jabalinaThread[JABALINA];
    
    //Inicializo los semaforos y mutex
    sem_init(&semCorredores, 0,0);
    sem_init(&semJabalinaOMartillo, 0,1);
    pthread_mutex_init(&mutexAtletas,NULL);
        
    //Inicializo los hilos
    for(int i=0;i<CORREDORES;i++){
        pthread_create(&corredoresThread[i],NULL,&corredor,NULL);    
    }    
    for(int i=0;i<MARTILLO;i++){
        pthread_create(&martilloThread[i],NULL,&martillo,NULL);    
    }    
    for(int i=0;i<JABALINA;i++){
        pthread_create(&jabalinaThread[i],NULL,&jabalina,NULL);    
    }    

    //Espero a los hilos que terminen
    for(int i=0;i<CORREDORES;i++){
        pthread_join(corredoresThread[i],NULL);    
    }    
    for(int i=0;i<MARTILLO;i++){
        pthread_join(martilloThread[i],NULL);    
    }    
    for(int i=0;i<JABALINA;i++){
        pthread_join(jabalinaThread[i],NULL);    
    }    

    
    return 0;
}