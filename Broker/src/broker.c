/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Proceso Broker
 ============================================================================
 */

#include "broker.h"


int main(void) {

	init_config();
	init_logger();
	init_message_queues();
	init_suscriber_lists();
	int socket_servidor = init_server();


	printf("broker!\n");
	fflush(stdout);

	while(1) {
		int socket_cliente = esperar_cliente(socket_servidor);
		if(socket_cliente > 0) {
			pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
			pthread_detach(thread);
		}
	}

}

void serve_client(int* socket_cliente)
{
	char* nombre_recibido = NULL;
	t_paquete* paquete_recibido = recibir_paquete(*socket_cliente, &nombre_recibido);

	process_request(paquete_recibido->codigo_operacion, paquete_recibido->id_correlativo, paquete_recibido->mensaje, *socket_cliente);

	free_paquete_recibido(nombre_recibido, paquete_recibido);
}

void process_request(int cod_op, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente)
{
	if (cod_op == SUSCRIPCION) {
		suscribir_a_cola((t_suscripcion_msg*) mensaje_recibido, socket_cliente);
	} else {
		uint32_t id_mensaje = generar_id();

		t_queue* queue = COLAS_MENSAJES[cod_op];
		t_list* suscriptores = SUSCRIPTORES_MENSAJES[cod_op];
		pthread_mutex_t mutex = MUTEX_COLAS[cod_op];
		t_list* suscriptores_informados = informar_a_suscriptores(cod_op, mensaje_recibido, id_mensaje, id_correlativo, suscriptores, mutex);
		push_message_queue(queue, id_mensaje, id_correlativo, mensaje_recibido, suscriptores_informados, mutex); //TODO modificar para hacer el push ANTES de informar
		enviar_id_respuesta(id_mensaje,socket_cliente);
		log_nuevo_mensaje(id_mensaje, cod_op, logger);
		recibir_multiples_ack(cod_op, id_mensaje, suscriptores_informados);
	}
}

void suscribir_a_cola(t_suscripcion_msg* estructuraSuscripcion, int socket_suscriptor)
{
	t_subscriber* subscriber = malloc(sizeof(*subscriber));
	subscriber->id_suscriptor = estructuraSuscripcion->id_proceso;
	subscriber->socket_suscriptor = socket_suscriptor;

	t_list* suscriptores = SUSCRIPTORES_MENSAJES[estructuraSuscripcion->tipo_cola];
	t_queue* queue = COLAS_MENSAJES[estructuraSuscripcion->tipo_cola];
	pthread_mutex_t mutex = MUTEX_SUSCRIPTORES[estructuraSuscripcion->tipo_cola];
	uint32_t cantidad_mensajes = size_message_queue(queue);
	t_enqueued_message* mensajes_encolados[cantidad_mensajes];

	if (isSubscriber(suscriptores, subscriber)) {
		responder_a_suscriptor_nuevo(estructuraSuscripcion->tipo_cola, queue, subscriber, cantidad_mensajes, mensajes_encolados);
	} else {
		subscribe_process(suscriptores, subscriber, mutex);

		responder_a_suscriptor_nuevo(estructuraSuscripcion->tipo_cola, queue, subscriber, cantidad_mensajes, mensajes_encolados);
		remover_suscriptor_si_es_temporal(suscriptores, subscriber, estructuraSuscripcion->temporal, mutex);
		log_nuevo_suscriptor(estructuraSuscripcion->id_proceso, estructuraSuscripcion->tipo_cola, logger);
	}

	recibir_ack(mensajes_encolados, cantidad_mensajes, subscriber);

}

void remover_suscriptor_si_es_temporal(t_list* subscribers, t_subscriber* subscriber, uint32_t temporal, pthread_mutex_t mutex)
{
	if (temporal == 1) {
		uint32_t codigo = 0;
		recv(subscriber->socket_suscriptor, &codigo, sizeof(uint32_t), MSG_WAITALL);
		unsubscribe_process(subscribers, subscriber, mutex);
		free(subscriber);
	}
}

uint32_t generar_id()
{
	pthread_mutex_lock(&mutex_id_counter);
	uint32_t id_generado = ++ID_COUNTER;
	pthread_mutex_unlock(&mutex_id_counter);

	return id_generado;
}

