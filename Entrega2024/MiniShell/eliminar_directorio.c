#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error: los parametros son invalidos. 'ayuda eliminar_directorio' para mas informacion.\n");
        return 0;
    }

    // Verificar si el directorio es el directorio raíz
    if (strcmp(argv[1], "/") == 0) {
        printf("\033[1;31m>> Error: No se puede eliminar el directorio raíz \033[0m \n"); // en rojo
        return 0;
    }

    if (rmdir(argv[1]) == 0) {
        printf("\033[1;32m>> Directorio removido con exito. \033[0m \n"); // en verde
    } else {
        printf("\033[1;31m>> Error al remover el directorio. \033[0m \n"); // en rojo
    }

    return 0;
}