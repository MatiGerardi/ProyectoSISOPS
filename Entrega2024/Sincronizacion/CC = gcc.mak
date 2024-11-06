CC = gcc
CFLAGS = -pthread -Wall
TARGET = SantaClaus TallerDeMotos

all: $(TARGET)

SantaClaus: SantaClaus.c
    $(CC) $(CFLAGS) -o $(TARGET) SantaClaus.c

TallerDeMotos: TallerDeMotos.c
    $(CC) $(CFLAGS) -o TallerDeMotos TallerDeMotos.c

clean:
    rm -f $(TARGET)