t_list* informar_a_suscriptores(op_code codigo, void* mensaje, uint32_t id, uint32_t id_correlativo, t_list* suscriptores, pthread_mutex_t mutex)
{
	t_list* suscriptores_informados = list_create();
	pthread_mutex_lock(&mutex);
	for (int i=0; i < list_size(suscriptores); i++) {
		t_subscriber* suscriptor = list_get(suscriptores, i);
		if (enviar_mensaje(codigo, id, id_correlativo, mensaje, suscriptor->socket_suscriptor) > 0) {
			list_add(suscriptores_informados, (void*)suscriptor);
			log_mensaje_a_suscriptor(suscriptor->id_suscriptor, id, logger);
		}
	}
	pthread_mutex_unlock(&mutex);

	return suscriptores_informados;
}

void recibir_multiples_ack(op_code codigo, uint32_t id, t_list* suscriptores_informados)
{
	t_enqueued_message* mensajes_encolados[1];
	t_queue* queue = COLAS_MENSAJES[codigo];
	t_enqueued_message* enqueued_message = find_message_by_id(queue, id);
	mensajes_encolados[0] = enqueued_message;

	for (int i=0; i < list_size(suscriptores_informados); i++) {
		t_subscriber* suscriptor = list_get(suscriptores_informados, i);
		recibir_ack(mensajes_encolados, 1, suscriptor);
	}
}

void responder_a_suscriptor_nuevo(op_code codigo, t_queue* message_queue, t_subscriber* subscriber, uint32_t cantidad_mensajes, t_enqueued_message* mensajes_encolados[])
{
	printf("Cantidad de mensajes en cola: %d\n", cantidad_mensajes);
	fflush(stdout);

	void* paquetes_serializados[cantidad_mensajes];
	int tamanio_paquetes[cantidad_mensajes];
	uint32_t tamanio_stream = 0;

	for (int i=0; i < cantidad_mensajes; i++) {
		uint32_t bytes;
		t_enqueued_message* mensaje_encolado = get_message_by_index(message_queue, i); // TODO deberia meter un mutex aca si en algun momento REMUEVO mensajes de la cola
		void* a_enviar = serializar_paquete(codigo, mensaje_encolado->ID, mensaje_encolado->ID_correlativo, mensaje_encolado->message, &bytes);
		bytes += sizeof(bytes);

		paquetes_serializados[i] = a_enviar;
		tamanio_paquetes[i] = bytes;
		tamanio_stream += bytes;
		mensajes_encolados[i] = mensaje_encolado;
	}

	enviar_mensajes_encolados(cantidad_mensajes, tamanio_stream, paquetes_serializados, tamanio_paquetes, mensajes_encolados, subscriber);

	for (int i=0; i < cantidad_mensajes; i++) {
		free(paquetes_serializados[i]);
	}
}

void enviar_mensajes_encolados(uint32_t cantidad_mensajes, uint32_t tamanio_stream, void** paquetes_serializados, int* tamanio_paquetes, t_enqueued_message* mensajes_encolados[], t_subscriber* subscriber)
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
			memcpy(a_enviar + offset, paquetes_serializados[i], tamanio_paquetes[i]);
			offset += tamanio_paquetes[i];
		}
		bytes_a_enviar += tamanio_stream;

	} else {
		a_enviar = malloc(sizeof(cantidad_mensajes) + sizeof(tamanio_stream));
		memcpy(a_enviar, &cantidad_mensajes, sizeof(cantidad_mensajes));
		memcpy(a_enviar + sizeof(cantidad_mensajes), &tamanio_stream, sizeof(tamanio_stream));
	}

	printf("Respondi!\n");
	if (send(subscriber->socket_suscriptor, a_enviar, bytes_a_enviar, 0) > 0 && cantidad_mensajes > 0) {
		printf("Envie mensajes encolados!\n");
		add_new_informed_subscriber_to_mq(mensajes_encolados, cantidad_mensajes, subscriber);
	}

	free(a_enviar);
}

void recibir_ack(t_enqueued_message* mensajes_encolados[], uint32_t cantidad_mensajes, t_subscriber* subscriber) {
	uint32_t response_status = 0;
	int status = recv(subscriber->socket_suscriptor, &response_status, sizeof(response_status), MSG_WAITALL);

	if(status > 0 && response_status == 200) {
		add_new_ack_suscriber_to_mq(mensajes_encolados, cantidad_mensajes, subscriber);
	}
}

int init_server()
{
	char* IP = config_get_string_value(config,"IP_BROKER");
	char* PUERTO = config_get_string_value(config,"PUERTO_BROKER");
	return iniciar_servidor(IP, PUERTO);
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
	char* broker_log = config_get_string_value(config,"LOG_FILE");
	logger = log_create(broker_log, BROKER_NAME, false, LOG_LEVEL_INFO);
}

void init_config()
{
	config = config_create(BROKER_CONFIG);
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
	config_destroy(config);
}

