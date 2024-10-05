#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){

	if(argv[2] != NULL){
		printf("Error: Parametros no validos. Consultar el manual para mas informacion.\n");
		return 0;
	}

	char * file_path = argv[1];
	FILE * file;
	char ch;

	file = fopen(file_path, "r");

	if (file != NULL){
		ch = fgetc(file);
		while(ch != EOF){
			putchar(ch);
			ch = fgetc(file);
		}
		fclose(file);
		printf("\n");
	}
	else{
		perror("Error al abrir el archivo");
		fflush(NULL);
	}

	return 0;
}
