#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 50

int main(int i, char * argv[]){

    int file;
    char buffer[BUFFER_SIZE];
    int read_size;
    
    if(argv[2] != NULL){
		printf("Error: los parametros son invalidos. 'ayuda crear_archivo' para mas informacion.\n");
		return 0;
	}
    
    file = open(argv[1], O_CREAT);
    if (file == -1){
        printf("\033[1;31m>> Error no existe el archivo \033[0m \n"); // en rojo
    }
    else{
        while ((read_size = read(file, buffer, BUFFER_SIZE)) > 0){
            write(1, &buffer, read_size);
        }
        printf("\033[1;32m>> Archivo creado con exito. Actualmenete no tiene permisos. \033[0m \n"); // en verde
    }    
    
    close(file);     

    return 0;   
}