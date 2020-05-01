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
#include <stdint.h>
#include<commons/collections/queue.h>

typedef struct
{
	uint32_t ID;
	uint32_t ID_correlativo;
	t_list* suscribers_sent;
	t_list* suscribers_ack;
	void* message;
} t_data;

typedef struct
{
	char* ip_proceso;
	char* puerto_proceso;
} t_subscriber;

t_queue* create_message_queue();
void push_message_queue(t_queue* queue, uint32_t ID, uint32_t ID_correlativo, void* message);
t_data* pop_message_queue(t_queue* queue);
void inform_message_sent_to(t_data* data, t_subscriber* subscriber);
void inform_message_ack_from(t_data* data, t_subscriber* subscriber);
t_data* find_message_by_id(t_queue* queue, uint32_t id);
t_data* find_message_by_id_correlativo(t_queue* queue, uint32_t id);
void remove_message_by_id(t_queue* queue, uint32_t id);
void remove_message_by_id_correlativo(t_queue* queue, uint32_t id);
void element_destroyer(void* data);
int size_message_queue(t_queue* queue);
int is_empty_message_queue(t_queue* queue);
void free_message_queue(t_queue* queue);

#endif /* MESSAGES_QUEUES_H_ */
