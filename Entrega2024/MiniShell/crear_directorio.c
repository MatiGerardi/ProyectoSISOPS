#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h> // para mkdir
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Error: los parametros son invalidos. 'ayudacrear_directorio' para mas informacion.\n");
        return 0;
    }

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", argv[1], argv[2]);

    if (mkdir(fullPath, 0777) == 0) {
        printf("\033[1;32m>> Directorio creado con exito \033[0m \n"); // en verde
    } else {
        printf("\033[1;31m>> Error al crear el directorio \033[0m \n"); // en rojo
    }

    return 0;
}