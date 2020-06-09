/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Proceso Broker
 ============================================================================
 */

#include "broker.h"

//TODO notify msg used

int main(void) {

	init_config();
	init_memory();
	init_logger();
	init_message_queues();
	init_suscriber_lists();
	int server_socket = init_server();

	printf("broker!\n");
	fflush(stdout);

	while(1) {
		int potential_client_socket = esperar_cliente(server_socket);
		if(potential_client_socket > 0) {
			int* client_socket = (int*) malloc(sizeof(int));
			*client_socket = potential_client_socket;
			printf("Llegó un cliente al socket: %d \n", *client_socket);
			fflush(stdout);
			pthread_create(&thread,NULL,(void*)serve_client,client_socket);
			pthread_detach(thread);
		}
	}

}

void serve_client(int* socket_cliente)
{
	char* nombre_recibido = NULL;
	t_paquete* paquete_recibido = recibir_paquete(*socket_cliente, &nombre_recibido);

	if (paquete_recibido->codigo_operacion == SUSCRIPCION) {
		process_suscription((t_suscripcion_msg*) (paquete_recibido->mensaje), *socket_cliente);
	} else {
		process_new_message(paquete_recibido->codigo_operacion, paquete_recibido->id_correlativo, paquete_recibido->mensaje, *socket_cliente);
	}

	free_paquete_recibido(nombre_recibido, paquete_recibido);
	liberar_conexion(*socket_cliente);
	free(socket_cliente);
}

void process_suscription(t_suscripcion_msg* subscription_msg, int socket_suscriptor)
{
	t_subscriber* subscriber;

	t_list* suscriptores = SUSCRIPTORES_MENSAJES[subscription_msg->tipo_cola];
	t_queue* queue = COLAS_MENSAJES[subscription_msg->tipo_cola];
	pthread_mutex_t mutex = MUTEX_SUSCRIPTORES[subscription_msg->tipo_cola];
	uint32_t cantidad_mensajes = size_message_queue(queue); // Como maximo sera del size de la lista, este valor es modificado en responder_a_suscriptor_nuevo
	t_list* mensajes_encolados = list_create();

	if (isSubscriberListed(suscriptores, subscription_msg->id_proceso)) {
		t_subscriber* subscriber_listed = get_subscriber_by_id(suscriptores, subscription_msg->id_proceso);
		subscriber_listed->socket_suscriptor = socket_suscriptor;
		subscriber_listed->activo = 1;
		reply_to_new_subscriber(subscription_msg->tipo_cola, queue, subscriber_listed, &cantidad_mensajes, mensajes_encolados);

		subscriber = subscriber_listed;
	} else {
		subscriber = malloc(sizeof(*subscriber));
		subscriber->id_suscriptor = subscription_msg->id_proceso;
		subscriber->socket_suscriptor = socket_suscriptor;
		subscriber->activo = 1;
		subscribe_process(suscriptores, subscriber, mutex);

		reply_to_new_subscriber(subscription_msg->tipo_cola, queue, subscriber, &cantidad_mensajes, mensajes_encolados);
		remove_subscriber_if_temporal(suscriptores, subscriber, subscription_msg->temporal, mutex);
		log_new_subscriber(subscription_msg->id_proceso, subscription_msg->tipo_cola, LOGGER);
	}

	receive_ack(mensajes_encolados, cantidad_mensajes, subscriber);

	list_destroy(mensajes_encolados);
}

void process_new_message(int cod_op, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente)
{
	uint32_t id_mensaje = generate_id();

	t_queue* queue = COLAS_MENSAJES[cod_op];
	t_list* suscriptores = SUSCRIPTORES_MENSAJES[cod_op];
	pthread_mutex_t mutex = MUTEX_COLAS[cod_op];

	t_enqueued_message* mensaje_encolado = push_message_queue(queue, id_mensaje, id_correlativo, mensaje_recibido, mutex);

	enviar_id_respuesta(id_mensaje, socket_cliente);
	t_list* suscriptores_informados = inform_subscribers(cod_op, mensaje_recibido, id_mensaje, id_correlativo, suscriptores, mutex);
	mensaje_encolado->suscribers_informed = suscriptores_informados;
	log_new_message(id_mensaje, cod_op, LOGGER);

	receive_multiples_ack(cod_op, id_mensaje, suscriptores_informados);
}

