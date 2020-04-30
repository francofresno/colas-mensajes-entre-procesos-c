/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Proceso Broker
 ============================================================================
 */

#include "broker.h"


int main(void) {

	//t_config* config = leer_config();
	init_message_queues();
	init_suscriber_lists();

	int socket_servidor = iniciar_servidor(IP, PUERTO);

	while(1) {
		int socket_cliente = esperar_cliente(socket_servidor);
		pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
		pthread_detach(thread);
	}

	pthread_mutex_destroy(&mutex_id_counter);
}

void serve_client(int* socket_cliente)
{
	int cod_op = recibir_codigo_operacion(*socket_cliente);
	uint32_t id = recibir_id(*socket_cliente);
	uint32_t id_correlativo = recibir_id(*socket_cliente);
	void* mensaje_recibido = recibir_paquete(cod_op, *socket_cliente);

	process_request(cod_op, id, id_correlativo, mensaje_recibido, *socket_cliente);

}

void process_request(int cod_op, uint32_t id, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente)
{
	switch(cod_op)
	{
		case SUSCRIPCION: ;
			t_suscripcion_msg* estructuraSuscripcion = malloc(sizeof(estructuraSuscripcion));
			estructuraSuscripcion = mensaje_recibido;

			suscribir(estructuraSuscripcion);

			break;
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = malloc(sizeof(estructuraNew));
			estructuraNew = mensaje_recibido;

			uint32_t id_mensaje = generar_id();
			push_message_queue(NEW_POKEMON_QUEUE, id_mensaje, 0, mensaje_recibido);
			printf("Recibi un mensaje y genere el ID: %d\n", id_mensaje);
			fflush(stdout);
			enviar_id_respuesta(id_mensaje,socket_cliente);

			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estructuraAppeared = malloc(sizeof(estructuraAppeared));
			estructuraAppeared = mensaje_recibido;



			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estructuraGet = malloc(sizeof(estructuraGet));
			estructuraGet = mensaje_recibido;



			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = malloc(sizeof(estructuraLocalized));
			estructuraLocalized = mensaje_recibido;




			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estructuraCatch = malloc(sizeof(estructuraCatch));
			estructuraCatch = mensaje_recibido;




			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estructuraCaught = malloc(sizeof(estructuraCaught));
			estructuraCaught = mensaje_recibido;



			break;
	}
}

void suscribir(t_suscripcion_msg* estructuraSuscripcion)
{
	t_subscriber* subscriber = malloc(sizeof(subscriber));
	subscriber->ID_proceso = estructuraSuscripcion->ID_proceso;

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
	pthread_mutex_lock(&mutex_id_counter);
	ID_COUNTER++;
    pthread_mutex_unlock(&mutex_id_counter);
	return ID_COUNTER;
	//2147483647 es el numero maximo de un uint32_t, despues de eso imprime el complementario para llegar a 4294967296, que es el max de uint
	//return (rand()%2147483647) + 1;
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

