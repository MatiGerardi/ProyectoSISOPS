#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char * argv[]){

	if (argv[1] == NULL){ //solo se ingreso "manual" (sin parametros)
		printf("Para ver el manual de un comando, introducir 'manual <tipo_comando>' \n");
		return 0;
	}
	
	if(argv[2] != NULL){ //se ingreso mas de un parametro
		printf("Error: Parametros no validos. Consultar 'manual' para mas informacion.\n");
		return 0;
	}
	
	char * instruccion = argv[1];
	
	if (strcmp(instruccion, "crear_un_directorio") == 0){
		
		printf("Descripcion \n \t Se encarga de crear un directorio en la ubicacion donde se encuentra la minishell. \n \t En caso de que se pueda crear, se muestra un mensaje de exito. \n \t En caso contrario se muestra que hubo un fallo. \nFormato \n \t crear_un_directorio [nombre_directorio]\n");
		
	}
	
	else if (strcmp(instruccion, "eliminar_un_directorio") == 0){
		
		printf("Descripcion \n \t Comando encargado de remover un directorio en la ubicacion donde se encuentra la minishell. \n \t En caso de que se pueda eliminar el directorio, se mostrara un mensaje de exito. \n \t En caso contrario, se mostrar un mensaje de fallo. \nFormato \n \t eliminar_un_directorio [nombre_directorio]\n");
		
	}
	
	else if (strcmp(instruccion, "crear_un_archivo") == 0){
		
		printf("Descripcion \n \t Comando encargado de crear un archivo en el directorio en donde se encuentra ubicado.\n \t En caso de que se pueda crear el archivo, se mostrara un mensaje de exito. \n \t caso contrario, se mostrara un mensaje de fallo. \nFormato \n \t crear_un_archivo [nombre_archivo]\n");
		
	}
	
	else if (strcmp(instruccion, "listar_directorio") == 0){
		
		printf("Descripcion \n \t Comando encargado de listar el contenido de un directorio. \n \t En caso de que se pueda listar el contenido del directorio, se mostrara por pantalla. \n \t En caso contrario, se mostrar un mensaje de fallo.\nFormato \n \t listar_directorio [ruta_directorio]\n");
		
	}
	
	else if (strcmp(instruccion, "mostrar_archivo") == 0){
		
		printf("Descripcion \n \t Comando encargado de mostrar el contenido de un archivo. \n \t En caso de que se pueda mostrar el archivo, se visualizara por pantalla. \n \t En caso contrario, se mostrar un mensaje de fallo. \nFormato \n \t mostrar_archivo [ruta_archivo]\n");
		
	}
	
	else if (strcmp(instruccion, "cambiar_permisos") == 0){
		
		printf("Descripcion \n \t Comando encargado de cambiar los permisos de un archivo en especifico. \n \t En caso de que se pueda cambiar los permisos, se mostrara un mensaje de exito. \n \t En caso contrario, se mostrar un mensaje de fallo. \nFormato \n \t cambiar_permisos [ruta_archivo][permisoDueño, permisoGrupo, permisoOtros] \nDonde [permisosX] poseen el siguiente formato \n \t 1 Ejecutar \n \t 2 Escribir \n \t 4 Leer \n \t Permitiendose la suma de los numeros, es decir, 7 (1+2+4), permitira leer escribir y ejecutar.\n");
		
	}

	else{

		printf("No existe manual para el comando ingresado \n");
		
	}
		
	return 0;
}