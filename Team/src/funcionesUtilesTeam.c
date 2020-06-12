/*
 * funcionesUtilesTeam.c
 *
 *  Created on: 4 may. 2020
 *      Author: utnso
 */

#include "funcionesUtilesTeam.h"

t_list* entrenadores;
t_list* hilosEntrenadores;
t_list* objetivoTeam;

//Listas de entrenadores segun estado
t_list* listaNuevos;
t_list* listaReady;
t_list* listaBloqueadosDeadlock;
t_list* listaBloqueadosEsperandoMensaje;
t_list* listaBloqueadosEsperandoPokemones;
t_list* listaFinalizados;

char* algoritmoPlanificacion;
int quantum;
int estimacionInicial;
double alfa;

pthread_mutex_t mutex_id_entrenadores = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_entrenador = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_entrenadores_ejecutar = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_hay_pokemones = PTHREAD_MUTEX_INITIALIZER;
/*
  ============================================================================
 	 	 	 	 	 	 	 	 HITO 2
  ============================================================================
*/

void ponerEntrenadoresEnLista(t_config* config) {

	inicializarListasDeEstados();

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

		uint32_t id_entrenador = generar_id();

		t_entrenador* entrenador = crear_entrenador(id_entrenador, coords,
				pokemonesQueTiene, pokemonesQueDesea,
				list_size(pokemonesQueTiene), NEW);

		list_add(entrenadores, entrenador);
		list_add(listaNuevos, entrenador);

		j++;
		i++;

	}

	hacerObjetivoTeam(listaDePokemonesDeEntrenadores, listaDePokemonesObjetivoDeEntrenadores);
}

void crearHilosEntrenadores() {

	hilosEntrenadores = list_create();

	int cantidadEntrenadores = list_size(entrenadores);

	pthread_t pthread_id[cantidadEntrenadores];

	for (int a = 0; a < cantidadEntrenadores; a++) {

		t_entrenador* entrenador = (t_entrenador*) list_get(entrenadores, a);

		pthread_create(&pthread_id[a], NULL, (void*) ejecutarEntrenador, entrenador);

		list_add(hilosEntrenadores, &pthread_id[a]);
	}

}

