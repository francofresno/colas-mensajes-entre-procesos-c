/*
 ============================================================================
 Name        : Team
 Author      : Fran and Co
 Description : Proceso Team
 ============================================================================
 */

#include "team.h"

int main(int argc, char *argv[]) {

	t_config* config = leer_config();

	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	char* puertoBroker = config_get_string_value(config, "PUERTO_BROKER");

	int socket_cliente = crear_conexion (ipBroker, puertoBroker);

	op_code codigoOperacion = stringACodigoOperacion(argv[1]);  //¿Por que argv[1] y no argv[2]?

	switch(codigoOperacion)
		{
			case GET_POKEMON: ;
				t_getPokemon_msg estructuraGet;
				estructuraGet.nombre_pokemon.nombre = argv[2];
				estructuraGet.nombre_pokemon.nombre_lenght = strlen(estructuraGet.nombre_pokemon.nombre)+1;
				enviar_mensaje(codigoOperacion, 1, 0, &estructuraGet, socket_cliente); //id y id correlativo??
				break;
			case CATCH_POKEMON: ;
				t_catchPokemon_msg estructuraCatch;
				estructuraCatch.nombre_pokemon.nombre = argv[2];
				estructuraCatch.nombre_pokemon.nombre_lenght = strlen(estructuraCatch.nombre_pokemon.nombre)+1;
				estructuraCatch.coordenadas.posX = atoi(argv[3]);
				estructuraCatch.coordenadas.posY = atoi(argv[4]);
				enviar_mensaje(codigoOperacion, 2, 0, &estructuraCatch, socket_cliente);
				break;
			case ERROR_CODIGO: printf("[!] Error en el codigo de operacion\n"); return -1; break;
			default: printf("[!] Error desconocido en el codigo de operacion\n"); return -1;
		}

	puts("Soy un team!\n");

	fflush(stdout);

	int socket_servidor = iniciar_servidor(IP, PUERTO);

	while(1) {

		int socket_cliente = esperar_cliente(socket_servidor);
		if(socket_cliente > 0) {
			pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
			pthread_detach(thread);
		}
	}


	return EXIT_SUCCESS;
}


t_log* iniciar_logger(void)
{
	//TODO catchear si == NULL
	return log_create(TEAM_LOG, TEAM_NAME, true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	//TODO catchear si == NULL
	return config_create(TEAM_CONFIG);

}

void serve_client(int* socket_cliente)
{
	t_paquete* paquete_recibido = recibir_paquete(*socket_cliente);

	process_request(paquete_recibido->codigo_operacion, paquete_recibido->id_correlativo, paquete_recibido->mensaje, *socket_cliente);
}

void process_request(int cod_op, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente)
{
	switch(cod_op)
	{

		case APPEARED_POKEMON: ;

		puts("Llego un appeared al Team!\n");

			break;

		case LOCALIZED_POKEMON: ;

		puts("Llego un localized al Team!\n");

			break;

		case CAUGHT_POKEMON: ;

		puts("Llego un caught al Team!\n");

			break;
	}
}


