/*
 ============================================================================
 Name        : GameBoy
 Author      : Fran and Co
 Description : Proceso GameBoy
 ============================================================================
 */

#include "gameboy.h"

int main(int argc, char *argv[])
{
	t_config* config = leer_config();
	char* ip;
	char* puerto;

	process_code codigoProceso = stringACodigoProceso(argv[1]);

	switch(codigoProceso)
	{
		case BROKER:
			ip = config_get_string_value(config, "IP_BROKER");
			puerto = config_get_string_value(config, "PUERTO_BROKER");
			break;
		case TEAM:
			ip = config_get_string_value(config, "IP_TEAM");
			puerto = config_get_string_value(config, "PUERTO_TEAM");
			break;
		case GAMECARD:
			ip = config_get_string_value(config, "IP_GAMECARD");
			puerto = config_get_string_value(config, "PUERTO_GAMECARD");
			break;
		case ERROR_PROCESO: printf("[!] Error en el codigo de proceso\n"); return -1; break;
		default: printf("[!] Error desconocido en el codigo de proceso\n"); return -1;
	}

	int socket_cliente = crear_conexion (ip, puerto);
	op_code codigoOperacion = stringACodigoOperacion(argv[2]);

	switch(codigoOperacion)
	{
		case NEW_POKEMON: ;
			t_newPokemon_msg estructuraNew;
			estructuraNew.nombre_pokemon.nombre = argv[3];
			estructuraNew.nombre_pokemon.nombre_lenght = strlen(estructuraNew.nombre_pokemon.nombre)+1;
			estructuraNew.coordenadas.posX = atoi(argv[4]);
			estructuraNew.coordenadas.posY = atoi(argv[5]);
			estructuraNew.cantidad_pokemons = atoi(argv[6]);
			enviar_mensaje(codigoOperacion, 1, 0, &estructuraNew, socket_cliente);
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg estructuraAppeared;
			estructuraAppeared.nombre_pokemon.nombre = argv[3];
			estructuraAppeared.nombre_pokemon.nombre_lenght = strlen(estructuraAppeared.nombre_pokemon.nombre)+1;
			estructuraAppeared.coordenadas.posX = atoi(argv[4]);
			estructuraAppeared.coordenadas.posY = atoi(argv[5]);
			enviar_mensaje(codigoOperacion, 2, 1, &estructuraAppeared, socket_cliente);
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg estructuraGet;
			estructuraGet.nombre_pokemon.nombre = argv[3];
			estructuraGet.nombre_pokemon.nombre_lenght = strlen(estructuraGet.nombre_pokemon.nombre)+1;
			enviar_mensaje(codigoOperacion, 3, 0, &estructuraGet, socket_cliente);
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg estructuraLocalized;
			estructuraLocalized.nombre_pokemon.nombre = argv[3];
			estructuraLocalized.nombre_pokemon.nombre_lenght = strlen(estructuraLocalized.nombre_pokemon.nombre)+1;
			estructuraLocalized.cantidad_coordenadas = atoi(argv[4]);
			estructuraLocalized.coordenadas = malloc(sizeof(uint32_t) * estructuraLocalized.cantidad_coordenadas * 2);
			int j = 5;
			for(int i = 0; i < estructuraLocalized.cantidad_coordenadas; i++)
			{
				estructuraLocalized.coordenadas[i].posX = atoi(argv[j]);
				estructuraLocalized.coordenadas[i].posY = atoi(argv[j+1]);
				j+=2;
			}
			enviar_mensaje(codigoOperacion, 4, 3, &estructuraLocalized, socket_cliente);
			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg estructuraCatch;
			estructuraCatch.nombre_pokemon.nombre = argv[3];
			estructuraCatch.nombre_pokemon.nombre_lenght = strlen(estructuraCatch.nombre_pokemon.nombre)+1;
			estructuraCatch.coordenadas.posX = atoi(argv[4]);
			estructuraCatch.coordenadas.posY = atoi(argv[5]);
			enviar_mensaje(codigoOperacion, 5, 0, &estructuraCatch, socket_cliente);
			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg estructuraCaught;
			estructuraCaught.atrapado = atoi(argv[3]);
			enviar_mensaje(codigoOperacion, 6, 5, &estructuraCaught, socket_cliente);
			break;
		case ERROR_CODIGO: printf("[!] Error en el codigo de operacion\n"); return -1; break;
		default: printf("[!] Error desconocido en el codigo de operacion\n"); return -1;
	}

	return EXIT_SUCCESS;
}

op_code stringACodigoOperacion(const char* string)
{
	for(int i = 0; i < sizeof(conversionCodigoOp) / sizeof(conversionCodigoOp[0]); i++)
	{
		if(!strcmp(string, conversionCodigoOp[i].str))
			return conversionCodigoOp[i].codigoOperacion;
	}
	return ERROR_CODIGO;
}

process_code stringACodigoProceso(const char* string)
{
	for(int i = 0; i < sizeof(conversionCodigoProceso) / sizeof(conversionCodigoProceso[0]); i++)
	{
		if(!strcmp(string, conversionCodigoProceso[i].str))
			return conversionCodigoProceso[i].codigoProceso;
	}
	return ERROR_PROCESO;
}

t_log* iniciar_logger(void)
{
	//TODO catchear si == NULL
	return log_create(GAMEBOY_LOG, GAMEBOY_NAME, true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	//TODO catchear si == NULL
	return config_create(GAMEBOY_CONFIG);
}


//TODO
void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	config_destroy(config);
//	log_destroy(logger);
	liberar_conexion(conexion);
}
