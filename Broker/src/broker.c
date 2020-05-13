/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Proceso Broker
 ============================================================================
 */

#include "broker.h"


int main(void) {

	t_config* config = leer_config();
	int socket_servidor = init_server(config);
	init_message_queues();
	init_suscriber_lists();

	while(1) {
		int socket_cliente = esperar_cliente(socket_servidor);
		if(socket_cliente > 0) {
			pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
			pthread_detach(thread);
		}
	}

	//terminar_programa(int socket_servidor, t_log* logger, t_config* config);
}

void serve_client(int* socket_cliente)
{
	t_paquete* paquete_recibido = recibir_paquete(*socket_cliente);

	process_request(paquete_recibido->codigo_operacion, paquete_recibido->id_correlativo, paquete_recibido->mensaje, *socket_cliente);
}

void process_request(int cod_op, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente)
{
	uint32_t id_mensaje;
	t_list* suscriptores_informados;
	switch(cod_op)
	{
		case SUSCRIPCION: ;
			suscribir_a_cola((t_suscripcion_msg*) mensaje_recibido, socket_cliente);
			break;
		case NEW_POKEMON: ;
			id_mensaje = generar_id();

			enviar_id_respuesta(id_mensaje,socket_cliente);
			suscriptores_informados = informar_a_suscriptores(NEW_POKEMON, mensaje_recibido, id_mensaje, 0, NEW_POKEMON_SUBSCRIBERS, mutex_new_susc);
			push_message_queue(NEW_POKEMON_QUEUE, id_mensaje, 0, mensaje_recibido, suscriptores_informados, mutex_new_queue);

			//TODO recibir ACK cuando??? hilo?
			break;
		case APPEARED_POKEMON: ;
			id_mensaje = generar_id();

			enviar_id_respuesta(id_mensaje,socket_cliente);
			suscriptores_informados = informar_a_suscriptores(APPEARED_POKEMON, mensaje_recibido, id_mensaje, id_correlativo, APPEARED_POKEMON_SUBSCRIBERS, mutex_appeared_susc);
			push_message_queue(APPEARED_POKEMON_QUEUE, id_mensaje, id_correlativo, mensaje_recibido, suscriptores_informados, mutex_appeared_queue);

			break;
		case GET_POKEMON: ;
			id_mensaje = generar_id();

			enviar_id_respuesta(id_mensaje,socket_cliente);
			suscriptores_informados = informar_a_suscriptores(GET_POKEMON, mensaje_recibido, id_mensaje, 0, GET_POKEMON_SUBSCRIBERS, mutex_get_susc);
			push_message_queue(GET_POKEMON_QUEUE, id_mensaje, 0, mensaje_recibido, suscriptores_informados, mutex_get_queue);

			break;
		case LOCALIZED_POKEMON: ;
			id_mensaje = generar_id();

			enviar_id_respuesta(id_mensaje,socket_cliente);
			suscriptores_informados = informar_a_suscriptores(LOCALIZED_POKEMON, mensaje_recibido, id_mensaje, id_correlativo, LOCALIZED_POKEMON_SUBSCRIBERS, mutex_localized_susc);
			push_message_queue(LOCALIZED_POKEMON_QUEUE, id_mensaje, id_correlativo, mensaje_recibido, suscriptores_informados, mutex_localized_queue);

			break;
		case CATCH_POKEMON: ;
			id_mensaje = generar_id();

			enviar_id_respuesta(id_mensaje,socket_cliente);
			suscriptores_informados = informar_a_suscriptores(CATCH_POKEMON, mensaje_recibido, id_mensaje, 0, CATCH_POKEMON_SUBSCRIBERS, mutex_catch_susc);
			push_message_queue(CATCH_POKEMON_QUEUE, id_mensaje, 0, mensaje_recibido, suscriptores_informados, mutex_catch_queue);

			break;
		case CAUGHT_POKEMON: ;
			id_mensaje = generar_id();

			enviar_id_respuesta(id_mensaje,socket_cliente);
			suscriptores_informados = informar_a_suscriptores(CAUGHT_POKEMON, mensaje_recibido, id_mensaje, id_correlativo, CAUGHT_POKEMON_SUBSCRIBERS, mutex_caught_susc);
			push_message_queue(CAUGHT_POKEMON_QUEUE, id_mensaje, id_correlativo, mensaje_recibido, suscriptores_informados, mutex_caught_queue);

			break;
	}
}

