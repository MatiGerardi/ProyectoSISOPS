#!/bin/bash

# Compilar el archivo SantaClaus.c
gcc -o SantaClaus SantaClaus.c -lpthread

# Verificar si la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo "Compilación exitosa. Ejecutando el programa..."
    # Ejecutar el programa compilado
    ./SantaClaus
else
    echo "Error en la compilación."
fi