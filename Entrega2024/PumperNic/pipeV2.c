#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define cantClientes 3
#define cantClientesVip 2

#define ROJO     "\033[0;31m" //Hamburguesas simple
#define VERDE    "\033[0;32m" //Cliente
#define VERDE_CLARO "\x1b[38;5;82m" // Cliente vip
#define AMARILLO "\033[0;33m" //Papas fritas
#define MAGENTA  "\033[0;35m" //Vegano
#define CIAN     "\033[0;36m" //Cajero
#define RESET    "\033[0m" // Para restablecer el color

char* opciones[] = {
    "hamburguesa simple",     // índice 1
    "papas fritas",            // índice 3
    "menu vegano",      // índice 2
};

// Pipes para la comunicación
int pipe_cajero_burguer[2]; //Comunicación cajero a cocinero de hamburguesas
int pipe_cajero_papas[2]; //Comunicación cajero a cocinero de papas
int pipe_cajero_vegano[2]; //Comunicación cajero a cocinero de vegano

int pipe_cliente_cajero[2]; //Comunicación cliente a cajero
int pipe_clienteVip_cajero[2]; //Comunicación cliente vip a cajero

int pipe_pedido_burguer[2]; //Comunicación para los clientes que pidieron hamburguesas retiren su pedido
int pipe_pedido_papas[2];  //Comunicación para los clientes que pidieron papas retiren su pedido
int pipe_pedido_vegano[2];  //Comunicación para los clientes que pidieron vegano retiren su pedido

int cocinero_hamburguesas_simple(){
    while(1){
        int pedido;
        int ret;
        read(pipe_cajero_burguer[0], &pedido, sizeof(pedido));
        printf(ROJO"El cocinero menu hamburguesas: cocinando hamburguesas simple\n"RESET);
        printf(ROJO"El cocinero menu hamburguesas: entrega pedido\n"RESET);
        ret = 0;
        write(pipe_pedido_burguer[1], &ret, sizeof(ret));
    }
}

void cocinero_papas(int id){
    while(1){
        int pedido;
        int ret;
        read(pipe_cajero_papas[0], &pedido, sizeof(pedido));
        printf(AMARILLO"El cocinero papas %i: cocinando papas fritas\n"RESET, id);
        printf(AMARILLO"El cocinero papas %i: entrega pedido\n"RESET,id);
        ret = 1;
        write(pipe_pedido_papas[1], &ret, sizeof(ret));
    }
}

void cocinero_vegano(){
    while(1){
        int pedido;
        int ret;
        read(pipe_cajero_vegano[0], &pedido, sizeof(pedido));
        printf(MAGENTA"El cocinero menu vegano: cocinando menu vegano\n"RESET);
        printf(MAGENTA"El cocinero menu vegano: entrega pedido\n"RESET);
        ret = 2;
        write(pipe_pedido_vegano[1], &ret, sizeof(ret));
    }
}

