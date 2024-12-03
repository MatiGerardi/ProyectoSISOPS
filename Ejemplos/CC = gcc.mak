# Variables para el compilador y opciones
CC = gcc
CFLAGS = -pthread -Wall -g

# Nombre del ejecutable final
EXEC = programa

# Objetivos del ejecutable y los archivos objeto
$(EXEC): SantaClaus.o TallerDeMotos.o
	$(CC) $(CFLAGS) -o $(EXEC) SantaClaus.o TallerDeMotos.o

# Compilación de SantaClaus.c
SantaClaus.o: SantaClaus.c
	$(CC) $(CFLAGS) -c SantaClaus.c

# Compilación de TallerDeMotos.c
TallerDeMotos.o: TallerDeMotos.c
	$(CC) $(CFLAGS) -c TallerDeMotos.c

# Objetivo para limpiar los archivos generados
clean:
	rm -f *.o $(EXEC)
