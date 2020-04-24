/*
 ============================================================================
 Name        : Messages Queues
 Author      : Fran and Co
 Description : Funciones de creación de colas que enrealidad no son colas
 ============================================================================
 */

#include "messages_queues.h"


void enqueue(void* new_message)
{
    t_message_queue *nuevo;
    nuevo = malloc(sizeof(struct Message_queue));
    nuevo->message = new_message;
    nuevo->sig = NULL;

    if (isEmpty()) {
        raiz = nuevo;
        fondo = nuevo;
    } else {
        fondo->sig = nuevo;
        fondo = nuevo;
    }
}

//void* dequeue()
//{
//    if (!isEmpty()) {
//        void* message = raiz->message;
//        t_message_queue *aux = raiz;
//
//        if (raiz == fondo) {
//            raiz = NULL;
//            fondo = NULL;
//        } else {
//            raiz = raiz->sig;
//        }
//
//        free(aux);
//        return message;
//    }
//
//    return NULL;
//}

void free_queue()
{
	t_message_queue *queue = raiz;
	t_message_queue *aux;
    while (queue != NULL) {
        aux = queue;
        queue = queue->sig;
        free(aux);
    }
}

int isEmpty()
{
	return raiz == NULL;
}

