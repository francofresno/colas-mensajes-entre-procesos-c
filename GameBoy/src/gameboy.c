/*
 ============================================================================
 Name        : GameBoy
 Author      : Fran and Co
 Description : Proceso GameBoy
 ============================================================================
 */

#include "gameboy.h"

#include <unistd.h>

int main(int argc, char *argv[]) {


	//./gameboy [PROCESO] [TIPO_MENSAJE] [ARGUMENTOS]*

	//./gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]

	//t_config* config = leer_config();

	//char* ip = config_get_string_value(config, IP);
	//char* puerto = config_get_string_value(config, PUERTO);

	//TODO catch si crear_conexion devuelve error (si es -1)
	int socket_cliente = crear_conexion ("127.0.0.1", "6011");

	t_newPokemon_msg* new_pokemon_msg = malloc(sizeof(new_pokemon_msg));

	t_nombrePokemon* nom = malloc(sizeof(t_nombrePokemon));
	nom->nombre = "PIKACHU";
	nom->nombre_lenght = strlen(nom->nombre)+1;

	t_coordenadas* coordenadas = malloc(sizeof(t_coordenadas));
	coordenadas->posX = 3;
	coordenadas->posY = 4;

	new_pokemon_msg->nombre_pokemon = nom;
	new_pokemon_msg->coordenadas = coordenadas;
	new_pokemon_msg->cantidad_pokemons = 1;

	t_buffer* buffer = malloc(sizeof(buffer));
	buffer = serializar_new_pokemon_msg(new_pokemon_msg);

	enviar_mensaje(NEW_POKEMON, buffer, socket_cliente);

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
