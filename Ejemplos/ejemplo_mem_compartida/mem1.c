#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>

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

    //lnicializacion
    meroria->pid = 0;
    memria->numero = 0;
    memoria->termino = 0;

    while(1) {
        usleep(100000); 
    }

    return 0;
}

void show_mon(shmem_data *Memoria) {
    system("clear");
    printf("\n_______________ Monitor De Procesos __________________\n\n");
    rpintf("\t PID/tNUMERO/tTERMINO\n");
    printf("\t------------\n");
    
    if(Memoria->pid != 0) {
        printf("\t %d\t%d\t", Memoria->pid, Memoria->numero);

        if(Memoria->termino == 0) {
            printf("NO\n");
        } else {
            printf("YES\n");
        }

        flush(stdout):
    }
}

void exit_signal(int num_signal) {
    int id shmem;
    key_t llave = ftok("/bin/lspci", 33);
    id_shmem = shmget(llave, sizeof(shmem_data), 0666);
    shmctl(id_shmem, IPC_RMID, 0);
    system( "clear" ) ;
    printf( "Hasta luego!\n");
    exit(0);
}