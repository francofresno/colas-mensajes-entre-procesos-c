#ifndef MENSAJES_H_
#define MENSAJES_H_

	typedef struct {
		uint32_t nombre_lenght;
		char* nombre_pokemon;
	} t_nombre_pokemon;

	typedef struct {
		uint32_t posx;
		uint32_t posy;
	} t_coordenadas;

	// ---- Estructuras de los mensajes ---- //
	typedef struct {
		t_nombre_pokemon nombre_pokemon;
		t_coordenadas coords;
		uint32_t cantidad;
	} t_new;

	typedef struct {
		t_nombre_pokemon nombre_pokemon;
		t_coordenadas coords;
	} t_appeared;

	typedef struct {
		t_nombre_pokemon nombre_pokemon;
		t_coordenadas coords;
	} t_catch;

	typedef struct {
		uint32_t estado;
	} t_caught;

	typedef struct {
		t_nombre_pokemon nombre_pokemon;
	} t_get;

	typedef struct {
		t_nombre_pokemon nombre_pokemon;
		uint32_t cantidad_coords;
		t_coordenadas **coords;
	} t_localized;
	// ---- END Estructuras de los mensajes ---- //

#endif /* MENSAJES_H_ */
