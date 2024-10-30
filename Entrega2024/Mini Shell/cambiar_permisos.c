#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char * argv[]){
	
	char * file_path = argv[1];
	
	char mode[3];
	
	strcpy(mode, argv[2]);
	
	int i;
	i = strtol(mode, 0,8); //el modo es indicado en octal para modificar los permisos
	
	if(argv[3] != NULL){
		printf("Error: Parametros no validos. Consultar 'manual cambiar_permisos' para mas informacion.\n");
		return 0;
	}
	
	int resultado = chmod(file_path, i);
	
	if(resultado < 0){
		perror("Error");
	}
	
	
	return 0;
}