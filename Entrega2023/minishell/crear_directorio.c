#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h> //mkdir();
#include <string.h> //manejo de strings;


int main(int i, char * argv[]){

    if(argv[2] != NULL){
		printf("Error: Parametros no validos. Consultar el manual para mas informacion.\n");
		return 0;
	}

    int check = 0;

    check = mkdir(argv[1],0777);

    if(!check)
	printf("\033[1;32m>> Directorio creado con éxito \033[0m \n");
    else
	printf("\033[1;31m>> Error al crear el directorio \033[0m \n");


    return 0;


}
