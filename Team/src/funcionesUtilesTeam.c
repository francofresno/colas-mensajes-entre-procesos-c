/*
 * funcionesUtilesTeam.c
 *
 *  Created on: 4 may. 2020
 *      Author: utnso
 */

#include "funcionesUtilesTeam.h"

/*
  ============================================================================
 	 	 	 	 	 	 	 	 HITO 2
  ============================================================================
*/

t_list* ponerEntrenadoresEnLista(t_config* config) {
	t_list *entrenadores = list_create(); //Creamos la lista de entrenadores

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

	while (coordenadasEntrenadores[i] != NULL) {

		coords->posX = atoi(coordenadasEntrenadores[i]);
		coords->posY = atoi(coordenadasEntrenadores[i + 2]);

		t_list* pokemonesQueTiene = (t_list*) list_get(
				organizarPokemones(pokemonesDeEntrenadores), j);
		t_list* pokemonesQueDesea = (t_list*) list_get(
				organizarPokemones(pokemonesObjetivoDeEntrenadores), j);

		uint32_t id_entrenador = generar_id();

		t_entrenador* entrenador = crear_entrenador(id_entrenador, coords,
				pokemonesQueTiene, pokemonesQueDesea,
				list_size(pokemonesQueDesea), NEW);

		list_add(entrenadores, entrenador);

		j++;
		i += 3;

	}

	return entrenadores;				//retornar listaDeEntrenadores
}

t_list* crearHilosEntrenadores(t_list* listaEntrenadores) {

	t_list *hilosFuncionesEntrenadores = list_create();

	int cantidadEntrenadores = list_size(listaEntrenadores);

	pthread_t pthread_id[cantidadEntrenadores];

	for (int a = 0; a < cantidadEntrenadores; a++) {

		t_entrenador* entrenador = (t_entrenador*) list_get(listaEntrenadores,
				a);

		pthread_create(&pthread_id[a], NULL, (void*) gestionarPokemones, entrenador);

		list_add(hilosFuncionesEntrenadores, &pthread_id[a]);
	}

	return hilosFuncionesEntrenadores;
}

t_entrenador* crear_entrenador(uint32_t id_entrenador,
		t_coordenadas* coordenadas, t_list* pokemonesQuePosee,
		t_list* pokemonesQueQuiere, uint32_t cantidad_pokemons,
		status_code estado) {
	t_entrenador* entrenador = malloc(sizeof(entrenador));

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

	t_list* listaDePokemones = list_create();

	t_list* listaDePokemonesDeEntrenadores = list_create();

	while (listaPokemones[j] != NULL) { //recorro los pokemones de cada entrenador separado por coma
		char* pipe = "|";
		char**pokemonesDeUnEntrenador = string_split(listaPokemones[j], pipe); //separo cada pokemon de un mismo entrenador separado por |

		while (pokemonesDeUnEntrenador[w] != NULL) { //recorro todos y voy creando cada pokemon

			t_nombrePokemon* pokemon = crear_pokemon(
					pokemonesDeUnEntrenador[w]);

			list_add(listaDePokemones, pokemon);

			w++;
		}

		list_add(listaDePokemonesDeEntrenadores, listaDePokemones);
		list_clean(listaDePokemones);

		j++;
	}

	return listaDePokemonesDeEntrenadores;

}

t_nombrePokemon* crear_pokemon(char* pokemon) {

	t_nombrePokemon* nuevoPokemon = malloc(sizeof(t_nombrePokemon));

	nuevoPokemon->nombre_lenght = strlen(pokemon) + 1;
	nuevoPokemon->nombre = (pokemon);

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


void planificarSegun(t_config* config, t_list* listaDeEntrenadores) {

	char* algoritmoPlanificacion = config_get_string_value(config,
			"ALGORITMO_PLANIFICACION");
	//char* quantum= config_get_array_value(config, "QUANTUM");
	//char* quantum= config_get_array_value(config, "ESTIMACION_INICIAL");

	switch (stringACodigoAlgoritmo(algoritmoPlanificacion)) {

	case FIFO:

		planificarSegunFifo(listaDeEntrenadores);

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

void planificarSegunFifo(t_list* listaDeEntrenadores) {

	t_estructuraCola estructura_cola;

	while (!list_is_empty(listaDeEntrenadores)) {

		int tamanio = list_size(listaDeEntrenadores);

		for (int b = 0; b < tamanio; b++) {

			t_entrenador* entrenador = (t_entrenador*) list_get(listaDeEntrenadores, b);

			if (entrenador->estado == READY) {

				list_remove(listaDeEntrenadores, b);

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

op_code stringACodigoOperacion(const char* string)
{
	for(int i = 0; i < sizeof(conversionCodigoOp) / sizeof(conversionCodigoOp[0]); i++)
	{
		if(!strcmp(string, conversionCodigoOp[i].str))
			return conversionCodigoOp[i].codigoOperacion;
	}
	return ERROR_CODIGO;
}

process_code stringACodigoProceso(const char* string)
{
	for(int i = 0; i < sizeof(conversionCodigoProceso) / sizeof(conversionCodigoProceso[0]); i++)
	{
		if(!strcmp(string, conversionCodigoProceso[i].str))
			return conversionCodigoProceso[i].codigoProceso;
	}
	return ERROR_PROCESO;
}

