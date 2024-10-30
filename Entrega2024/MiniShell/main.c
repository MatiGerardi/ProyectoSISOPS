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
		printf("\e[1;34m>> MiniShell: \033[0m");
		
		fgets(instruccion, sizeof(char)*300, stdin); //lee por consola
		size_t len = strlen(instruccion);
		instruccion[--len] = '\0'; //indica el final de la cadena 
		
		int i = 0;
		char * token;
		token = strtok(instruccion, " "); //divide lo escrito en tokens separados por espacio
		while(token != NULL){
			args[i] = token;
			token = strtok(NULL, " ");
			i++;
		}
		args[i] = NULL; //asegura que haya null al final del arreglo
			
		// Si el primer argumento es "salir", se libera la memoria y se termina el programa.
		if(strcmp(args[0], "salir") == 0) {
			free(instruccion);
			exit(0);
		}	

		// Si el primer argumento es "ayuda", se imprime el ayuda de la instrucción ingresada.
		char path[50];
		strcpy(path, "./"); //se copia el path actual
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