/*
 * funcionesUtilesTeam.c
 *
 *  Created on: 4 may. 2020
 *      Author: utnso
 */

#include "funcionesUtilesTeam.h"

t_list* entrenadores;
t_list* hilosEntrenadores;

pthread_mutex_t mutex_id_entrenadores = PTHREAD_MUTEX_INITIALIZER;
/*
  ============================================================================
 	 	 	 	 	 	 	 	 HITO 2
  ============================================================================
*/

void ponerEntrenadoresEnLista(t_config* config) {
	entrenadores = list_create(); //Creamos la lista de entrenadores

	t_coordenadas* coords = malloc(sizeof(t_coordenadas));
	char** coordenadasEntrenadores = config_get_array_value(config,
			"POSICIONES_ENTRENADORES");

	//t_nombrePokemon* pokemonesQuePosee = malloc(sizeof(t_nombrePokemon));
	char** pokemonesDeEntrenadores = config_get_array_value(config,
			"POKEMON_ENTRENADORES");

	//t_nombrePokemon* pokemonesQueQuiere = malloc(sizeof(t_nombrePokemon));
	char** pokemonesObjetivoDeEntrenadores = config_get_array_value(config,
			"OBJETIVOS_ENTRENADORES");

	int i = 0, j = 0;

	t_list* listaDePokemonesDeEntrenadores = organizarPokemones(pokemonesDeEntrenadores);
	t_list* listaDePokemonesObjetivoDeEntrenadores = organizarPokemones(pokemonesObjetivoDeEntrenadores);

	while (coordenadasEntrenadores[i] != NULL) {

		coords->posX = atoi(&coordenadasEntrenadores[i][0]);
		coords->posY = atoi(&coordenadasEntrenadores[i][2]);

		t_list* pokemonesQueTiene = (t_list*) list_get(listaDePokemonesDeEntrenadores, j);
		t_list* pokemonesQueDesea = (t_list*) list_get(listaDePokemonesObjetivoDeEntrenadores, j);


		//TODO OBJETIVOS GLOBALES DEL TEMA --> VARIABLE GLOBAL.

		uint32_t id_entrenador = generar_id();

		t_entrenador* entrenador = crear_entrenador(id_entrenador, coords,
				pokemonesQueTiene, pokemonesQueDesea,
				list_size(pokemonesQueTiene), NEW);

		list_add(entrenadores, entrenador);

		j++;
		i++;

	}
}

void crearHilosEntrenadores() {

	hilosEntrenadores = list_create();

	int cantidadEntrenadores = list_size(entrenadores);

	pthread_t pthread_id[cantidadEntrenadores];

	for (int a = 0; a < cantidadEntrenadores; a++) {

		t_entrenador* entrenador = (t_entrenador*) list_get(entrenadores, a);

		pthread_create(&pthread_id[a], NULL, (void*) gestionarPokemones, entrenador);

		list_add(hilosEntrenadores, &pthread_id[a]);
	}

}


t_entrenador* crear_entrenador(uint32_t id_entrenador,
		t_coordenadas* coordenadas, t_list* pokemonesQuePosee,
		t_list* pokemonesQueQuiere, uint32_t cantidad_pokemons,
		status_code estado) {
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));

	entrenador->id_entrenador = id_entrenador;
	entrenador->coordenadas = coordenadas;
	entrenador->pokemonesQuePosee = pokemonesQuePosee;
	entrenador->pokemonesQueQuiere = pokemonesQueQuiere;
	entrenador->cantidad_pokemons = cantidad_pokemons;
	entrenador->estado = estado;

	return entrenador;
}

