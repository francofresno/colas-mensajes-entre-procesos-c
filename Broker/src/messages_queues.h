/*
 ============================================================================
 Name        : Messages Queues
 Author      : Fran and Co
 Description : Header Funciones de creación de colas que en realidad no son colas
 ============================================================================
 */

#ifndef MESSAGES_QUEUES_H_
#define MESSAGES_QUEUES_H_

#include<stdio.h>
#include<stdlib.h>

typedef struct Message_queue {
    void* message;
    struct Message_queue *sig;
} t_message_queue;

struct Message_queue *raiz = NULL;
struct Message_queue *fondo = NULL;

void enqueue(void* new_message);
void* dequeue();
void free_queue();
int isEmpty();


#endif /* MESSAGES_QUEUES_H_ */
