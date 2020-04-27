#ifndef MENSAJES_H_
#define MENSAJES_H_

#include <stdint.h>

/****************************************
 *********ESTRUCTURAS GENERALES**********
 ****************************************/

typedef struct
{
	uint32_t nombre_lenght;
	char* nombre;
} t_nombrePokemon;

typedef struct
{
	uint32_t posX;
	uint32_t posY;
} t_coordenadas;

typedef struct
{
	int id_entrenador;
	t_coordenadas coordenadas;
	t_nombrePokemon pokemones;
	uint32_t cantidad_pokemons;
} t_entrenador;

/****************************************
 *******ESTRUCTURAS DE LOS MENSAJES******
 ****************************************/

typedef struct
{
	t_nombrePokemon nombre_pokemon;
	t_coordenadas coordenadas;
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
	t_coordenadas* coordenadas;
} t_localizedPokemon_msg;


#endif /* MENSAJES_H_ */
