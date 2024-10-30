#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//En prueba
int main(int i, char * argv[]){
    
    
    if(argv[2] != NULL){
		printf("Error: Parametros no validos. Consultar 'manual eliminar_un_directorio' para mas informacion.\n");
		return 0;
	}

    if(rmdir(argv[1])==0){
	    printf("\033[1;32m>> Directorio removido con éxito \033[0m \n"); //Verde
    }
    else{
	    printf("\033[1;31m>> Error al remover el directorio \033[0m \n"); //Rojo
    }
    return 0;

    
}