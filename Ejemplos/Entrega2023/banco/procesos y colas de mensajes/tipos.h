#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <time.h>

#define MAX_COLA_ENTRADA 30
#define MAX_COLA_CLIENTE 15

#define CONT_MESA_PRINCIPAL 1

#define CONT_MESA_COMUN 2
#define COLA_MESA_COMUN 3
#define TURNO_COMUN_INICIO 4
#define TURNO_COMUN_FIN 5

#define CONT_MESA_EMPRESARIAL 6
#define COLA_MESA_EMPRESARIAL 7
#define TURNO_EMPRESARIAL_INICIO 8
#define TURNO_EMPRESARIAL_FIN 9

#define CONT_MESA_POLITICO 10
#define COLA_MESA_POLITICO 11
#define TURNO_POLITICO_INICIO 12
#define TURNO_POLITICO_FIN 13
