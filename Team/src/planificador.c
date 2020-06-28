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

	pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
	if(!(list_is_empty(listaBloqueadosEsperandoMensaje))){
		int j = list_size(listaBloqueadosEsperandoMensaje);
		for(int i=0; i<j ; i++){
			t_entrenador* entrenador = list_get(listaBloqueadosEsperandoMensaje, i);
			if(entrenador->puedeAtrapar){
				entrenador->estado = READY;
				pthread_mutex_lock(&mutex_listaReady);
				list_add(listaReady, entrenador);
				pthread_mutex_unlock(&mutex_listaReady);
				list_remove(listaBloqueadosEsperandoMensaje, i);
			}
		}
	}
	pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje);

	int distancia;

	pthread_mutex_lock(&mutex_listaReady);

	int tamanio = list_size(listaReady);

	for (int i = 0; i < tamanio; i++) {

		sem_wait(&sem_planificar);

		t_entrenador* entrenador = (t_entrenador*) list_remove(listaReady, i);

		entrenador->estado=EXEC;

		if(!(entrenador->puedeAtrapar)){ 		//TODO fijarnos

			do{
				distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);
				sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
				sem_post(semaforoDelEntrenador);//TODO hacer impide que otro entrenador ejecute a la par
				//TODO esta llamando realmente al hilo del entrenador?
			}while(distancia !=0);

			if(entrenador->idMensajeCaught){
				entrenador->estado = BLOCKED;

				pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
				list_add(listaBloqueadosEsperandoMensaje, entrenador);
				pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje);

			} else{
				verificarTieneTodoLoQueQuiere(entrenador);
			}
		} else{
			sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
			sem_post(semaforoDelEntrenador);
			//Se llama al hilo o lo que sea (se desbloquea el hilo del entrandor en cuestion)
		}


		//TODO deadlock

		sem_post(&sem_planificar);
	}

	pthread_mutex_unlock(&mutex_listaReady);


	pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
	int tamanioDeadlock = list_size(listaBloqueadosDeadlock); //TODO ver donde ponerlo
	for (int b = 0; b < tamanioDeadlock; b++) {

		t_entrenador* entrenador = (t_entrenador*) list_remove(listaBloqueadosDeadlock, 0);
		entrenador->estado = EXEC;

		int tamanioModificado = list_size(listaBloqueadosDeadlock);
		for(int a=0; a<tamanioModificado ; a++){
			//algo
		}
	}
	pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);

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

void verificarTieneTodoLoQueQuiere(t_entrenador* entrenador){
	if(entrenador->cantidad_pokemons == list_size(entrenador->pokemonesQueQuiere)){
		if(tieneTodoLoQueQuiere(entrenador)){
			entrenador->estado = FINISHED;
			pthread_mutex_lock(&mutex_listaFinalizados);
			list_add(listaFinalizados, entrenador);
			pthread_mutex_unlock(&mutex_listaFinalizados);
		} else{
			entrenador->estado = BLOCKED;
			pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
			list_add(listaBloqueadosDeadlock, entrenador);
			pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);
		}
	}else{
		entrenador->estado = BLOCKED;
		pthread_mutex_lock(&mutex_listaBloqueadosEsperandoPokemones);
		list_add(listaBloqueadosEsperandoPokemones, entrenador);
		pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoPokemones);
	}
}



