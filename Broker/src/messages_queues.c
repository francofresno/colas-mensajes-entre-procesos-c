/*
 ============================================================================
 Name        : Messages Queues
 Author      : Fran and Co
 Description : Funciones de creación de colas que enrealidad no son colas
 ============================================================================
 */

#include "messages_queues.h"

/////////////////////////////////
// ---- Colas de mensajes ---- //
/////////////////////////////////

t_queue* create_message_queue()
{
	return queue_create();
}

void push_message_queue(t_queue* queue, uint32_t ID, uint32_t ID_correlativo, void* message, t_list* suscribers_sent, pthread_mutex_t mutex)
{
	t_data* data = malloc(sizeof(data));
	data->suscribers_ack = list_create();
	data->suscribers_sent = suscribers_sent;
	data->ID = ID;
	data->ID_correlativo = ID_correlativo;
	data->message = message;

	pthread_mutex_lock(&mutex);
	queue_push(queue, (void*) data);
	pthread_mutex_unlock(&mutex);
}

t_data* pop_message_queue(t_queue* queue, pthread_mutex_t mutex)
{
	pthread_mutex_lock(&mutex);
	t_data* data = (t_data*) queue_pop(queue);
	pthread_mutex_unlock(&mutex);
	return data;
}

t_data* get_message_by_index(t_queue* queue, int index)
{
	t_data* data = (t_data*) list_get(queue->elements, index);
	return data;
}

void inform_message_sent_to(t_data* data, t_subscriber* subscriber)
{
	list_add(data->suscribers_sent, (void*)subscriber);
}

void inform_message_ack_from(t_data* data, t_subscriber* subscriber)
{
	list_add(data->suscribers_ack, (void*)subscriber);
}

bool is_same_id(uint32_t data_id, uint32_t id)
{
	return data_id == id;
}

t_data* find_message_by_id(t_queue* queue, uint32_t id)
{
	t_link_element *element = queue->elements->head;
	t_data* data = (t_data*) (queue->elements->head->data);

	while(element != NULL && !is_same_id(data->ID, id)) {
		element = element->next;
		data = element == NULL ? NULL : element->data;
	}

	return data;
}

t_data* find_message_by_id_correlativo(t_queue* queue, uint32_t id)
{
	t_link_element *element = queue->elements->head;
	t_data* data = (t_data*) (queue->elements->head->data);

	while(element != NULL && !is_same_id(data->ID_correlativo, id)) {
		element = element->next;
		data = element == NULL ? NULL : element->data;
	}

	return data;
}

void remove_message_by_id(t_queue* queue, uint32_t id)
{
	t_link_element *element = queue->elements->head;
	t_data* data = (t_data*) (queue->elements->head->data);
	int position = 0;

	while(element != NULL && !is_same_id(data->ID, id)) {
		element = element->next;
		data = element == NULL ? NULL : element->data;
		position++;
	}

	if (data != NULL) {
		data = (t_data*) list_remove(queue->elements, position);
		element_destroyer_mq((void*) data);
	}
}

void remove_message_by_id_correlativo(t_queue* queue, uint32_t id)
{
	t_link_element *element = queue->elements->head;
	t_data* data = (t_data*) (queue->elements->head->data);
	int position = 0;

	while(element != NULL && !is_same_id(data->ID_correlativo, id)) {
		element = element->next;
		data = element == NULL ? NULL : element->data;
		position++;
	}

	if (data != NULL) {
		data = (t_data*) list_remove(queue->elements, position);
		element_destroyer_mq((void*) data);
	}
}

void element_destroyer_mq(void* data)
{
	t_data* data_mq = (t_data*) data;
	free_subscribers_list(data_mq->suscribers_ack);
	free_subscribers_list(data_mq->suscribers_sent);
	free(data_mq->message);
	free(data_mq);
}

int size_message_queue(t_queue* queue)
{
	return queue_size(queue);
}

int is_empty_message_queue(t_queue* queue)
{
	return queue_is_empty(queue);
}

void free_message_queue(t_queue* queue)
{
	queue_destroy_and_destroy_elements(queue, element_destroyer_mq);
}

/////////////////////////////////////
// ---- Suscriptores de colas ---- //
/////////////////////////////////////

void subscribe_process(t_list* subscribers, t_subscriber* subscriber, pthread_mutex_t mutex)
{
	pthread_mutex_lock(&mutex);
	list_add(subscribers, (void*) subscriber);
	pthread_mutex_unlock(&mutex);
}

void free_subscribers_list(t_list* subscribers)
{
	list_destroy(subscribers);
}


