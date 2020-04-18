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

	t_new_message* new_pokemon_msg = malloc(sizeof(new_pokemon_msg));

	t_nombre nom;
	nom.nombre = "PIKACHU";
	nom.nombre_lenght = 8;

	t_coords coord;
	coord.posx = 1;
	coord.posy = 2;

	new_pokemon_msg->nombre_pokemon = nom;
	new_pokemon_msg->coords = coord;
	new_pokemon_msg->cantidad = 1;


	printf("%s\n",new_pokemon_msg->nombre_pokemon.nombre);
	fflush(stdout);

	printf("%d\n",sizeof(*new_pokemon_msg));
	fflush(stdout);

	enviar_mensaje(NEW_POKEMON, new_pokemon_msg, socket_cliente);

//	sleep(2);
//
//	socket_cliente = crear_conexion ("127.0.0.1", "6011");
//
//	enviar_mensaje(APPEARED_POKEMON, "APPEARED_POKEMON", socket_cliente);



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
