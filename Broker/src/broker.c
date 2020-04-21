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

	int socket_servidor = iniciar_servidor(IP, PUERTO);

	printf("Broker!");
	fflush(stdout);

	while(1) {
		int socket_cliente = esperar_cliente(socket_servidor);
		pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
		pthread_detach(thread);
	}

}

void serve_client(int* socket_cliente)
{
	int cod_op = recibir_codigo_operacion(*socket_cliente);
	process_request(cod_op, *socket_cliente);
}

void process_request(int cod_op, int cliente_fd) {
	t_newPokemon_msg* new_pokemon_msg;

	switch (cod_op) {
	//TODO todos los cases
		case NEW_POKEMON:
			new_pokemon_msg = malloc(sizeof(new_pokemon_msg));
			new_pokemon_msg = deserializar_new_pokemon_msg(cliente_fd);

			printf("\n%d|",new_pokemon_msg->nombre_pokemon->nombre_lenght);
			fflush(stdout);
			printf("%s|",new_pokemon_msg->nombre_pokemon->nombre);
			fflush(stdout);
			printf("%d|",new_pokemon_msg->coordenadas->posX);
			fflush(stdout);
			printf("%d|",new_pokemon_msg->coordenadas->posY);
			fflush(stdout);
			printf("%d",new_pokemon_msg->cantidad_pokemons);
			fflush(stdout);

			free(new_pokemon_msg);
			break;
		case APPEARED_POKEMON:

			break;
		case CATCH_POKEMON:

			break;
		case CAUGHT_POKEMON:

			break;
		case GET_POKEMON:

			break;
		case LOCALIZED_POKEMON:

			break;
		case 0:
			close(cliente_fd);
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		default:
			printf("default");
			fflush(stdout);
			break;
	}
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
