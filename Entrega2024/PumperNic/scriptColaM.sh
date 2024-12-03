#!/bin/bash

# Compilar el archivo pipe.c
gcc -o cola colaMensajes.c

# Verificacion de la compilacion
if [ $? -eq 0 ]; then
    echo "Compilacion exitosa."
    # Ejecutar el programa
    ./cola
else
    echo "Error en la compilacion."
fi

# Limpiar los archivos generados
echo "Limpiando los archivos generados"
rm -f cola
