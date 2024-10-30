#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

int main(){
    pid_t pid;
    int error;
    char * instruccion = (char*) malloc(sizeof(char)*300);
	char * args[50];
	
	for (int j = 0; j < 50; j++){
		args[j] = NULL;
	}

    while(1){
		printf("\e[1;34m>> Instrucción: \033[0m");
		
		fgets(instruccion, sizeof(char)*300, stdin); //se lee por consola la instruccion
		size_t len = strlen(instruccion);
		instruccion[--len] = '\0'; 					//se indica el final de la cadena 
		
		int i = 0;
		char * token;
		token = strtok(instruccion, " "); //se divide la instruccion en tokens separados por espacio
		while(token != NULL){
			args[i] = token;
			token = strtok(NULL, " ");
			i++;
		}
		args[i] = NULL;								//nos aseguramos de que haya null como fin de la instruccion
			
		// Si la instruccion es exit, se termina la ejecucion
		if(strcmp(args[0], "salir") == 0) {
			free(instruccion);
			exit(0);
		}	

		// En el caso de tratarse de otra instruccion, se modifica el primer elemento de args con el path de la instruccion.
		char path[50];
		strcpy(path, "./");				//se espera que las instrucciones a ejecutar estan en el mismo directorio
		strcat(path, args[0]);

		args[0] = path;		
		
		pid = fork();

		if(pid==0){
			error = execv(args[0], args);
			if(error == -1){
		 		perror("Error");
		 		fflush(NULL);
			}
		}else{
			wait(NULL);
		}
    }
    
    return 0;
}