t_list* organizarPokemones(char** listaPokemones) { //tanto para pokemonesObjetivoDeEntrenadores como para pokemonesDeEntrenadores

	int j = 0, w = 0;

	t_list* listaDePokemonesDeEntrenadores = list_create();

	while (listaPokemones[j] != NULL) { //recorro los pokemones de cada entrenador separado por coma
		char pipe = '|';
		char**pokemonesDeUnEntrenador = string_split(listaPokemones[j], &pipe); //separo cada pokemon de un mismo entrenador separado por |

		t_list* listaDePokemones = list_create();

		while (pokemonesDeUnEntrenador[w] != NULL) { //recorro todos y voy creando cada pokemon

			t_nombrePokemon* pokemon = crear_pokemon(pokemonesDeUnEntrenador[w]);

			list_add(listaDePokemones, pokemon);

			w++;
		}

		list_add(listaDePokemonesDeEntrenadores, listaDePokemones);

		j++;
		w=0;
	}

	return listaDePokemonesDeEntrenadores;

}

t_nombrePokemon* crear_pokemon(char* pokemon) {

	t_nombrePokemon* nuevoPokemon = malloc(sizeof(t_nombrePokemon));

	nuevoPokemon->nombre_lenght = strlen(pokemon) + 1;
	nuevoPokemon->nombre = pokemon;

	return nuevoPokemon;

}

uint32_t generar_id() {
	pthread_mutex_lock(&mutex_id_entrenadores);
	uint32_t id_generado = ++ID_ENTRENADORES;
	pthread_mutex_unlock(&mutex_id_entrenadores);

	return id_generado;
}

void buscarPokemones(t_entrenador* entrenador) {

	printf("El estado del entrenador es: %d\n", entrenador->estado);
}

void gestionarPokemones(t_entrenador* entrenador){
	printf("Soy el entrenador con el id %d\n", entrenador->id_entrenador);
}

void planificarSegun(t_config* config) {

	char* algoritmoPlanificacion = config_get_string_value(config,
			"ALGORITMO_PLANIFICACION");
	//char* quantum= config_get_array_value(config, "QUANTUM");
	//char* quantum= config_get_array_value(config, "ESTIMACION_INICIAL");

	switch (stringACodigoAlgoritmo(algoritmoPlanificacion)) {

	case FIFO:

		planificarSegunFifo(entrenadores);

		break;

	case RR:

		puts("Planifico segun RR \n");

		break;

	case SJFCD:

		puts("Planifico segun SFJ-CD \n");

		break;
	case SJFSD:

		puts("Planifico segun SFJ-SD \n");

		break;

	case ERROR_CODIGO_ALGORITMO:

		puts("Se recibio mal el codigo\n");

		break;

	default:

		puts("Error desconocido\n");

		break;

	}

}

void planificarSegunFifo() {

	t_estructuraCola estructura_cola;

	while (!list_is_empty(entrenadores)) {

		int tamanio = list_size(entrenadores);

		for (int b = 0; b < tamanio; b++) {

			t_entrenador* entrenador = (t_entrenador*) list_get(entrenadores, b);

			if (entrenador->estado == READY) {

				list_remove(entrenadores, b);

				entrenador->estado = EXEC;
				queue_push(estructura_cola.colaEnEjecucion, entrenador);
				buscarPokemones(entrenador); //Buscar pokemones cambia el estado a finalizado o bloqueado.

				if (entrenador->estado == FINISHED) {
					queue_pop(estructura_cola.colaEnEjecucion);
					queue_push(estructura_cola.colaFinalizados, entrenador);
				}

				if (entrenador->estado == BLOCKED){
					queue_pop(estructura_cola.colaEnEjecucion);
					queue_push(estructura_cola.colaBloqueados, entrenador);
				}
			}
		}
	}
}

algoritmo_code stringACodigoAlgoritmo(const char* string) {
	for (int i = 0;
			i < sizeof(conversionAlgoritmo) / sizeof(conversionAlgoritmo[0]);
			i++) {
		if (!strcmp(string, conversionAlgoritmo[i].str))
			return conversionAlgoritmo[i].codigo_algoritmo;
	}
	return ERROR_CODIGO_ALGORITMO;
}

/*
  ============================================================================
 	 	 	 	 	 	 	 	 HITO 3
  ============================================================================
*/




