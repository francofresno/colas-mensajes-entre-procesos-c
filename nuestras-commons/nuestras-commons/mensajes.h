#ifndef MENSAJES_H_
#define MENSAJES_H_

#include <stdint.h>
#include "conexion.h"

/****************************************
 *********ESTRUCTURAS GENERALES**********
 ****************************************/

typedef struct {
	uint32_t nombre_lenght;
	char* nombre;
} t_nombrePokemon;

typedef struct {
	uint32_t posX;
	uint32_t posY;
} t_coordenadas;

/****************************************
 *******ESTRUCTURAS DE LOS MENSAJES******
 ****************************************/

typedef struct
{
	t_nombrePokemon* nombre_pokemon;
	t_coordenadas* coordenadas;
	uint32_t cantidad_pokemons;
} t_newPokemon_msg;

typedef struct
{
	t_nombrePokemon nombre_pokemon;
	t_coordenadas coordenadas;
} t_appearedPokemon_msg;

typedef struct
{
	t_nombrePokemon nombre_pokemon;
	t_coordenadas coordenadas;
} t_catchPokemon_msg;

typedef struct
{
	uint32_t atrapado;
} t_caughtPokemon_msg;

typedef struct
{
	t_nombrePokemon nombre_pokemon;
} t_getPokemon_msg;

typedef struct
{
	t_nombrePokemon nombre_pokemon;
	uint32_t cantidad_coordenadas;
	t_coordenadas coordenadas[];
} t_localizedPokemon_msg;


t_buffer* serializar_new_pokemon_msg(t_newPokemon_msg* new_pokemon_msg);
t_newPokemon_msg* deserializar_new_pokemon_msg(int socket_cliente);

#endif /* MENSAJES_H_ */
