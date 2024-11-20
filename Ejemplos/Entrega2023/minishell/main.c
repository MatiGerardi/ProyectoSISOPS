#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

int main(){

    pid_t pid;
    int res;

    char * instruccion = (char*) malloc(sizeof(char)*300);
	char * args[50];

	for (int j = 0; j < 50; j++){
		args[j] = NULL;
	}

    while(1){
		printf("\033[1;33m>> Instrucción: \033[0m");

		fgets(instruccion, sizeof(char)*300, stdin);
		size_t len = strlen(instruccion);
		instruccion[--len] = '\0';



		int i = 0;
		char * token;
		token = strtok(instruccion, " ");
		while(token != NULL){
			args[i] = token;
			token = strtok(NULL, " ");
			i++;
		}
		args[i] = NULL;

		// Si la instruccion es exit, se termina la ejecucion
		if(strcmp(args[0], "salir") == 0) {
			free(instruccion);
			exit(0);
		}

		// En el caso de tratarse de otra instruccion, se busca el path y se modifica el primer elemento de args con el path de la instruccion.
		char path[50];
		strcpy(path, "./");
		strcat(path, args[0]);

		args[0] = path;

		pid = fork();
		switch(pid){

			case(-1):{ // Error
				printf("Fork error \n");
				break;
			}

			case(0):{ // Hijo
				res = execv(args[0], args);
				if(res == -1){
					printf("Error: Comando no existente, consulte el comando 'manual' para recibir manual \n");
					fflush(NULL);
				}
				break;
			}

			default:{ // Padre
				wait(NULL); //Espero que termine el hijo;
			}
		}
    }

    return 0;
}
