#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define cantClientes 5
#define cantClientesVip 2

#define ROJO     "\033[0;31m" //Hamburguesas simple
#define VERDE    "\033[0;32m" //Cliente
#define VERDE_CLARO "\x1b[38;5;82m" // Cliente vip
#define AMARILLO "\033[0;33m" //Papas fritas
#define MAGENTA  "\033[0;35m" //Vegano
#define CIAN     "\033[0;36m" //Cajero
#define RESET    "\033[0m" // Para restablecer el color

#define KEY ((key_t) (1243)) 

char* opciones[] = {
    "hamburguesa simple",     // índice 1
    "papas fritas",            // índice 3
    "menu vegano",      // índice 2
};

//Tipos mensajes
#define PEDIR_PEDIDO_VIP 1
#define PEDIR_PEDIDO 2

#define COCINAR_HAMBURGUESA 3
#define COCINAR_PAPAS_FRITAS 4
#define COCINAR_MENU_VEGANO 5

#define RETIRAR_HAMBURGUESA 6
#define RETIRAR_PAPAS_FRITAS 7
#define RETIRAR_MENU_VEGANO 8

typedef struct {
    long tipo;
    int pedido;
    int clientId;
    int vip;
} msg_t;

#define MSG_SIZE sizeof(msg_t)-sizeof(long)
int msgid;

int cocinero_hamburguesas_simple(){
    while(1){
        msg_t pedido;
        msg_t ret;
        msgrcv(msgid, &pedido, MSG_SIZE, COCINAR_HAMBURGUESA, 0);
        printf(ROJO"El cocinero menu hamburguesas: cocinando hamburguesas simple\n"RESET);
        printf(ROJO"El cocinero menu hamburguesas: entrega pedido\n"RESET);
        ret.clientId = pedido.clientId;
        ret.pedido = pedido.pedido;
        ret.vip = pedido.vip;
        ret.tipo = RETIRAR_HAMBURGUESA;
        msgsnd(msgid, &ret, MSG_SIZE, 0);
    }
}

void cocinero_papas(int id){
    while(1){
        msg_t pedido;
        msg_t ret;
        msgrcv(msgid, &pedido, MSG_SIZE, COCINAR_PAPAS_FRITAS, 0);
        printf(AMARILLO"El cocinero papas %i: cocinando papas fritas\n"RESET, id);
        printf(AMARILLO"El cocinero papas %i: entrega pedido\n"RESET,id);

        ret.clientId = pedido.clientId;
        ret.pedido = pedido.pedido;
        ret.vip = pedido.vip;
        ret.tipo = RETIRAR_PAPAS_FRITAS;
        msgsnd(msgid, &ret, MSG_SIZE, 0);
    }
}

void cocinero_vegano(){
    while(1){
        msg_t pedido;
        msg_t ret;
        msgrcv(msgid, &pedido, MSG_SIZE, COCINAR_MENU_VEGANO, 0);

        printf(MAGENTA"El cocinero menu vegano: cocinando menu vegano\n"RESET);
        printf(MAGENTA"El cocinero menu vegano: entrega pedido\n"RESET);

        ret.clientId = pedido.clientId;
        ret.pedido = pedido.pedido;
        ret.vip = pedido.vip;
        ret.tipo = RETIRAR_MENU_VEGANO;
        msgsnd(msgid, &ret, MSG_SIZE, 0);
    }
}

void cajero(){
    while(1){
        int pedido;
        
        msg_t msg_pedido;
        msgrcv(msgid, &msg_pedido, MSG_SIZE, -2, 0);
        pedido = msg_pedido.pedido;
        if(msg_pedido.vip == 1){
            //Vip
            printf(CIAN"El cajero: recibio un pedido de %s de un cliente vip %i\n"RESET, opciones[pedido], msg_pedido.clientId);
        }else{
            //No vip
            printf(CIAN"El cajero: recibio un pedido de %s de un cliente %i\n"RESET, opciones[pedido], msg_pedido.clientId);
        }

        msg_t snd;
        snd.clientId = msg_pedido.clientId;
        snd.pedido = msg_pedido.pedido;
        snd.vip = msg_pedido.vip;
        switch (pedido){
            case (0):{
                //Hamburguesa simple
                snd.tipo = COCINAR_HAMBURGUESA;
                printf(CIAN"El cajero: envia un mensaje al cocinero de "ROJO"hamburguesas simples"CIAN"\n"RESET);
                msgsnd(msgid, &snd, MSG_SIZE, 0);
                break;
            }
            case (1):{
                //Papas fritas
                snd.tipo = COCINAR_PAPAS_FRITAS;
                printf(CIAN"El cajero: envia un mensaje a unos de los cocinero de papas fritas\n"RESET);
                msgsnd(msgid, &snd, MSG_SIZE, 0);
                break;
            }
            case (2):{
                //Menu vegano
                snd.tipo = COCINAR_MENU_VEGANO;
                printf(CIAN"El cajero: envia un mensaje al cocinero del menu vegano\n"RESET);
                msgsnd(msgid, &snd, MSG_SIZE, 0);
                break;
            }
        }

    }
}
void cliente(int id){
    srand(time(NULL)+getpid());
    printf(VERDE"Cliente %i: Llega al local\n"RESET,id);
    //Simulacion del espera, 1 cada de 10 veces se ira el cliente
    int paciencia = rand()%10+1;
    
    if(paciencia == 1){
        printf(VERDE"El cliente %i: decide irse y volver mas tarde\n"RESET, id);
        return;
    }else{
        //Selecciona la comida
        int pedido=rand()%3;
        msg_t msg_pedido;

        msg_pedido.tipo = PEDIR_PEDIDO;
        msg_pedido.pedido = pedido;
        msg_pedido.clientId = id;
        msg_pedido.vip = 0;

        printf(VERDE"El cliente %i: pide %s\n"RESET, id, opciones[pedido]);
        msgsnd(msgid, &msg_pedido, MSG_SIZE, 0);
        
        int ret;
        switch (pedido){
            case (0):{
                msgrcv(msgid, &ret, MSG_SIZE,RETIRAR_HAMBURGUESA,0);
                printf(VERDE"El cliente %i: recibio una  "ROJO"%s"VERDE"\n"RESET, id, opciones[pedido]);
                break;
            }
            case (1):{
                msgrcv(msgid, &ret, MSG_SIZE, RETIRAR_PAPAS_FRITAS,0);
                printf(VERDE"El cliente %i: recibio "AMARILLO"%s"VERDE"\n"RESET, id, opciones[pedido]);
                break;
            }
            case (2):{
                msgrcv(msgid, &ret, MSG_SIZE, RETIRAR_MENU_VEGANO,0);
                printf(VERDE"El cliente %i: recibio el "MAGENTA"%s"VERDE"\n"RESET, id, opciones[pedido]);
                break;
            }
            
        }
        printf(VERDE"El cliente %i se retiro del local\n"RESET, id);
    }
}

