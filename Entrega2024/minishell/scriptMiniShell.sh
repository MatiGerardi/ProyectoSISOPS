#!/bin/bash

# Compilar los archivos
gcc -o listar_directorio listar_directorio.c -Wall
gcc -o crear_directorio crear_directorio.c -Wall
gcc -o mostrar_archivo mostrar_archivo.c -Wall
gcc -o eliminar_directorio eliminar_directorio.c -Wall
gcc -o crear_archivo crear_archivo.c -Wall
gcc -o cambiar_permisos cambiar_permisos.c -Wall
gcc -o manual manual.c -Wall

gcc -o main main.c -Wall
echo "Compilación completada exitosamente."
./main
