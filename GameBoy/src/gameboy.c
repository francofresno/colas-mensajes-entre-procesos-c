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

	//t_config* config = leer_config();

	//char* ip = config_get_string_value(config, IP);
	//char* puerto = config_get_string_value(config, PUERTO);

	//TODO catch si crear_conexion devuelve error (si es -1)
	int socket_cliente = crear_conexion ("127.0.0.1", "6011");

	t_newPokemon_msg new;
	new.nombre_pokemon.nombre = "PIKACHU";
	new.nombre_pokemon.nombre_lenght = 8;
	new.coordenadas.posX = 6;
	new.coordenadas.posY = 2;
	new.cantidad_pokemons = 7;


	t_appearedPokemon_msg appeared;
	appeared.nombre_pokemon.nombre = "PIKACHU";
	appeared.nombre_pokemon.nombre_lenght = 8;
	appeared.coordenadas.posX = 1;
	appeared.coordenadas.posY = 2;


	t_getPokemon_msg get;
	get.nombre_pokemon.nombre = "PIKACHU";
	get.nombre_pokemon.nombre_lenght = 8;


	t_localizedPokemon_msg localized;
	localized.nombre_pokemon.nombre = "PIKACHU";
	localized.nombre_pokemon.nombre_lenght = 8;
	localized.cantidad_coordenadas = 2;
	localized.coordenadas = malloc(sizeof(uint32_t) * localized.cantidad_coordenadas * 2);
	localized.coordenadas[0].posX = 1;
	localized.coordenadas[0].posY = 2;
	localized.coordenadas[1].posX = 3;
	localized.coordenadas[1].posY = 4;


	t_catchPokemon_msg catch;
	catch.nombre_pokemon.nombre = "PIKACHU";
	catch.nombre_pokemon.nombre_lenght = 8;
	catch.coordenadas.posX = 1;
	catch.coordenadas.posY = 2;


	t_caughtPokemon_msg caught;
	caught.atrapado = 1;

	op_code n = NEW_POKEMON;
	op_code a = APPEARED_POKEMON;
	op_code g = GET_POKEMON;
	op_code l = LOCALIZED_POKEMON;
	op_code cat = CATCH_POKEMON;
	op_code cau = CAUGHT_POKEMON;

	enviar_mensaje(cau, &caught, socket_cliente);

	return EXIT_SUCCESS;
}

t_log* iniciar_logger(void)
{
	//TODO catchear si == NULL
	return log_create(GAMEBOY_LOG, GAMEBOY_NAME, true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	//TODO catchear si == NULL
	return config_create("tp0.config");
}


//TODO
void terminar_programa(int conexion, t_log* logger, t_config* config)
{


}
