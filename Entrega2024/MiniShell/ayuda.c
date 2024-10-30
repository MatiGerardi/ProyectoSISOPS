#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main(int argc, char * argv[]){

	if (argv[1] == NULL){ // ayuda
		printf("Mostrar ayuda de un comando especifico, introducir 'ayuda <tipo_comando>' \n\n");
		printf("Comandos disponibles:\n");

		printf(ANSI_COLOR_GREEN"crear_directorio	eliminar_directorio\n");
		printf("crear_archivo		listar_directorio\n");
		printf("mostrar_archivo		cambiar_permisos		salir\n"ANSI_COLOR_RESET);
		return 0;
	}
	
	if(argv[2] != NULL){ // mas de un argumento
		printf("Error: los parametros son invalidos. 'ayuda' para mas informacion.\n");
		return 0;
	}
	
	char * instruccion = argv[1];
	
	if (strcmp(instruccion, "crear_directorio") == 0){
		printf("DESCRIPCION \n \t Este comando crea un directorio en la ubicacion actual de la minishell. \n \t Si el directorio se crea exitosamente, se muestra un mensaje de exito. \n \t De lo contrario, se muestra un mensaje de error. \nFormato \n \t crear_directorio [nombre_directorio]\n");
	} else if (strcmp(instruccion, "eliminar_directorio") == 0){
		printf("DESCRIPCION \n \t Este comando elimina un directorio en la ubicacion actual de la minishell. \n \t Si el directorio se elimina exitosamente, se muestra un mensaje de exito. \n \t De lo contrario, se muestra un mensaje de error. \nFormato \n \t eliminar_directorio [nombre_directorio]\n");
	} else if (strcmp(instruccion, "crear_archivo") == 0){
		printf("DESCRIPCION \n \t Este comando crea un archivo en el directorio actual. \n \t Si el archivo se crea exitosamente, se muestra un mensaje de exito. \n \t De lo contrario, se muestra un mensaje de error. \nFormato \n \t crear_archivo [nombre_archivo]\n");
	} else if (strcmp(instruccion, "listar_directorio") == 0){
		printf("DESCRIPCION \n \t Este comando lista el contenido de un directorio. \n \t Si el contenido se lista exitosamente, se muestra en pantalla. \n \t De lo contrario, se muestra un mensaje de error.\nFormato \n \t listar_directorio [ruta_directorio]\n");
	} else if (strcmp(instruccion, "mostrar_archivo") == 0){
		printf("DESCRIPCION \n \t Este comando muestra el contenido de un archivo. \n \t Si el archivo se muestra exitosamente, se visualiza en pantalla. \n \t De lo contrario, se muestra un mensaje de error. \nFormato \n \t mostrar_archivo [ruta_archivo]\n");
	} else if (strcmp(instruccion, "cambiar_permisos") == 0){
		printf("Descripcion \n \t Este comando cambia los permisos de un archivo especifico. \n \t Si los permisos se cambian exitosamente, se muestra un mensaje de exito. \n \t De lo contrario, se muestra un mensaje de error. \nFormato \n \t cambiar_permisos [ruta_archivo] [permisos] \n \t Donde [permisos] tienen el siguiente formato \n \t \t rwxrwxrwx \n \t Cada conjunto de 'r', 'w', 'x' representa los permisos de lectura, escritura y ejecucion \n \t para el propietario, el grupo y otros, respectivamente.\n \t Debe completar los 9 caracteres correspondientes. \n \t De no requerir un permiso puede reemplazar la letra por '-' (guion medio).\n");
	} else if (strcmp(instruccion, "salir") == 0){
		printf("Descripcion \n \t Termina el programa. \n");
	} else {
		printf("Ayuda no encontrada \n");
	}
		
	return 0;
}