void cajero(){
    while(1){
        int pedido;
        
        int flags = fcntl(pipe_clienteVip_cajero[0], F_GETFL, 0);
        fcntl(pipe_clienteVip_cajero[0], F_SETFL, flags | O_NONBLOCK);
        if(read(pipe_clienteVip_cajero[0], &pedido, sizeof(int)) > 0){
            //Vip
            printf(CIAN"El cajero: recibio un pedido de %s de un cliente vip\n"RESET, opciones[pedido]);
        }else{
            //No vip
            read(pipe_cliente_cajero[0], &pedido, sizeof(pedido));
            printf(CIAN"El cajero: recibio un pedido de %s de un clienten\n"RESET, opciones[pedido]);
        }

        int snd;
        switch (pedido){
            case (0):{
                //Hamburguesa simple
                snd = 0;
                printf(CIAN"El cajero: envia un mensaje al cocinero de hamburguesas simples\n"RESET);
                write(pipe_cajero_burguer[1], &snd, sizeof(snd));
                break;
            }
            case (1):{
                //Papas fritas
                snd = 1;
                printf(CIAN"El cajero: envia un mensaje a unos de los cocinero de papas fritas\n"RESET);
                write(pipe_cajero_papas[1], &snd, sizeof(snd));
                break;
            }
            case (2):{
                //Menu vegano
                snd = 2;
                printf(CIAN"El cajero: envia un mensaje al cocinero del menu vegano\n"RESET);
                write(pipe_cajero_vegano[1], &snd, sizeof(snd));
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

        printf(VERDE"El cliente %i: pide %s\n"RESET, id, opciones[pedido]);
        write(pipe_cliente_cajero[1], &pedido, sizeof(pedido));
        
        int ret;
        switch (pedido){
            case (0):{
                read(pipe_pedido_burguer[0], &ret, sizeof(ret));
                printf(VERDE"El cliente %i: recibio una  "ROJO"%s"VERDE"\n"RESET, id, opciones[ret]);
                break;
            }
            case (1):{
                read(pipe_pedido_papas[0], &ret, sizeof(ret));
                printf(VERDE"El cliente %i: recibio "AMARILLO"%s"VERDE"\n"RESET, id, opciones[ret]);
                break;
            }
            case (2):{
                read(pipe_pedido_vegano[0], &ret, sizeof(ret));
                printf(VERDE"El cliente %i: recibio el "MAGENTA"%s"VERDE"\n"RESET, id, opciones[ret]);
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

        printf(VERDE_CLARO"El cliente vip %i: pide %s\n"RESET, id, opciones[pedido]);
        write(pipe_clienteVip_cajero[1], &pedido, sizeof(pedido));
        
        int ret;
        switch (pedido){
            case (0):{
                read(pipe_pedido_burguer[0], &ret, sizeof(ret));
                printf(VERDE_CLARO"El cliente vip %i: recibio una  "ROJO"%s"VERDE_CLARO"\n"RESET, id, opciones[ret]);
                break;
            }
            case (1):{
                read(pipe_pedido_papas[0], &ret, sizeof(ret));
                printf(VERDE_CLARO"El cliente vip %i: recibio "AMARILLO"%s"VERDE_CLARO"\n"RESET, id, opciones[ret]);
                break;
            }
            case (2):{
                read(pipe_pedido_vegano[0], &ret, sizeof(ret));
                printf(VERDE_CLARO"El cliente vip %i: recibio el "MAGENTA"%s"VERDE_CLARO"\n"RESET, id, opciones[ret]);
                break;
            }
            
        }
        printf(VERDE_CLARO"El cliente vip %i se retiro del local\n"RESET, id);
    }
}

int main(){
    pipe(pipe_cajero_burguer);
    pipe(pipe_cajero_papas);
    pipe(pipe_cajero_vegano);
    pipe(pipe_cliente_cajero);
    pipe(pipe_clienteVip_cajero);
    pipe(pipe_pedido_burguer);
    pipe(pipe_pedido_papas);
    pipe(pipe_pedido_vegano);

    //Cocinero hamburguesas simples
    pid_t cocineroHamburguesas_p = fork();
    if(cocineroHamburguesas_p == 0){
        close(pipe_cajero_papas[0]);
        close(pipe_cajero_papas[1]);
        close(pipe_cajero_vegano[0]);
        close(pipe_cajero_vegano[1]);
        close(pipe_cliente_cajero[0]);
        close(pipe_cliente_cajero[1]);
        close(pipe_clienteVip_cajero[0]);
        close(pipe_clienteVip_cajero[1]);
        close(pipe_pedido_papas[0]);
        close(pipe_pedido_papas[1]);
        close(pipe_pedido_vegano[0]);
        close(pipe_pedido_vegano[1]); 

        close(pipe_cajero_burguer[1]);
        close(pipe_pedido_burguer[0]);
        cocinero_hamburguesas_simple();
        exit(0);
    }
    
    //Cocineros papas fritas
    pid_t cocineroPapa1_p = fork();
    if(cocineroPapa1_p == 0){
        close(pipe_cajero_burguer[0]);
        close(pipe_cajero_burguer[1]);
        close(pipe_cajero_vegano[0]);
        close(pipe_cajero_vegano[1]);
        close(pipe_cliente_cajero[0]);
        close(pipe_cliente_cajero[1]);
        close(pipe_clienteVip_cajero[0]);
        close(pipe_clienteVip_cajero[1]);
        close(pipe_pedido_burguer[0]);
        close(pipe_pedido_burguer[1]);
        close(pipe_pedido_vegano[0]);
        close(pipe_pedido_vegano[1]); 

        close(pipe_cajero_papas[1]);   
        close(pipe_pedido_papas[0]);
        cocinero_papas(1);   
        exit(0);
    }
    pid_t cocineroPapa2_p = fork();
    if(cocineroPapa2_p == 0){
        close(pipe_cajero_burguer[0]);
        close(pipe_cajero_burguer[1]);
        close(pipe_cajero_vegano[0]);
        close(pipe_cajero_vegano[1]);
        close(pipe_cliente_cajero[0]);
        close(pipe_cliente_cajero[1]);
        close(pipe_clienteVip_cajero[0]);
        close(pipe_clienteVip_cajero[1]);
        close(pipe_pedido_burguer[0]);
        close(pipe_pedido_burguer[1]);
        close(pipe_pedido_vegano[0]);
        close(pipe_pedido_vegano[1]);

        close(pipe_cajero_papas[1]);   
        close(pipe_pedido_papas[0]);
        cocinero_papas(2); 
        exit(0);
    }
     
    //Cocineros menu vegano
    pid_t cocineroVegano_p = fork();
    if(cocineroVegano_p == 0){
        close(pipe_cajero_burguer[0]);
        close(pipe_cajero_burguer[1]);
        close(pipe_cajero_papas[0]);
        close(pipe_cajero_papas[1]);
        close(pipe_cliente_cajero[0]);
        close(pipe_cliente_cajero[1]);
        close(pipe_clienteVip_cajero[0]);
        close(pipe_clienteVip_cajero[1]);
        close(pipe_pedido_burguer[0]);
        close(pipe_pedido_burguer[1]);
        close(pipe_pedido_papas[0]);
        close(pipe_pedido_papas[1]);

        close(pipe_cajero_vegano[1]);
        close(pipe_pedido_vegano[0]);
        cocinero_vegano();
        exit(0);
    }
    
    //Cajero
    pid_t cajero_p = fork();
    if(cajero_p == 0){
        close(pipe_pedido_burguer[0]);
        close(pipe_pedido_burguer[1]);
        close(pipe_pedido_papas[0]);
        close(pipe_pedido_papas[1]);
        close(pipe_pedido_vegano[0]);
        close(pipe_pedido_vegano[1]);    

        close(pipe_cliente_cajero[1]);
        close(pipe_clienteVip_cajero[1]);

        close(pipe_cajero_burguer[0]);
        close(pipe_cajero_papas[0]);
        close(pipe_cajero_vegano[0]);
        cajero();
        exit(0);
    }

    for(int i=0;i<cantClientes;i++){
        pid_t cliente_p = fork();
        if(cliente_p == 0){
            close(pipe_cajero_burguer[0]);
            close(pipe_cajero_burguer[1]);
            close(pipe_cajero_papas[0]);
            close(pipe_cajero_papas[1]);
            close(pipe_cajero_vegano[0]);
            close(pipe_cajero_vegano[1]);
            close(pipe_clienteVip_cajero[0]);
            close(pipe_clienteVip_cajero[1]);
            
            close(pipe_cliente_cajero[0]);

            close(pipe_pedido_burguer[1]);
            close(pipe_pedido_papas[1]);
            close(pipe_pedido_vegano[1]);  
            cliente(i+1);
            exit(0);
        }
    }
    for(int i=0;i<cantClientesVip;i++){
        pid_t cliente_vip_p = fork();
        if(cliente_vip_p == 0){
            close(pipe_cajero_papas[1]);
            close(pipe_cajero_vegano[0]);
            close(pipe_cajero_vegano[1]);
            close(pipe_cliente_cajero[0]);
            close(pipe_cliente_cajero[1]);
            
            close(pipe_clienteVip_cajero[0]);

            close(pipe_pedido_burguer[1]);
            close(pipe_pedido_papas[1]);
            close(pipe_pedido_vegano[1]);  
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