void suscribir_a_cola(t_suscripcion_msg* estructuraSuscripcion, int socket_suscriptor)
{
	t_subscriber* subscriber = malloc(sizeof(subscriber));
	subscriber->id_suscriptor = estructuraSuscripcion->id_proceso;
	subscriber->socket_suscriptor = socket_suscriptor;

	//TODO informar sent to
	switch(estructuraSuscripcion->tipo_cola)
	{
		case NEW_POKEMON: ;
			subscribe_process(NEW_POKEMON_SUBSCRIBERS, subscriber, mutex_new_susc);
			responder_a_suscriptor_nuevo(NEW_POKEMON, NEW_POKEMON_QUEUE, subscriber);
			break;
		case APPEARED_POKEMON: ;
			subscribe_process(APPEARED_POKEMON_SUBSCRIBERS, subscriber, mutex_appeared_susc);
			responder_a_suscriptor_nuevo(APPEARED_POKEMON, APPEARED_POKEMON_QUEUE, subscriber);
			break;
		case GET_POKEMON: ;
			subscribe_process(GET_POKEMON_SUBSCRIBERS, subscriber, mutex_get_susc);
			responder_a_suscriptor_nuevo(GET_POKEMON, GET_POKEMON_QUEUE, subscriber);
			break;
		case LOCALIZED_POKEMON: ;
			subscribe_process(LOCALIZED_POKEMON_SUBSCRIBERS, subscriber, mutex_localized_susc);
			responder_a_suscriptor_nuevo(LOCALIZED_POKEMON, LOCALIZED_POKEMON_QUEUE, subscriber);
			break;
		case CATCH_POKEMON: ;
			subscribe_process(CATCH_POKEMON_SUBSCRIBERS, subscriber, mutex_catch_susc);
			responder_a_suscriptor_nuevo(CATCH_POKEMON, CATCH_POKEMON_QUEUE, subscriber);
			break;
		case CAUGHT_POKEMON: ;
			subscribe_process(CAUGHT_POKEMON_SUBSCRIBERS, subscriber, mutex_caught_susc);
			responder_a_suscriptor_nuevo(CAUGHT_POKEMON, CAUGHT_POKEMON_QUEUE, subscriber);
			break;
		case SUSCRIPCION:
		case ERROR_CODIGO:
		default: //TODO
			break;
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
		}
	}
	pthread_mutex_unlock(&mutex);
	return suscriptores_informados;
}

void responder_a_suscriptor_nuevo(op_code codigo, t_queue* message_queue, t_subscriber* subscriber)
{
	uint32_t cantidad_mensajes = size_message_queue(message_queue);
	t_paquete paquetes[cantidad_mensajes];
	t_data* mensajes_de_cola[cantidad_mensajes];

	for (int i=0; i < cantidad_mensajes; i++) {
		t_data* data = get_message_by_index(message_queue, i); // TODO deberia meter un mutex aca si en algun momento REMUEVO mensajes de la cola
		t_paquete paquete;
		paquete.codigo_operacion = codigo;
		paquete.id = data->ID;
		paquete.id_correlativo = data->ID_correlativo;
		paquete.mensaje = data->message;
		paquetes[i] = paquete;
		mensajes_de_cola[i] = data;
	}

	if (responder_a_suscripcion(cantidad_mensajes, paquetes, subscriber->socket_suscriptor) != -1) {
		add_new_informed_subscriber_mq(mensajes_de_cola, cantidad_mensajes, subscriber);
	}
}

int init_server(t_config* config)
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
}

void init_suscriber_lists()
{
	NEW_POKEMON_SUBSCRIBERS = list_create();
	APPEARED_POKEMON_SUBSCRIBERS = list_create();
	CATCH_POKEMON_SUBSCRIBERS = list_create();
	CAUGHT_POKEMON_SUBSCRIBERS = list_create();
	GET_POKEMON_SUBSCRIBERS = list_create();
	LOCALIZED_POKEMON_SUBSCRIBERS = list_create();
}

t_log* iniciar_logger(void)
{
	//TODO catchear si == NULL
	return log_create(BROKER_LOG, BROKER_NAME, true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	//TODO catchear si == NULL
	return config_create(BROKER_CONFIG);

}

void destroy_mutex()
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

void terminar_programa(int socket_servidor, t_log* logger, t_config* config)
{
	destroy_mutex();
	liberar_conexion(socket_servidor);
	log_destroy(logger);
	config_destroy(config);
}

