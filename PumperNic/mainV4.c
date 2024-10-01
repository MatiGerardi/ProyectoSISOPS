#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_CLIENTES 10
#define HAM_SIMPLE 1
#define VEGAN 2
#define FRTAS 3

typedef struct {
    int id;
    int ham;
    int veg;
    int fritas;
    int vip;
} Pedido;

typedef struct Cliente {
    Pedido pedido;
    struct Cliente* next;
} Cliente;

Cliente* cola_clientes = NULL;
Cliente* cola_pedidos = NULL;
pthread_mutex_t mutex_clientes = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pedidos = PTHREAD_MUTEX_INITIALIZER;
int pipe_fd[2];

void encolar_cliente(Cliente** cola, Pedido pedido) {
    Cliente* nuevo_cliente = (Cliente*)malloc(sizeof(Cliente));
    nuevo_cliente->pedido = pedido;
    nuevo_cliente->next = NULL;

    pthread_mutex_lock(&mutex_clientes);
    if (*cola == NULL) {
        *cola = nuevo_cliente;
    } else {
        Cliente* temp = *cola;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = nuevo_cliente;
    }
    pthread_mutex_unlock(&mutex_clientes);
}

Pedido desencolar_cliente(Cliente** cola) {
    pthread_mutex_lock(&mutex_clientes);
    Cliente* temp = *cola;
    Pedido pedido = temp->pedido;
    *cola = (*cola)->next;
    free(temp);
    pthread_mutex_unlock(&mutex_clientes);
    return pedido;
}

void atender_cliente(int id) {
    Pedido pedido = {id, rand() % 2, rand() % 2, rand() % 2, rand() % 2};
    if (pedido.ham == 0 && pedido.veg == 0 && pedido.fritas == 0) {
        pedido.fritas = 1;
    }
    encolar_cliente(&cola_clientes, pedido);
    write(pipe_fd[1], &pedido, sizeof(Pedido));
}

void* recepcion(void* arg) {
    Pedido pedido;
    for (int i = 0; i < NUM_CLIENTES; i++) {
        read(pipe_fd[0], &pedido, sizeof(Pedido));
        printf("Recepción atendiendo a un cliente...\n");

        pthread_mutex_lock(&mutex_pedidos);
        if (pedido.vip) {
            Cliente* nuevo_cliente = (Cliente*)malloc(sizeof(Cliente));
            nuevo_cliente->pedido = pedido;
            nuevo_cliente->next = cola_pedidos;
            cola_pedidos = nuevo_cliente;
        } else {
            encolar_cliente(&cola_pedidos, pedido);
        }
        pthread_mutex_unlock(&mutex_pedidos);
    }
    return NULL;
}

void* empleado_ham(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex_pedidos);
        if (cola_pedidos != NULL && cola_pedidos->pedido.ham == 1) {
            Pedido pedido = desencolar_cliente(&cola_pedidos);
            pthread_mutex_unlock(&mutex_pedidos);
            printf("Empleado de hamburguesas preparando pedido %d\n", pedido.id);
            sleep(1); // Simula el tiempo de preparación
            pedido.ham = 0;
            encolar_cliente(&cola_pedidos, pedido);
        } else {
            pthread_mutex_unlock(&mutex_pedidos);
        }
        sleep(1);
    }
    return NULL;
}

void* empleado_veg(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex_pedidos);
        if (cola_pedidos != NULL && cola_pedidos->pedido.veg == 1) {
            Pedido pedido = desencolar_cliente(&cola_pedidos);
            pthread_mutex_unlock(&mutex_pedidos);
            printf("Empleado de veganos preparando pedido %d\n", pedido.id);
            sleep(1); // Simula el tiempo de preparación
            pedido.veg = 0;
            encolar_cliente(&cola_pedidos, pedido);
        } else {
            pthread_mutex_unlock(&mutex_pedidos);
        }
        sleep(1);
    }
    return NULL;
}

void* empleado_fritas(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex_pedidos);
        if (cola_pedidos != NULL && cola_pedidos->pedido.fritas == 1) {
            Pedido pedido = desencolar_cliente(&cola_pedidos);
            pthread_mutex_unlock(&mutex_pedidos);
            printf("Empleado de fritas preparando pedido %d\n", pedido.id);
            sleep(1); // Simula el tiempo de preparación
            pedido.fritas = 0;
            encolar_cliente(&cola_pedidos, pedido);
        } else {
            pthread_mutex_unlock(&mutex_pedidos);
        }
        sleep(1);
    }
    return NULL;
}

int main() {
    pipe(pipe_fd);
    pthread_t recepcion_thread, ham_thread, veg_thread, fritas_thread1, fritas_thread2;

    pthread_create(&recepcion_thread, NULL, recepcion, NULL);
    pthread_create(&ham_thread, NULL, empleado_ham, NULL);
    pthread_create(&veg_thread, NULL, empleado_veg, NULL);
    pthread_create(&fritas_thread1, NULL, empleado_fritas, NULL);
    pthread_create(&fritas_thread2, NULL, empleado_fritas, NULL);

    for (int i = 0; i < NUM_CLIENTES; i++) {
        atender_cliente(i);
    }

    pthread_join(recepcion_thread, NULL);
    pthread_join(ham_thread, NULL);
    pthread_join(veg_thread, NULL);
    pthread_join(fritas_thread1, NULL);
    pthread_join(fritas_thread2, NULL);

    return 0;
}