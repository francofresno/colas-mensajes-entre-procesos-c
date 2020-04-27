/*
 ============================================================================
 Name        : GameBoy
 Author      : Fran and Co
 Description : Proceso GameBoy
 ============================================================================
 */

#include "gameboy.h"

int main(int argc, char *argv[]) {


	//./gameboy [PROCESO] [TIPO_MENSAJE] [ARGUMENTOS]*

	//./gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]

	t_config* config = leer_config();
	char* ip;
	char* puerto;

	op_code codigoOperacion = stringACodigoOperacion(argv[2]);
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
	}

	int socket_cliente = crear_conexion (ip, puerto);

	switch(codigoOperacion)
	{
		case NEW_POKEMON: ;
			t_newPokemon_msg new;
			new.nombre_pokemon.nombre = "PIKACHU";
			new.nombre_pokemon.nombre_lenght = 8;
			new.coordenadas.posX = 6;
			new.coordenadas.posY = 2;
			new.cantidad_pokemons = 7;
			enviar_mensaje(codigoOperacion, &new, socket_cliente);
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg appeared;
			appeared.nombre_pokemon.nombre = "PIKACHU";
			appeared.nombre_pokemon.nombre_lenght = 8;
			appeared.coordenadas.posX = 1;
			appeared.coordenadas.posY = 2;
			enviar_mensaje(codigoOperacion, &appeared, socket_cliente);
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg get;
			get.nombre_pokemon.nombre = "PIKACHU";
			get.nombre_pokemon.nombre_lenght = 8;
			enviar_mensaje(codigoOperacion, &get, socket_cliente);
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg localized;
			localized.nombre_pokemon.nombre = "PIKACHU";
			localized.nombre_pokemon.nombre_lenght = 8;
			localized.cantidad_coordenadas = 2;
			localized.coordenadas = malloc(sizeof(uint32_t) * localized.cantidad_coordenadas * 2);
			localized.coordenadas[0].posX = 1;
			localized.coordenadas[0].posY = 2;
			localized.coordenadas[1].posX = 3;
			localized.coordenadas[1].posY = 4;
			enviar_mensaje(codigoOperacion, &localized, socket_cliente);
			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg catch;
			catch.nombre_pokemon.nombre = "PIKACHU";
			catch.nombre_pokemon.nombre_lenght = 8;
			catch.coordenadas.posX = 1;
			catch.coordenadas.posY = 2;
			enviar_mensaje(codigoOperacion, &catch, socket_cliente);
			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg caught;
			caught.atrapado = 1;
			enviar_mensaje(codigoOperacion, &caught, socket_cliente);
			break;
	}

	return EXIT_SUCCESS;
}

op_code stringACodigoOperacion(const char* string)
{
	for(int i = 0; i < sizeof(conversionCodigoOp) / sizeof(conversionCodigoOp[0]); ++i)
	{
		if(!strcmp(string, conversionCodigoOp[i].str))
			return conversionCodigoOp[i].codigoOperacion;
	}
	printf("[!] Error en el codigo de operacion");
}

process_code stringACodigoProceso(const char* string)
{
	for(int i = 0; i < sizeof(conversionCodigoProceso) / sizeof(conversionCodigoProceso[0]); ++i)
		{
			if(!strcmp(string, conversionCodigoProceso[i].str))
				return conversionCodigoProceso[i].process_code;
		}
		printf("[!] Error en el codigo de operacion");
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
