#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 50

int main(int argc, char *argv[]) {
    int file;
    char buffer[BUFFER_SIZE];
    int read_size;

    if (argc != 3) {
        printf("Error: los parametros son invalidos. 'ayuda crear_archivo' para mas informacion.\n");
        return 0;
    }

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", argv[1], argv[2]);

    file = open(fullPath, O_CREAT | O_WRONLY, 0644);
    if (file == -1) {
        printf("\033[1;31m>> Error al crear el archivo \033[0m \n"); // en rojo
    } else {
        printf("\033[1;32m>> Archivo creado con exito. Actualmente no tiene permisos. \033[0m \n"); // en verde
    }

    close(file);

    return 0;
}