void remove_subscriber_if_temporal(t_list* subscribers, t_subscriber* subscriber, uint32_t temporal, pthread_mutex_t mutex)
{
	if (temporal == 1) {
		uint32_t codigo = 0;
		recv(subscriber->socket_suscriptor, &codigo, sizeof(uint32_t), MSG_WAITALL);
		unsubscribe_process(subscribers, subscriber, mutex);
		free(subscriber);
	}
}

uint32_t generate_id()
{
	pthread_mutex_lock(&mutex_id_counter);
	uint32_t id_generado = ++ID_COUNTER;
	pthread_mutex_unlock(&mutex_id_counter);

	return id_generado;
}

t_list* inform_subscribers(op_code codigo, void* mensaje, uint32_t id, uint32_t id_correlativo, t_list* suscriptores, pthread_mutex_t mutex)
{
	t_list* suscriptores_informados = list_create();
	pthread_mutex_lock(&mutex);
	for (int i=0; i < list_size(suscriptores); i++) {
		t_subscriber* suscriptor = list_get(suscriptores, i);

		if (suscriptor->activo == 1) {
			if (enviar_mensaje(codigo, id, id_correlativo, mensaje, suscriptor->socket_suscriptor) > 0) {
				list_add(suscriptores_informados, (void*)suscriptor);
				log_message_to_subscriber(suscriptor->id_suscriptor, id, LOGGER);
			} else {
				suscriptor->activo = 0;
			}
		}

	}
	pthread_mutex_unlock(&mutex);

	return suscriptores_informados;
}

void receive_multiples_ack(op_code codigo, uint32_t id, t_list* suscriptores_informados)
{
	t_list* mensajes_encolados = list_create();
	t_queue* queue = COLAS_MENSAJES[codigo];
	t_enqueued_message* enqueued_message = find_message_by_id(queue, id);
	list_add(mensajes_encolados, enqueued_message);

	for (int i=0; i < list_size(suscriptores_informados); i++) {
		t_subscriber* suscriptor = list_get(suscriptores_informados, i);
		receive_ack(mensajes_encolados, 1, suscriptor);
	}

	list_destroy(mensajes_encolados);
}

void reply_to_new_subscriber(op_code codigo, t_queue* message_queue, t_subscriber* subscriber, uint32_t* cantidad_mensajes, t_list* mensajes_encolados)
{
	printf("Cantidad de mensajes en cola: %d\n", *cantidad_mensajes);
	printf("Socket suscriptor: %d\n", subscriber->socket_suscriptor);
	fflush(stdout);

	t_list* paquetes_serializados = list_create();
	t_list* tamanio_paquetes = list_create();
	uint32_t tamanio_stream = 0;

	pthread_mutex_t mutex = MUTEX_COLAS[codigo];
	pthread_mutex_lock(&mutex);
	for (int i=0; i < *cantidad_mensajes; i++) {
		uint32_t bytes;
		t_enqueued_message* mensaje_encolado = get_message_by_index(message_queue, i);
		//TODO si mensaje_encolado->message == NULL, sacar de la cola y no enviar a nadie

		if(!isSubscriberListed(mensaje_encolado->suscribers_ack, subscriber->id_suscriptor)) {
			void* a_enviar = serializar_paquete(codigo, mensaje_encolado->ID, mensaje_encolado->ID_correlativo, mensaje_encolado->message, &bytes);
			bytes += sizeof(bytes);

			list_add(paquetes_serializados, a_enviar);
			list_add(tamanio_paquetes, &bytes);
			tamanio_stream += bytes;
			list_add(mensajes_encolados, (void*) mensaje_encolado);
		}

	}
	pthread_mutex_unlock(&mutex);

	*cantidad_mensajes = list_size(paquetes_serializados);

	send_enqueued_messages(*cantidad_mensajes, tamanio_stream, paquetes_serializados, tamanio_paquetes, mensajes_encolados, subscriber);

	list_destroy(tamanio_paquetes);
	list_destroy_and_destroy_elements(paquetes_serializados, free);
}

