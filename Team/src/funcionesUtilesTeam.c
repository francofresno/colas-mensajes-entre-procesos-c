/*
 * funcionesUtilesTeam.c
 *
 *  Created on: 4 may. 2020
 *      Author: utnso
 */

#include "funcionesUtilesTeam.h"


t_queue* ponerEntrenadoresEnCola(t_config* config)
{
	t_list *entrenadores = list_create(); //Creamos la lista de entrenadores

	t_queue *hilosFuncionesEntrenadores = queue_create();

	t_coordenadas* coords = malloc(sizeof(t_coordenadas));
	char** coordenadasEntrenadores = config_get_array_value(config, "POSICIONES_ENTRENADORES");

	//t_nombrePokemon* pokemonesQuePosee = malloc(sizeof(t_nombrePokemon));
	char** pokemonesDeEntrenadores = config_get_array_value(config, "POKEMON_ENTRENADORES");

	//t_nombrePokemon* pokemonesQueQuiere = malloc(sizeof(t_nombrePokemon));
	char** pokemonesObjetivoDeEntrenadores = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");

	int i = 0, j = 0;



	while(coordenadasEntrenadores[i] != NULL){

		coords->posX = atoi(coordenadasEntrenadores[i]);
		coords->posY = atoi(coordenadasEntrenadores[i+2]);

		t_list* pokemonesQueTiene = (t_list*)list_get(organizarPokemones(pokemonesDeEntrenadores), j);
		t_list* pokemonesQueDesea = (t_list*)list_get(organizarPokemones(pokemonesObjetivoDeEntrenadores), j);

		uint32_t id_entrenador = generar_id();

		t_entrenador* entrenador = crear_entrenador(0 ,coords, pokemonesQueTiene, pokemonesQueDesea, list_size(pokemonesQueDesea));


		list_add(entrenadores, entrenador);


		j++;
		i += 3;

	}

	int cantidadEntrenadores = list_size(entrenadores);

	pthread_t pthread_id[cantidadEntrenadores];

	for(int a=0; a<cantidadEntrenadores; a++){

		t_entrenador* entrenador = (t_entrenador*)list_get(entrenadores, a);

		pthread_create(&pthread_id[a],NULL,(void*)buscarPokemones,entrenador);

		queue_push(hilosFuncionesEntrenadores, pthread_id[a]);

	}

	return hilosFuncionesEntrenadores;
}

t_entrenador* crear_entrenador(uint32_t id_entrenador, t_coordenadas* coordenadas, t_list* pokemonesQuePosee, t_list* pokemonesQueQuiere, uint32_t cantidad_pokemons)
{
	t_entrenador* entrenador=sizeof(entrenador);

	entrenador->id_entrenador = id_entrenador;
	entrenador->coordenadas= coordenadas;
	entrenador->pokemonesQuePosee = pokemonesQuePosee;
	entrenador->pokemonesQueQuiere = pokemonesQueQuiere;
	entrenador->cantidad_pokemons = cantidad_pokemons;


	return entrenador;
}


t_list* organizarPokemones(char** listaPokemones){ //tanto para pokemonesObjetivoDeEntrenadores como para pokemonesDeEntrenadores

		int j=0, w=0;

		 t_list* listaDePokemones = list_create();

		 t_list* listaDePokemonesDeEntrenadores = list_create();

		while(listaPokemones[j]!= NULL){ //recorro los pokemones de cada entrenador separado por coma

			char**pokemonesDeUnEntrenador= string_split(listaPokemones[j], '|'); //separo cada pokemon de un mismo entrenador separado por |

			while (pokemonesDeUnEntrenador[w]!= NULL){  //recorro todos y voy creando cada pokemon

				t_nombrePokemon* pokemon = crearPokemon(pokemonesDeUnEntrenador[w]);

				list_add(listaDePokemones, pokemon);

				w++;
			}

			list_add(listaDePokemonesDeEntrenadores, listaDePokemones);
			list_clean(listaDePokemones);

			j++;
		}


	return listaDePokemonesDeEntrenadores;

}


t_nombrePokemon* crear_pokemon(char* pokemon){

	t_nombrePokemon* nuevoPokemon = malloc(sizeof(nuevoPokemon));

	nuevoPokemon->nombre_lenght = strlen(pokemon)+1;
	nuevoPokemon->nombre = (pokemon);

	return nuevoPokemon;

}

uint32_t generar_id()
{
	pthread_mutex_lock(&mutex_id_counter);
	uint32_t id_generado = ++ID_COUNTER;
	pthread_mutex_unlock(&mutex_id_counter);

	return id_generado;
}

void buscarPokemones(t_entrenador* entrenador){

	printf ("Soy el entrenador con el id %d\n", entrenador->id_entrenador);
};
