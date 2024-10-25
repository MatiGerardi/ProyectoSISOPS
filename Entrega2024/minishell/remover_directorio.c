#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int main(int i, char * argv[]){

    int check = 0;

    if(argv[2] != NULL){
		printf("Error: Parametros no validos. Consultar el manual para mas informacion.\n");
		return 0;
	}

    check = rmdir(argv[1]);

    if(!check)
	printf("\033[1;32m>> Directorio removido con éxito \033[0m \n");
    else
	printf("\033[1;31m>> Error al remover el directorio \033[0m \n");


    return 0;


}
