#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
	
	if(argv[2] != NULL){
		printf("\033[1;32m>> Error: Parametros no validos. Consultar 'ayuda mostrar_archivo'.\033[0m \n");
		return 0;
	}
	
	char * file_path = argv[1];
	FILE * file;
	char ch;
	
	file = fopen(file_path, "r");
	
	if (file != NULL){
		ch = fgetc(file);
		while(ch != EOF){
			putchar(ch);     //imprime el caracter por pantalla
			ch = fgetc(file);
		}
		fclose(file);
		printf("\n");
	}
	else{
		perror("\033[1;31m>> Error al abrir el archivo\033[0m"); // en rojo
		fflush(NULL);
	}
	
	return 0;
}