void cliente_vip(int id){
    srand(time(NULL)+getpid());
    printf(VERDE_CLARO"Cliente vip %i: Llega al local\n"RESET,id);
    //Simulacion del espera, 1 cada de 10 veces se ira el cliente
    int paciencia = rand()%10+1;
    
    if(paciencia == 1){
        printf(VERDE_CLARO"El cliente vip %i: decide irse y volver mas tarde\n"RESET, id);
        return;
    }else{
        //Selecciona la comida
        int pedido=rand()%3;
        msg_t msg_pedido;

        //Valor negativo para que sea con prioridad
        msg_pedido.tipo = PEDIR_PEDIDO_VIP;
        msg_pedido.pedido = pedido;
        msg_pedido.clientId = id;
        msg_pedido.vip = 1;
        printf(VERDE_CLARO"El cliente vip %i: pide %s\n"RESET, id, opciones[pedido]);
        msgsnd(msgid, &msg_pedido, MSG_SIZE, 0);
        
        int ret;
        switch (pedido){
            case (0):{
                msgrcv(msgid, &ret, MSG_SIZE,RETIRAR_HAMBURGUESA, 0);
                printf(VERDE_CLARO"El cliente vip %i: recibio una  "ROJO"%s"VERDE_CLARO"\n"RESET, id, opciones[pedido]);
                break;
            }
            case (1):{
                msgrcv(msgid, &ret, MSG_SIZE, RETIRAR_PAPAS_FRITAS, 0);
                printf(VERDE_CLARO"El cliente vip %i: recibio "AMARILLO"%s"VERDE_CLARO"\n"RESET, id, opciones[pedido]);
                break;
            }
            case (2):{
                msgrcv(msgid, &ret, MSG_SIZE, RETIRAR_MENU_VEGANO, 0);
                printf(VERDE_CLARO"El cliente vip %i: recibio el "MAGENTA"%s"VERDE_CLARO"\n"RESET, id, opciones[pedido]);
                break;
            }
            
        }
        printf(VERDE_CLARO"El cliente vip %i se retiro del local\n"RESET, id);
    }
}

int main(){
    msgid = msgget(KEY, IPC_CREAT | 0666);
    msgctl(msgid, IPC_RMID, NULL);
    msgid = msgget(KEY, IPC_CREAT | 0666);

    //Cocinero hamburguesas simples
    pid_t cocineroHamburguesas_p = fork();
    if(cocineroHamburguesas_p == 0){
        cocinero_hamburguesas_simple();
        exit(0);
    }
    
    //Cocineros papas fritas
    pid_t cocineroPapa1_p = fork();
    if(cocineroPapa1_p == 0){

        cocinero_papas(1);   
        exit(0);
    }
    pid_t cocineroPapa2_p = fork();
    if(cocineroPapa2_p == 0){
        cocinero_papas(2); 
        exit(0);
    }
     
    //Cocineros menu vegano
    pid_t cocineroVegano_p = fork();
    if(cocineroVegano_p == 0){
        cocinero_vegano();
        exit(0);
    }
    
    //Cajero
    pid_t cajero_p = fork();
    if(cajero_p == 0){
        cajero();
        exit(0);
    }

    for(int i=0;i<cantClientes;i++){
        pid_t cliente_p = fork();
        if(cliente_p == 0){
            cliente(i+1);
            exit(0);
        }
    }
    for(int i=0;i<cantClientesVip;i++){
        pid_t cliente_vip_p = fork();
        if(cliente_vip_p == 0){
            cliente_vip(i+1);
            exit(0);
        }
    }


    
    for(int i=0;i<cantClientes+ cantClientesVip;i++){
        wait(NULL);
    }
    kill(cocineroHamburguesas_p, SIGKILL);
    kill(cocineroPapa1_p, SIGKILL);
    kill(cocineroPapa2_p, SIGKILL);
    kill(cocineroVegano_p, SIGKILL);
    kill(cajero_p, SIGKILL);
    printf("Termino el programa\n");
    return 0;
}