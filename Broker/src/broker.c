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

//	int socket_servidor = iniciar_servidor(IP, PUERTO);
//
//	printf("Broker!");
//	fflush(stdout);

	t_newPokemon_msg new1;
	new1.nombre_pokemon.nombre = "PIKACHU";
	new1.nombre_pokemon.nombre_lenght = strlen(new1.nombre_pokemon.nombre) + 1;
	new1.coordenadas.posX = 6;
	new1.coordenadas.posY = 2;
	new1.cantidad_pokemons = 7;

	t_newPokemon_msg new2;
	new2.nombre_pokemon.nombre = "CHARMANDER";
	new2.nombre_pokemon.nombre_lenght = strlen(new2.nombre_pokemon.nombre) + 1;
	new2.coordenadas.posX = 6;
	new2.coordenadas.posY = 2;
	new2.cantidad_pokemons = 7;

	t_newPokemon_msg new3;
	new3.nombre_pokemon.nombre = "RAICHU";
	new3.nombre_pokemon.nombre_lenght = strlen(new3.nombre_pokemon.nombre) + 1;
	new3.coordenadas.posX = 6;
	new3.coordenadas.posY = 2;
	new3.cantidad_pokemons = 7;

	t_data* n1 = malloc(sizeof(t_data*));
	n1->ID = 1;
	n1->ID_correlativo = 0;
	n1->message = (void*) &new1;
	t_data* n2 = malloc(sizeof(t_data*));
	n2->ID = 2;
	n2->ID_correlativo = 1;
	n2->message = (void*) &new2;
	t_data* n3 = malloc(sizeof(t_data*));
	n3->ID = 3;
	n3->ID_correlativo = 2;
	n3->message = (void*) &new3;

	t_queue* newq = create_message_queue();
	push_message_queue(newq, n1);
	push_message_queue(newq, n2);
	push_message_queue(newq, n3);

	t_data* data = find_message_by_id(newq, 2);

	t_newPokemon_msg* new4 = malloc(sizeof(new4));
	new4 = data->message;

	printf("Cantidad en cola: %d\n", newq->elements->elements_count);
	printf("ID: %d\n",data->ID);
	printf("Nombre: %s\n",new4->nombre_pokemon.nombre);
	fflush(stdout);

//	while(1) {
//		int socket_cliente = esperar_cliente(socket_servidor);
//		pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
//		pthread_detach(thread);
//	}

}

void serve_client(int* socket_cliente)
{
	int cod_op = recibir_codigo_operacion(*socket_cliente);
	uint32_t id = recibir_id(*socket_cliente);
	uint32_t id_correlativo = recibir_id(*socket_cliente);
	void* paqueteRecibido = recibir_paquete(cod_op, *socket_cliente);
	switch(cod_op)
	{
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = malloc(sizeof(estructuraNew));
			estructuraNew = paqueteRecibido;

			printf("\n%s|","NEW");
			printf("%d|",id);
			printf("%d|",id_correlativo);
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
			printf("%d|",id);
			printf("%d|",id_correlativo);
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
			printf("%d|",id);
			printf("%d|",id_correlativo);
			printf("%d|",estructuraGet->nombre_pokemon.nombre_lenght);
			printf("%s|",estructuraGet->nombre_pokemon.nombre);
			fflush(stdout);
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = malloc(sizeof(estructuraLocalized));
			estructuraLocalized = paqueteRecibido;

			printf("\n%s|","LOCALIZED");
			printf("%d|",id);
			printf("%d|",id_correlativo);
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
			printf("%d|",id);
			printf("%d|",id_correlativo);
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
			printf("%d|",id);
			printf("%d|",id_correlativo);
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
