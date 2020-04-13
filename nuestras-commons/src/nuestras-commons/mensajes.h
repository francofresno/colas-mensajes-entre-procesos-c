#ifndef MENSAJES_H_
#define MENSAJES_H_

	typedef struct {
		int posx;
		int posy;
	} t_coordenadas;

	typedef struct {
		char* nombre;
		t_coordenadas coords;
	} t_comun;

	typedef struct {
		t_comun comun;
		int cant;
	} t_new;

	typedef struct {
		t_comun comun;
	} t_appeared;

	typedef struct {
		t_comun comun;
	} t_catch;

	typedef struct {
		int estado;
	} t_caught;

	typedef struct {
		char* nombre;
	} t_get;

	typedef struct {
		char* nombre;
		int cantidad_coords;
		t_coordenadas **coords;
	} t_localized;

#endif /* MENSAJES_H_ */
