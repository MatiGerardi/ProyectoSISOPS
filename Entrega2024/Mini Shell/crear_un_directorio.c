#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h> //mkdir();
#include <string.h> //manejo de strings;

int main(int i, char * argv[]){
    
    if(argv[2] != NULL){
		printf("Error: Parametros no validos. Consultar 'manual crear_un_directorio' para mas informacion.\n");
		return 0;
	}

    if(mkdir(argv[1],0777)==0){
	    printf("\033[1;32m>> Directorio creado con éxito \033[0m \n"); //Verde
    }else{
	    printf("\033[1;31m>> Error al crear el directorio \033[0m \n"); //Rojo
    }
    
    return 0;

    
}