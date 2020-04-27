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
	void* paqueteRecibido = recibir_paquete(cod_op, *socket_cliente);
	switch(cod_op)
	{
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = malloc(sizeof(estructuraNew));
			estructuraNew = paqueteRecibido;

			printf("\n%s|","NEW");
			printf("%d|",estructuraNew->nombre_pokemon.nombre_lenght);
			printf("%s|",estructuraNew->nombre_pokemon.nombre);
			printf("%d|",estructuraNew->coordenadas.posX);
			printf("%d|",estructuraNew->coordenadas.posY);
			printf("%d",estructuraNew->cantidad_pokemons);
			fflush(stdout);
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estructuraAppeared = malloc(sizeof(estructuraAppeared));
			estructuraAppeared = paqueteRecibido;

			printf("\n%s|","APPEARED");
			printf("%d|",estructuraAppeared->nombre_pokemon.nombre_lenght);
			printf("%s|",estructuraAppeared->nombre_pokemon.nombre);
			printf("%d|",estructuraAppeared->coordenadas.posX);
			printf("%d|",estructuraAppeared->coordenadas.posY);
			fflush(stdout);
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estructuraGet = malloc(sizeof(estructuraGet));
			estructuraGet = paqueteRecibido;

			printf("\n%s|","GET");
			printf("%d|",estructuraGet->nombre_pokemon.nombre_lenght);
			printf("%s|",estructuraGet->nombre_pokemon.nombre);
			fflush(stdout);
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = malloc(sizeof(estructuraLocalized));
			estructuraLocalized = paqueteRecibido;

			printf("\n%s|","LOCALIZED");
			printf("%d|",estructuraLocalized->nombre_pokemon.nombre_lenght);
			printf("%s|",estructuraLocalized->nombre_pokemon.nombre);
			printf("%d|",estructuraLocalized->cantidad_coordenadas);
			fflush(stdout);
			for(int i = 0; i < estructuraLocalized->cantidad_coordenadas; i++)
			{
				printf("%d|",estructuraLocalized->coordenadas[i].posX);
				printf("%d|",estructuraLocalized->coordenadas[i].posY);
				fflush(stdout);
			}
			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estructuraCatch = malloc(sizeof(estructuraCatch));
			estructuraCatch = paqueteRecibido;

			printf("\n%s|","CATCH");
			printf("%d|",estructuraCatch->nombre_pokemon.nombre_lenght);
			printf("%s|",estructuraCatch->nombre_pokemon.nombre);
			printf("%d|",estructuraCatch->coordenadas.posX);
			printf("%d|",estructuraCatch->coordenadas.posY);
			fflush(stdout);
		break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estructuraCaught = malloc(sizeof(estructuraCaught));
			estructuraCaught = paqueteRecibido;

			printf("\n%s|","CAUGHT");
			printf("%d|",estructuraCaught->atrapado);
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
