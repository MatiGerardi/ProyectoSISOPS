#!/bin/bash

# Compilar el archivo TallerDeMotos.c
gcc -o TallerDeMotos TallerDeMotos.c -lpthread

# Verificar si la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo "Compilación exitosa. Ejecutando el programa..."
    # Ejecutar el programa compilado
    ./TallerDeMotos
else
    echo "Error en la compilación."
fi