void inicializarListasDeEstados(){

listaNuevos = list_create();
listaReady = list_create();
listaBloqueadosDeadlock= list_create();
listaBloqueadosEsperandoMensaje= list_create();
listaBloqueadosEsperandoPokemones = list_create();
listaFinalizados = list_create();

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

void ejecutarEntrenador(t_entrenador* entrenador){
	printf("Soy el entrenador con el id %d\n", entrenador->id_entrenador);
	entrenador->estado = FINISHED;
		printf("El estado del entrenador de id %d es: %d\n", entrenador->id_entrenador, entrenador->estado);
	pthread_mutex_lock(&mutex_entrenadores_ejecutar);
}

void hacerObjetivoTeam(t_list* listaPokemonesTieneEntrenadores, t_list* listaPokemonesDeseaEntrenadores){ //Siempre Despues de Usar estas Listas

	 t_list* listaGrande = list_create();
	 t_list* listaMini = list_create();

	 listaGrande = aplanarDobleLista(listaPokemonesDeseaEntrenadores);
	 listaMini = aplanarDobleLista(listaPokemonesTieneEntrenadores);

	 contiene(listaGrande, listaMini);

}

t_list* aplanarDobleLista(t_list* lista){

	t_list* listaAplanada = list_create();

	int tamanioListaSuprema = list_size(lista);

		for(int b=0; b<tamanioListaSuprema ;b++){

			 int tamanioSubLista = list_size(list_get(lista, b));  //aca esta el error

			 for(int a=0; a<tamanioSubLista; a++){

				 list_add(listaAplanada, (t_nombrePokemon*)list_get(list_get(lista, b), a));
			 }
		}

	return listaAplanada;
}

void contiene(t_list* listaA, t_list* listaB){ 		//listaGrande A lista chica B

	objetivoTeam = list_create();

	int a = list_size(listaA);

	for(int i=0; i < a; i++){

		int b = list_size(listaB);
		int j=0;

		while((j < b) && (sonIguales(list_get(listaB,j), list_get(listaA, i))!=0)){
			j++;
		}

		if(j==b){
			list_add(objetivoTeam, (t_nombrePokemon*)list_get(listaA, i));
		}else{
			list_remove(listaB, j);
		}

	}
}

int sonIguales(t_nombrePokemon* pokemon1, t_nombrePokemon* pokemon2){
	return strcmp(pokemon1->nombre, pokemon2->nombre);					//retorna un 0 si cumple
}

t_entrenador* entrenadorMasCercano(t_newPokemon* pokemon){
	t_entrenador* entrenadorTemporal;
	t_entrenador* entrenadorMasCercanoBlocked;

	int distanciaTemporal;
	int menorDistanciaBlocked;

	t_list* entrenadores_bloqueados = listaBloqueadosEsperandoPokemones;

	if(!list_is_empty(entrenadores_bloqueados)){
		entrenadorMasCercanoBlocked = list_get(entrenadores_bloqueados, 0);
		menorDistanciaBlocked = distanciaA(entrenadorMasCercanoBlocked->coordenadas, pokemon->coordenadas);

		for(int i=0; i < entrenadores_bloqueados->elements_count; i++){

			if(menorDistanciaBlocked ==0){
				break;
			}

			entrenadorTemporal = list_get(entrenadores_bloqueados, i);
			distanciaTemporal = distanciaA(entrenadorTemporal->coordenadas, pokemon->coordenadas);

			if(distanciaTemporal < menorDistanciaBlocked){
				entrenadorMasCercanoBlocked = entrenadorTemporal;
				menorDistanciaBlocked = distanciaTemporal;
			}
		}
	}

	t_list* entrenadores_new = listaNuevos;
	t_entrenador* entrenadorMasCercanoNew;
	int menorDistanciaNew;


	if(!list_is_empty(entrenadores_new)){
		entrenadorMasCercanoNew = list_get(entrenadores_new, 0);
		menorDistanciaNew = distanciaA(entrenadorMasCercanoNew->coordenadas, pokemon->coordenadas);


		for(int i = 1; i < entrenadores_new->elements_count; i++){

			if(menorDistanciaNew == 0){
				break;
			}

			entrenadorTemporal = list_get(entrenadores_new, i);
			distanciaTemporal = distanciaA(entrenadorTemporal->coordenadas, pokemon->coordenadas);

			if(distanciaTemporal < menorDistanciaNew){
				entrenadorMasCercanoNew = entrenadorTemporal;
				menorDistanciaNew = distanciaTemporal;
			}

		}
	}

	if(menorDistanciaNew <= menorDistanciaBlocked ){ //TODO despertar o poner a ejecutar.
		entrenadorMasCercanoNew->pokemonInstantaneo = pokemon;
		return entrenadorMasCercanoNew;

	} else{
		entrenadorMasCercanoBlocked->pokemonInstantaneo = pokemon;
		return entrenadorMasCercanoBlocked;
	}

}

int distanciaA(t_coordenadas* desde, t_coordenadas* hasta){

	int distanciaX = abs(desde->posX - hasta->posX);
	int distanciaY = abs(desde->posY - hasta->posY);

	return distanciaX + distanciaY;
}

void buscarPokemon(t_newPokemon* pokemon){

	//pthread_mutex_lock(&mutex_entrenadores_ejecutar); //TODO hacer impide que otro entrenador ejecute a la par
	t_entrenador* entrenador = entrenadorMasCercano(pokemon);

	entrenador->estado = READY;

	planificarSegun();

	//pthread_mutex_unlock(&mutex_entrenador); 		//a un entrenador no se le asignen más de un pokemon al haber un appeard
	//pthread_mutex_unlock(&mutex_entrenador_hilo); //signal al hilo del entrenador que va a ejecutar

}

void planificarSegun() {


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

void planificarSegunFifo() {  //TODO semaforos con mensaje appeard

	int tamanio = list_size(listaReady);

	for (int b = 0; b < tamanio; b++) {

		t_entrenador* entrenador = (t_entrenador*) list_get(listaReady, b);

			entrenador->estado = EXEC;
			ejecutarEntrenador(entrenador); //Buscar pokemones cambia el estado a finalizado o bloqueado.

			if (entrenador->estado == FINISHED) {
				list_add(listaFinalizados, entrenador);
			}

			if (entrenador->estado == BLOCKED){
				list_add(listaBloqueadosDeadlock, entrenador);
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





