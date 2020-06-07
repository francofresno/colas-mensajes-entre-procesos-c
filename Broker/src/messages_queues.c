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

t_enqueued_message* push_message_queue(t_queue* queue, uint32_t ID, uint32_t ID_correlativo, void* message, pthread_mutex_t mutex)
{
	t_enqueued_message* data = malloc(sizeof(*data));
	data->suscribers_ack = list_create();
	data->suscribers_informed = list_create();
	data->ID = ID;
	data->ID_correlativo = ID_correlativo;
	data->message = message;

	pthread_mutex_lock(&mutex);
	queue_push(queue, (void*) data);
	pthread_mutex_unlock(&mutex);

	return data;
}

t_enqueued_message* pop_message_queue(t_queue* queue, pthread_mutex_t mutex)
{
	pthread_mutex_lock(&mutex);
	t_enqueued_message* message = (t_enqueued_message*) queue_pop(queue);
	pthread_mutex_unlock(&mutex);
	return message;
}

t_enqueued_message* get_message_by_index(t_queue* queue, int index)
{
	t_enqueued_message* message = (t_enqueued_message*) list_get(queue->elements, index);
	return message;
}

void inform_message_sent_to(t_enqueued_message* message, t_subscriber* subscriber)
{
	list_add(message->suscribers_informed, (void*)subscriber);
}

void inform_message_ack_from(t_enqueued_message* message, t_subscriber* subscriber)
{
	list_add(message->suscribers_ack, (void*)subscriber);
}

int is_same_id(uint32_t data_id, uint32_t id)
{
	return data_id == id;
}

t_enqueued_message* find_message_by_id(t_queue* queue, uint32_t id)
{
	t_link_element *element = queue->elements->head;
	t_enqueued_message* message = (t_enqueued_message*) (queue->elements->head->data);

	while(element != NULL && !is_same_id(message->ID, id)) {
		element = element->next;
		message = element == NULL ? NULL : element->data;
	}

	return message;
}

t_enqueued_message* find_message_by_id_correlativo(t_queue* queue, uint32_t id)
{
	t_link_element *element = queue->elements->head;
	t_enqueued_message* message = (t_enqueued_message*) (queue->elements->head->data);

	while(element != NULL && !is_same_id(message->ID_correlativo, id)) {
		element = element->next;
		message = element == NULL ? NULL : element->data;
	}

	return message;
}

void remove_message_by_id(t_queue* queue, uint32_t id)
{
	t_link_element *element = queue->elements->head;
	t_enqueued_message* message = (t_enqueued_message*) (queue->elements->head->data);
	int position = 0;

	while(element != NULL && !is_same_id(message->ID, id)) {
		element = element->next;
		message = element == NULL ? NULL : element->data;
		position++;
	}

	if (message != NULL) {
		message = (t_enqueued_message*) list_remove(queue->elements, position);
		element_destroyer_mq((void*) message);
	}
}

void remove_message_by_id_correlativo(t_queue* queue, uint32_t id)
{
	t_link_element *element = queue->elements->head;
	t_enqueued_message* message = (t_enqueued_message*) (queue->elements->head->data);
	int position = 0;

	while(element != NULL && !is_same_id(message->ID_correlativo, id)) {
		element = element->next;
		message = element == NULL ? NULL : element->data;
		position++;
	}

	if (message != NULL) {
		message = (t_enqueued_message*) list_remove(queue->elements, position);
		element_destroyer_mq((void*) message);
	}
}

void element_destroyer_mq(void* message)
{
	t_enqueued_message* message_enqueue = (t_enqueued_message*) message;
	free_subscribers_list(message_enqueue->suscribers_ack);
	free_subscribers_list(message_enqueue->suscribers_informed);
	free(message_enqueue->message);
	free(message_enqueue);
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

void unsubscribe_process(t_list* subscribers, t_subscriber* subscriber, pthread_mutex_t mutex)
{
	pthread_mutex_lock(&mutex);

	int index = get_index_of_subscriber(subscribers, subscriber->id_suscriptor);
	if (index != -1)
		list_remove(subscribers, index);

	pthread_mutex_unlock(&mutex);
}

int get_index_of_subscriber(t_list* subscribers, uint32_t id_subscriber)
{
	if (subscribers->head == NULL)
		return -1;

	t_link_element *element = subscribers->head;
	t_subscriber* subscriber_listed = (t_subscriber*) (subscribers->head->data);

	int index = 0;
	while(element != NULL) {
		if (is_same_id(subscriber_listed->id_suscriptor, id_subscriber))
			return index;

		element = element->next;
		subscriber_listed = element == NULL ? NULL : element->data;
		index++;
	}

	return -1;
}

t_subscriber* get_subscriber_by_id(t_list* subscribers, uint32_t id_subscriber)
{
	int index = get_index_of_subscriber(subscribers, id_subscriber);

	return index >= 0 ? (t_subscriber*) list_get(subscribers, index) : NULL;
}

int isSubscriberListed(t_list* subscribers, uint32_t id_subscriber)
{
	return get_index_of_subscriber(subscribers, id_subscriber) >= 0;
}

void add_new_informed_subscriber_to_mq(t_enqueued_message* messages_in_queue[], uint32_t number_of_mensajes, t_subscriber* subscriber) {
	for (int i=0; i < number_of_mensajes; i++) {
		if (!isSubscriberListed(messages_in_queue[i]->suscribers_informed, subscriber->id_suscriptor)) {
			list_add(messages_in_queue[i]->suscribers_informed, subscriber); //TODO mutex?
		}
	}
}

void add_new_ack_suscriber_to_mq(t_enqueued_message* messages_in_queue[], uint32_t number_of_mensajes, t_subscriber* subscriber) {
	for (int i=0; i < number_of_mensajes; i++) {
		if (!isSubscriberListed(messages_in_queue[i]->suscribers_ack, subscriber->id_suscriptor)) {
			list_add(messages_in_queue[i]->suscribers_ack, subscriber); //TODO mutex?
		}
	}
}


void free_subscribers_list(t_list* subscribers)
{
	list_destroy(subscribers);
}


