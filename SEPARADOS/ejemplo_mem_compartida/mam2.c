#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>

typedef struct {
    pid_t pid;
    int numero;
    unsigned char termino;
 } shmen_data;

void show_mon(shmem_data*);
void exit_signal(int);

int main() {
    key_t  llave;
    int id_shmem;
    shmem_data *Memoria;

    signal(2, exit _signal);

    //Obtener llave
    llave = ftok("/bin/lspci", 33) //usa un archivo para formar la llave

    // Creacion del segmento de memoria compartida
    id_shmem = shmget(llave, sizeof(shmem_data), IPC_CREAT | 0666);

    // Vinculacion al segmento con la zona de datos de nuestro programa
    // Se llama una vez por proceso
    Memoria = (shmem_data *) shmat(id_shmem, NULL, 0);

    //Entrar
    meroria->pid = getpid();  //Cmbia el valor de la memoria compartida con su pid
    memria->numero = 0;
    memoria->termino = 0;
    usleep(500000);

    system( "clear");
    srand (getpid( ));
    int repeticion = rand()%91+10;
    for(int i=0; i<repeticion; i++) {
        Memoria->numero++;
        printf("Numero: %d\n" , i);
        usleep(500000);
    }
    Memoria->termino = 1; //modifica el atributo apra indicar que termino
    shmdt(Memoria); // se desvincula sin destruir la memoria compartida
    // destruir la memoria compartida es resposabilidad del ultimo proceso que la use (mem1)

    return 0;
}