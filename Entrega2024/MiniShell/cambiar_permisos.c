#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int parse_permissions(const char *perm_str) {
    int permissions = 0;

    if (perm_str[0] == 'r') permissions |= S_IRUSR;
    if (perm_str[1] == 'w') permissions |= S_IWUSR;
    if (perm_str[2] == 'x') permissions |= S_IXUSR;
    if (perm_str[3] == 'r') permissions |= S_IRGRP;
    if (perm_str[4] == 'w') permissions |= S_IWGRP;
    if (perm_str[5] == 'x') permissions |= S_IXGRP;
    if (perm_str[6] == 'r') permissions |= S_IROTH;
    if (perm_str[7] == 'w') permissions |= S_IWOTH;
    if (perm_str[8] == 'x') permissions |= S_IXOTH;

    return permissions;
}

int main(int argc, char * argv[]){
    if (argc != 3) {
        printf("Error: los parametros son invalidos. 'ayuda cambiar_permisos' para mas informacion.\n");
        return 0;
    }

    char *file_path = argv[1];
    char *perm_str = argv[2];

    if (strlen(perm_str) != 9) {
        printf("\033[1;31m>> Error: formato de permisos invalido. Use 'rwxrwxrwx'. \033[0m \n");
        return 0;
    }

    int permissions = parse_permissions(perm_str);

    int resultado = chmod(file_path, permissions);

    if (resultado < 0) {
        perror("\033[1;31m>> Error al cambio de permisos\033[0m"); // en rojo
    } else {
        printf("\033[1;32m>> Permisos cambiados con exito \033[0m \n"); // en verde
    }

    return 0;
}
