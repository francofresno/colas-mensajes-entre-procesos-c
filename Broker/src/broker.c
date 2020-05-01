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
		pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
		pthread_detach(thread);
	}

	pthread_mutex_destroy(&mutex_id_counter);
	//terminar_programa(int socket_servidor, t_log* logger, t_config* config);
}

void serve_client(int* socket_cliente)
{
	int cod_op = recibir_codigo_operacion(*socket_cliente);
	recibir_id(*socket_cliente);
	uint32_t id_correlativo = recibir_id(*socket_cliente);
	void* mensaje_recibido = recibir_paquete(cod_op, *socket_cliente);

	process_request(cod_op, id_correlativo, mensaje_recibido, *socket_cliente);
}

void process_request(int cod_op, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente)
{
	uint32_t id_mensaje;
	switch(cod_op)
	{
		case SUSCRIPCION: ;
			t_suscripcion_msg* estructuraSuscripcion = malloc(sizeof(estructuraSuscripcion));
			estructuraSuscripcion = mensaje_recibido;

			suscribir(estructuraSuscripcion);

			//TODO enviar todos los mensajes de la cola a la que se suscribio

			break;
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = malloc(sizeof(estructuraNew));
			estructuraNew = mensaje_recibido;

			pthread_mutex_lock(&mutex_id_counter);
			id_mensaje = generar_id();
		    pthread_mutex_unlock(&mutex_id_counter);

			push_message_queue(NEW_POKEMON_QUEUE, id_mensaje, 0, mensaje_recibido);
			enviar_id_respuesta(id_mensaje,socket_cliente);

			//informar_a_suscriptores(NEW_POKEMON, mensaje_recibido, id_mensaje, 0, NEW_POKEMON_SUBSCRIBERS);

			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estructuraAppeared = malloc(sizeof(estructuraAppeared));
			estructuraAppeared = mensaje_recibido;

			pthread_mutex_lock(&mutex_id_counter);
			id_mensaje = generar_id();
		    pthread_mutex_unlock(&mutex_id_counter);

			push_message_queue(APPEARED_POKEMON_QUEUE, id_mensaje, id_correlativo, mensaje_recibido);
			enviar_id_respuesta(id_mensaje,socket_cliente);

			//informar_a_suscriptores(APPEARED_POKEMON, mensaje_recibido, id_mensaje, id_correlativo, APPEARED_POKEMON_SUBSCRIBERS);

			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estructuraGet = malloc(sizeof(estructuraGet));
			estructuraGet = mensaje_recibido;

			pthread_mutex_lock(&mutex_id_counter);
			id_mensaje = generar_id();
		    pthread_mutex_unlock(&mutex_id_counter);

			push_message_queue(GET_POKEMON_QUEUE, id_mensaje, 0, mensaje_recibido);
			enviar_id_respuesta(id_mensaje,socket_cliente);

			//informar_a_suscriptores(GET_POKEMON, mensaje_recibido, id_mensaje, 0, GET_POKEMON_SUBSCRIBERS);

			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = malloc(sizeof(estructuraLocalized));
			estructuraLocalized = mensaje_recibido;

			pthread_mutex_lock(&mutex_id_counter);
			id_mensaje = generar_id();
		    pthread_mutex_unlock(&mutex_id_counter);

			push_message_queue(LOCALIZED_POKEMON_QUEUE, id_mensaje, id_correlativo, mensaje_recibido);
			enviar_id_respuesta(id_mensaje,socket_cliente);

			//informar_a_suscriptores(LOCALIZED_POKEMON, mensaje_recibido, id_mensaje, id_correlativo, LOCALIZED_POKEMON_SUBSCRIBERS);

			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estructuraCatch = malloc(sizeof(estructuraCatch));
			estructuraCatch = mensaje_recibido;

			pthread_mutex_lock(&mutex_id_counter);
			id_mensaje = generar_id();
		    pthread_mutex_unlock(&mutex_id_counter);

			push_message_queue(CATCH_POKEMON_QUEUE, id_mensaje, 0, mensaje_recibido);
			enviar_id_respuesta(id_mensaje,socket_cliente);

			//informar_a_suscriptores(CATCH_POKEMON, mensaje_recibido, id_mensaje, 0, CATCH_POKEMON_SUBSCRIBERS);

			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estructuraCaught = malloc(sizeof(estructuraCaught));
			estructuraCaught = mensaje_recibido;

			pthread_mutex_lock(&mutex_id_counter);
			id_mensaje = generar_id();
		    pthread_mutex_unlock(&mutex_id_counter);

			push_message_queue(CAUGHT_POKEMON_QUEUE, id_mensaje, id_correlativo, mensaje_recibido);
			enviar_id_respuesta(id_mensaje,socket_cliente);

			//informar_a_suscriptores(CAUGHT_POKEMON, mensaje_recibido, id_mensaje, id_correlativo, CAUGHT_POKEMON_SUBSCRIBERS);

			break;
	}
}

void suscribir(t_suscripcion_msg* estructuraSuscripcion)
{
	t_subscriber* subscriber = malloc(sizeof(subscriber));
	subscriber->ip_proceso = estructuraSuscripcion->ip_proceso;
	subscriber->puerto_proceso = estructuraSuscripcion->puerto_proceso;

	switch(estructuraSuscripcion->tipo_cola)
	{
		case NEW_POKEMON: ;
			list_add(NEW_POKEMON_SUBSCRIBERS, (void*) subscriber);
			break;
		case APPEARED_POKEMON: ;
			list_add(APPEARED_POKEMON_SUBSCRIBERS, (void*) subscriber);
			break;
		case GET_POKEMON: ;
			list_add(CATCH_POKEMON_SUBSCRIBERS, (void*) subscriber);
			break;
		case LOCALIZED_POKEMON: ;
			list_add(CAUGHT_POKEMON_SUBSCRIBERS, (void*) subscriber);
			break;
		case CATCH_POKEMON: ;
			list_add(GET_POKEMON_SUBSCRIBERS, (void*) subscriber);
			break;
		case CAUGHT_POKEMON: ;
			list_add(LOCALIZED_POKEMON_SUBSCRIBERS, (void*) subscriber);
			break;
		case SUSCRIPCION:
		case ERROR_CODIGO:
		default:
			break;
	}
}

uint32_t generar_id()
{
	ID_COUNTER++;
	return ID_COUNTER;
	//2147483647 es el numero maximo de un uint32_t, despues de eso imprime el complementario para llegar a 4294967296, que es el max de uint
	//return (rand()%2147483647) + 1;
}

void informar_a_suscriptores(op_code codigo, void* mensaje, uint32_t id, uint32_t id_correlativo, t_list* suscriptores)
{
	for (int i=0; i < list_size(suscriptores); i++) {
		t_subscriber* suscriptor = list_get(suscriptores, i);

		int socket_suscriptor = crear_conexion(suscriptor->ip_proceso, suscriptor->puerto_proceso);
		if (socket_suscriptor != -1) {
			enviar_mensaje(codigo, id, id_correlativo, mensaje, socket_suscriptor);
			liberar_conexion(socket_suscriptor);
		}
		//TODO deberia desuscribir al que no esta levantado?
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

void terminar_programa(int socket_servidor, t_log* logger, t_config* config)
{
	liberar_conexion(socket_servidor);
	log_destroy(logger);
	config_destroy(config);
}

unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

void setear_seed()
{
	unsigned long seed = mix(clock(), time(NULL), getpid());
	srand (seed);
}