void send_enqueued_messages(uint32_t cantidad_mensajes, uint32_t tamanio_stream, t_list* paquetes_serializados, t_list* tamanio_paquetes, t_list* mensajes_encolados, t_subscriber* subscriber)
{
	void* a_enviar;
	int bytes_a_enviar = sizeof(cantidad_mensajes) + sizeof(tamanio_stream);
	if (cantidad_mensajes > 0) {
		a_enviar = malloc(tamanio_stream + sizeof(cantidad_mensajes) + sizeof(tamanio_stream));
		int offset = 0;
		memcpy(a_enviar + offset, &cantidad_mensajes, sizeof(cantidad_mensajes));
		offset += sizeof(cantidad_mensajes);
		memcpy(a_enviar + offset, &tamanio_stream, sizeof(tamanio_stream));
		offset += sizeof(tamanio_stream);

		for (int i=0; i < cantidad_mensajes; i++ ) {
			uint32_t* tamanio_paquete = (uint32_t*) list_get(tamanio_paquetes, i);
			void* paquete = list_get(paquetes_serializados, i);
			memcpy(a_enviar + offset, paquete, *tamanio_paquete);
			offset += *tamanio_paquete;
		}
		bytes_a_enviar += tamanio_stream;

	} else {
		a_enviar = malloc(sizeof(cantidad_mensajes) + sizeof(tamanio_stream));
		memcpy(a_enviar, &cantidad_mensajes, sizeof(cantidad_mensajes));
		memcpy(a_enviar + sizeof(cantidad_mensajes), &tamanio_stream, sizeof(tamanio_stream));
	}

	if (send(subscriber->socket_suscriptor, a_enviar, bytes_a_enviar, MSG_NOSIGNAL) > 0) {
	printf("Respondí a la suscripción!\n");
		if (cantidad_mensajes > 0) {
			printf("Envié mensajes encolados!\n");
			add_new_informed_subscriber_to_mq(mensajes_encolados, cantidad_mensajes, subscriber, LOGGER);
		}
	} else {
		printf("El suscriptor está inactivo!\n");
		subscriber->activo = 0;
	}

	free(a_enviar);
}

void receive_ack(t_list* mensajes_encolados, uint32_t cantidad_mensajes, t_subscriber* subscriber)
{
	uint32_t response_status = 0;
	int status = recv(subscriber->socket_suscriptor, &response_status, sizeof(response_status), MSG_WAITALL);

	if(status > 0 && response_status == 200) {
		add_new_ack_suscriber_to_mq(mensajes_encolados, cantidad_mensajes, subscriber, LOGGER);
	}
}

int init_server()
{
	char* IP = config_get_string_value(CONFIG,"IP_BROKER");
	char* PUERTO = config_get_string_value(CONFIG,"PUERTO_BROKER");
	return iniciar_servidor(IP, PUERTO);
}

t_selection_algorithm choose_partition_algorithm()
{
	char* partition_algorithm = config_get_string_value(CONFIG,"ALGORITMO_PARTICION_LIBRE");
	if (strcmp(partition_algorithm, "FF") == 0) {
		return FIRST_FIT;
	} else if (strcmp(partition_algorithm, "BF") == 0) {
		return BEST_FIT;
	}
	return NONE;
}

t_selection_algorithm choose_victim_algorithm()
{
	char* victim_algorithm = config_get_string_value(CONFIG,"ALGORITMO_REEMPLAZO");
	if (strcmp(victim_algorithm, "FIFO") == 0) {
		return FIFO;
	} else if (strcmp(victim_algorithm, "LRU") == 0) {
		return LRU;
	}
	return NONE;
}

void init_memory()
{
	char* size_memory = config_get_string_value(CONFIG,"TAMANO_MEMORIA");
	int size = atoi(size_memory);

	t_memory_algorithm memory_alg;
	t_selection_algorithm victim_alg;
	t_selection_algorithm partition_alg;

	char* mem_algorithm = config_get_string_value(CONFIG,"ALGORITMO_MEMORIA");
	if (strcmp(mem_algorithm, "PD") == 0) {
		memory_alg = DYNAMIC_PARTITIONS;
		victim_alg = choose_victim_algorithm();
		partition_alg = choose_partition_algorithm();
	} else if (strcmp(mem_algorithm, "BS") == 0) {
		memory_alg = BUDDY_SYSTEM;
		victim_alg = choose_victim_algorithm();
		partition_alg = NONE;
	}

	char* min_par_size = config_get_string_value(CONFIG,"TAMANO_MINIMO_PARTICION");
	int min_part_size = atoi(min_par_size);

	char* frequency = config_get_string_value(CONFIG,"FRECUENCIA_COMPACTACION");
	int freq_compact = atoi(frequency);

	load_memory(size, min_part_size, freq_compact, memory_alg, victim_alg, partition_alg);
}

