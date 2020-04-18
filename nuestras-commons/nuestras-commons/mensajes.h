#ifndef MENSAJES_H_
#define MENSAJES_H_

	#include <stdint.h>

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
	} t_new_message;

	typedef struct {
		t_nombre nombre_pokemon;
		t_coords coords;
	} t_appeared_message;

	typedef struct Catch_Message {
		t_nombre nombre_pokemon;
		t_coords coords;
	} t_catch_message;

	typedef struct Caught_Message {
		uint32_t estado;
	} t_caught_message;

	typedef struct Get_Message {
		t_nombre nombre_pokemon;
	} t_get_message;

	typedef struct Localized_Message {
		t_nombre nombre_pokemon;
		uint32_t cantidad_coords;
		t_coords **coords;
	} t_localized_message;
	// ---- END Estructuras de los mensajes ---- //

#endif /* MENSAJES_H_ */
