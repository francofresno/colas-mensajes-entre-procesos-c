#ifndef MENSAJES_H_
#define MENSAJES_H_

	#include <stdint.h>
 	#include "conexion.h"

	typedef struct  Nombre_Pokemon {
		uint32_t nombre_lenght;
		char* nombre;
	} t_nombre;

	typedef struct Coordenadas {
		uint32_t posx;
		uint32_t posy;
	} t_coords;


	// ---- Estructuras de los mensajes ---- //
	typedef struct New_Message {
		t_nombre nombre_pokemon;
		t_coords coords;
		uint32_t cantidad;
	} t_new_pokemon_msg;

	typedef struct {
		t_nombre nombre_pokemon;
		t_coords coords;
	} t_appeared_pokemon_msg;

	typedef struct Catch_Message {
		t_nombre nombre_pokemon;
		t_coords coords;
	} t_catch_pokemon_msg;

	typedef struct Caught_Message {
		uint32_t estado;
	} t_caught_pokemon_msg;

	typedef struct Get_Message {
		t_nombre nombre_pokemon;
	} t_get_pokemon_msg;

	typedef struct Localized_Message {
		t_nombre nombre_pokemon;
		uint32_t cantidad_coords;
		t_coords **coords;
	} t_localized_pokemon_msg;
	// ---- END Estructuras de los mensajes ---- //


	t_buffer* serializar_new_pokemon_msg(t_new_pokemon_msg* new_pokemon_msg);
	t_new_pokemon_msg* deserializar_new_pokemon_msg(int socket_cliente);

#endif /* MENSAJES_H_ */