void init_message_queues()
{
	NEW_POKEMON_QUEUE = create_message_queue();
	APPEARED_POKEMON_QUEUE = create_message_queue();
	CATCH_POKEMON_QUEUE = create_message_queue();
	CAUGHT_POKEMON_QUEUE = create_message_queue();
	GET_POKEMON_QUEUE = create_message_queue();
	LOCALIZED_POKEMON_QUEUE = create_message_queue();

	COLAS_MENSAJES[1] = NEW_POKEMON_QUEUE;
	COLAS_MENSAJES[2] = APPEARED_POKEMON_QUEUE;
	COLAS_MENSAJES[3] = CATCH_POKEMON_QUEUE;
	COLAS_MENSAJES[4] = CAUGHT_POKEMON_QUEUE;
	COLAS_MENSAJES[5] = GET_POKEMON_QUEUE;
	COLAS_MENSAJES[6] = LOCALIZED_POKEMON_QUEUE;

	MUTEX_COLAS[1] = mutex_new_queue;
	MUTEX_COLAS[2] = mutex_appeared_queue;
	MUTEX_COLAS[3] = mutex_get_queue;
	MUTEX_COLAS[4] = mutex_localized_queue;
	MUTEX_COLAS[5] = mutex_catch_queue;
	MUTEX_COLAS[6] = mutex_caught_queue;
}

void init_suscriber_lists()
{
	NEW_POKEMON_SUBSCRIBERS = list_create();
	APPEARED_POKEMON_SUBSCRIBERS = list_create();
	CATCH_POKEMON_SUBSCRIBERS = list_create();
	CAUGHT_POKEMON_SUBSCRIBERS = list_create();
	GET_POKEMON_SUBSCRIBERS = list_create();
	LOCALIZED_POKEMON_SUBSCRIBERS = list_create();

	SUSCRIPTORES_MENSAJES[1] = NEW_POKEMON_SUBSCRIBERS;
	SUSCRIPTORES_MENSAJES[2] = APPEARED_POKEMON_SUBSCRIBERS;
	SUSCRIPTORES_MENSAJES[3] = CATCH_POKEMON_SUBSCRIBERS;
	SUSCRIPTORES_MENSAJES[4] = CAUGHT_POKEMON_SUBSCRIBERS;
	SUSCRIPTORES_MENSAJES[5] = GET_POKEMON_SUBSCRIBERS;
	SUSCRIPTORES_MENSAJES[6] = LOCALIZED_POKEMON_SUBSCRIBERS;

	MUTEX_SUSCRIPTORES[1] = mutex_new_susc;
	MUTEX_SUSCRIPTORES[2] = mutex_appeared_susc;
	MUTEX_SUSCRIPTORES[3] = mutex_get_susc;
	MUTEX_SUSCRIPTORES[4] = mutex_localized_susc;
	MUTEX_SUSCRIPTORES[5] = mutex_catch_susc;
	MUTEX_SUSCRIPTORES[6] = mutex_caught_susc;
}

void init_logger()
{
	char* broker_log = config_get_string_value(CONFIG,"LOG_FILE");
	LOGGER = log_create(broker_log, BROKER_NAME, false, LOG_LEVEL_INFO);
}

void init_config()
{
	CONFIG = config_create(BROKER_CONFIG);
}


void destroy_all_mutex()
{
	pthread_mutex_destroy(&mutex_id_counter);
	pthread_mutex_destroy(&mutex_new_queue);
	pthread_mutex_destroy(&mutex_appeared_queue);
	pthread_mutex_destroy(&mutex_get_queue);
	pthread_mutex_destroy(&mutex_localized_queue);
	pthread_mutex_destroy(&mutex_catch_queue);
	pthread_mutex_destroy(&mutex_caught_queue);
	pthread_mutex_destroy(&mutex_new_susc);
	pthread_mutex_destroy(&mutex_appeared_susc);
	pthread_mutex_destroy(&mutex_get_susc);
	pthread_mutex_destroy(&mutex_localized_susc);
	pthread_mutex_destroy(&mutex_catch_susc);
	pthread_mutex_destroy(&mutex_caught_susc);
}

void terminar_programa(int socket_servidor, t_log* logger)
{
	destroy_all_mutex();
	liberar_conexion(socket_servidor);
	log_destroy(logger);
	config_destroy(CONFIG);
}

