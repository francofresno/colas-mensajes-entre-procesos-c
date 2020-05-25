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
	t_list* suscriptores_informados;

	if (cod_op == SUSCRIPCION) {
		suscribir_a_cola((t_suscripcion_msg*) mensaje_recibido, socket_cliente);
	} else {
		uint32_t id_mensaje = generar_id();
		enviar_id_respuesta(id_mensaje,socket_cliente);

		t_queue* queue = COLAS_MENSAJES[cod_op];
		t_list* suscriptores = SUSCRIPTORES_MENSAJES[cod_op];
		pthread_mutex_t mutex = MUTEX_COLAS[cod_op];
		suscriptores_informados = informar_a_suscriptores(cod_op, mensaje_recibido, id_mensaje, id_correlativo, suscriptores, mutex);
		push_message_queue(queue, id_mensaje, id_correlativo, mensaje_recibido, suscriptores_informados, mutex);
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

	subscribe_process(suscriptores, subscriber, mutex);
	responder_a_suscriptor_nuevo(estructuraSuscripcion->tipo_cola, queue, subscriber);
	remover_suscriptor_si_es_temporal(suscriptores, subscriber, estructuraSuscripcion->tiempo, mutex);
}

void remover_suscriptor_si_es_temporal(t_list* subscribers, t_subscriber* subscriber, uint32_t tiempo, pthread_mutex_t mutex)
{
	if (tiempo > 0) {
		sleep(tiempo); //TODO chequear esto con ayudantes
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
		}
	}
	pthread_mutex_unlock(&mutex);
	return suscriptores_informados;
}

void responder_a_suscriptor_nuevo(op_code codigo, t_queue* message_queue, t_subscriber* subscriber)
{
	uint32_t cantidad_mensajes = size_message_queue(message_queue);
	t_paquete paquetes[cantidad_mensajes];
	t_enqueued_message* mensajes_encolados[cantidad_mensajes];

	printf("Cantidad de mensajes en cola: %d\n", cantidad_mensajes);
	fflush(stdout);

	for (int i=0; i < cantidad_mensajes; i++) {
		t_enqueued_message* mensaje_encolado = get_message_by_index(message_queue, i); // TODO deberia meter un mutex aca si en algun momento REMUEVO mensajes de la cola
		t_paquete paquete;
		paquete.codigo_operacion = codigo;
		paquete.id = mensaje_encolado->ID;
		paquete.id_correlativo = mensaje_encolado->ID_correlativo;
		paquete.mensaje = mensaje_encolado->message;
		paquetes[i] = paquete;
		mensajes_encolados[i] = mensaje_encolado;
	}

	if (enviar_mensajes_encolados_a_suscriptor_nuevo(cantidad_mensajes, paquetes, subscriber->socket_suscriptor) != -1) {
		printf("Respondi!\n");
		add_new_informed_subscriber_to_mq(mensajes_encolados, cantidad_mensajes, subscriber);
	}
}

int enviar_mensajes_encolados_a_suscriptor_nuevo(uint32_t cantidad_a_enviar, t_paquete paquetes[], int socket_envio)
{
	// Envio la cantidad de paquetes que se enviaran
	if(send(socket_envio, &cantidad_a_enviar, sizeof(cantidad_a_enviar), 0) < 0)
		return -1;

	if (cantidad_a_enviar > 0) {
		// Recorro los mensajes y los envio 1 por 1
		for (int i=0; i < cantidad_a_enviar; i++) {
			t_paquete paquete = paquetes[i];
			if(enviar_mensaje(paquete.codigo_operacion, paquete.id, paquete.id_correlativo, paquete.mensaje, socket_envio) < 0)
				return -1;
		}
	}
	return 0;
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

void terminar_programa(int socket_servidor, t_log* logger, t_config* config)
{
	destroy_all_mutex();
	liberar_conexion(socket_servidor);
	log_destroy(logger);
	config_destroy(config);
}

