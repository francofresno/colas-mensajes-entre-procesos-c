/*
 * planificador.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#include "planificador.h"

pthread_mutex_t mutex_atrapados = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pendientes = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_objetivoTeam = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_listaNuevos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaReady = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaBloqueadosDeadlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaBloqueadosEsperandoMensaje = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaBloqueadosEsperandoPokemones = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaFinalizados = PTHREAD_MUTEX_INITIALIZER;


void planificarSegun() {


	switch (stringACodigoAlgoritmo(algoritmoPlanificacion)) {

	case FIFO:

		planificarSegunFifo();

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
	int distancia;

	for (int i = 0; i < tamanio; i++) {

		sem_wait(&sem_planificar);
		pthread_mutex_lock(&mutex_listaReady);
		t_entrenador* entrenador = (t_entrenador*) list_remove(listaReady, i);
		pthread_mutex_unlock(&mutex_listaReady);
		entrenador->estado=EXEC;

		if(!(entrenador->idMensajeCaught)){

			do{
				distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);
				sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
				sem_post(semaforoDelEntrenador);//TODO hacer impide que otro entrenador ejecute a la par
			}while(distancia !=0);

			if(entrenador->idMensajeCaught){
				entrenador->estado = BLOCKED;
				pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
				list_add(listaBloqueadosEsperandoMensaje, entrenador);
				pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje); //TODO todos los mutex a estas listas
			} else{
				verificarTieneTodoLoQueQuiere(entrenador);
			}
		} else{
			//TODO llego un mensaje caught
		}


		//TODO deadlock


		//TODO poner nuevamente en null al pokeInstant o sera un semaforo más
		sem_post(&sem_planificar);
	}



	int tamanioDeadlock = list_size(listaBloqueadosDeadlock); //TODO ver donde ponerlo
	for (int b = 0; b < tamanioDeadlock; b++) {

		t_entrenador* entrenador = (t_entrenador*) list_remove(listaBloqueadosDeadlock, 0);
		entrenador->estado = EXEC;

		int tamanioModificado = list_size(listaBloqueadosDeadlock);
		for(int a=0; a<tamanioModificado ; a++){
			//algo
		}
	}

}

int distanciaA(t_coordenadas* desde, t_coordenadas* hasta){

	int distanciaX = abs(desde->posX - hasta->posX);
	int distanciaY = abs(desde->posY - hasta->posY);

	return distanciaX + distanciaY;
}

int tieneTodoLoQueQuiere(t_entrenador* entrenador){

	t_list* listaTodoLoQueQuiere = list_duplicate(entrenador->pokemonesQueQuiere);
	t_list* listaTodoLoQuePosee = list_duplicate(entrenador->pokemonesQuePosee);
	t_list* diferencia = list_create();

	diferenciaYCargarLista(listaTodoLoQueQuiere, listaTodoLoQuePosee, diferencia);

	list_destroy(listaTodoLoQueQuiere);
	list_destroy(listaTodoLoQuePosee);

	return list_is_empty(diferencia);

}

void diferenciaYCargarLista(t_list* listaA, t_list* listaB, t_list* listaACargar){ 		//listaGrande A lista chica B

	int a = list_size(listaA);

	for(int i=0; i < a; i++){

		int b = list_size(listaB);
		int j=0;

		while((j < b) && (!sonIguales(list_get(listaB,j), list_get(listaA, i)))){
			j++;
		}

		if(j==b){
			list_add(listaACargar, list_get(listaA, i));
		}else{
			list_remove(listaB, j);
		}

	}
}

int sonIguales(t_nombrePokemon* pokemon1, t_nombrePokemon* pokemon2){
	return strcmp(pokemon1->nombre, pokemon2->nombre) == 0;
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

void inicializarListasDeEstados(){

	listaNuevos = list_create();
	listaReady = list_create();
	listaBloqueadosDeadlock= list_create();
	listaBloqueadosEsperandoMensaje= list_create();
	listaBloqueadosEsperandoPokemones = list_create();
	listaFinalizados = list_create();

}

void verificarTieneTodoLoQueQuiere(t_entrenador* entrenador){ //TODO MUTEX listas
	if(entrenador->cantidad_pokemons == list_size(entrenador->pokemonesQueQuiere)){
		if(tieneTodoLoQueQuiere(entrenador)){
			entrenador->estado = FINISHED;
			list_add(listaFinalizados, entrenador);
		} else{
			entrenador->estado = BLOCKED;
			list_add(listaBloqueadosDeadlock, entrenador);
		}
	}else{
		entrenador->estado = BLOCKED;
		list_add(listaBloqueadosEsperandoPokemones, entrenador);
	}
}


