#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char * argv[]){
	
	if(argv[2] != NULL){
		printf("Error: los parametros son invalidos. 'ayuda listar_directorio' para mas informacion.\n");
		return 0;
	}
	
    char * dir_name = argv[1];
	
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_name);
    
    if (d != NULL){
		while((dir = readdir(d))){
	    	printf("%s\n", dir->d_name);
		}
		closedir(d);
    }
    else {	    
		perror("\033[1;31m>> Error al listar el contenido del directorio\033[0m"); // en rojo
		fflush(NULL);
    }
    
    return 0;
}