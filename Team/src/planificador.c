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
pthread_mutex_t mutex_entrenadores = PTHREAD_MUTEX_INITIALIZER;

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

void planificarSegunFifo() {

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

			if((entrenador->pokemonInstantaneo)==NULL){
				pthread_mutex_lock(&mutex_listaBloqueadosEsperandoPokemones);
				list_add(listaBloqueadosEsperandoPokemones, entrenador);
				pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoPokemones);
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

		if(!(entrenador->puedeAtrapar)){

			do{
				distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);
				sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
				sem_post(semaforoDelEntrenador);//TODO hacer impide que otro entrenador ejecute a la par

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
			verificarTieneTodoLoQueQuiere(entrenador);
		}
		sem_post(&sem_planificar);
	}

	pthread_mutex_unlock(&mutex_listaReady);

	pthread_mutex_lock(&mutex_objetivoTeam);
	int tamanioObjetivoTeam = list_size(objetivoTeam);
	pthread_mutex_unlock(&mutex_objetivoTeam);

	pthread_mutex_lock(&mutex_atrapados);
	int tamanioAtrapados = list_size(atrapados);
	pthread_mutex_unlock(&mutex_atrapados);

	if(tamanioObjetivoTeam == tamanioAtrapados){ //o cumplio el objetivo o hay deadlock

		pthread_mutex_lock(&mutex_entrenadores);
		int tamanioEntrenadores = list_size(entrenadores);
		pthread_mutex_unlock(&mutex_entrenadores);

		pthread_mutex_lock(&mutex_listaFinalizados);
		int tamanioFinalizados = list_size(listaFinalizados);
		pthread_mutex_unlock(&mutex_listaFinalizados);

		if(tamanioEntrenadores == tamanioFinalizados){
			printf("El Team cumplio el obj\n"); // TODO ??
		} else{

			pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
			int tamanioDeadlock = list_size(listaBloqueadosDeadlock);
			for (int b = 0; b < tamanioDeadlock; b++) {

				t_entrenador* entrenador = (t_entrenador*) list_remove(listaBloqueadosDeadlock, 0);
				entrenador->estado = EXEC;

				t_entrenador* entrenadorConQuienIntercambiar = elegirConQuienIntercambiar(entrenador);

				pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
				sacarEntrenadorDeLista(entrenadorConQuienIntercambiar, listaBloqueadosDeadlock);
				pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);

				do{
					distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
					sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
					sem_post(semaforoDelEntrenador);//TODO hacer impide que otro entrenador ejecute a la par

				}while(distancia !=0);

				verificarTieneTodoLoQueQuiere(entrenador);
				verificarTieneTodoLoQueQuiere(entrenadorConQuienIntercambiar);

			}

			pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);
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

void sacarEntrenadorDeLista(t_entrenador* entrenador, t_list* lista){
	int a = list_size(lista);
	for(int i=0; i<a ; i++){
		t_entrenador* entrenadorDeLista = list_get(lista, i);
		if(entrenador->id_entrenador == entrenadorDeLista->id_entrenador){
			list_remove(lista, i);
		}
	}
}

t_entrenador* elegirConQuienIntercambiar(t_entrenador* entrenador){ //TODO probar

	t_list* listaQuiere1 = list_duplicate(entrenador->pokemonesQueQuiere);
	t_list* listaPosee1 = list_duplicate(entrenador->pokemonesQuePosee);

	t_list* leFaltanParaObj1 = list_create();

	diferenciaYCargarLista(listaQuiere1, listaPosee1, leFaltanParaObj1);

	list_destroy(listaQuiere1);
	list_destroy(listaPosee1);

	t_list* tienePeroNoQuiere2 = list_create();
	t_list* pokemonesDe2QueQuiere1 = list_create();

	t_list* sublistasPosiblesProveedoresDePokemon = list_create();

	pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
	int tamanioDeadlock = list_size(listaBloqueadosDeadlock);

	for(int a=0; a< tamanioDeadlock; a++){

		t_entrenador* entrenador2 = list_get(listaBloqueadosDeadlock, a);

		t_list* listaQuiere2 = list_duplicate(entrenador2->pokemonesQueQuiere);
		t_list* listaPosee2 = list_duplicate(entrenador2->pokemonesQuePosee);

		diferenciaYCargarLista(listaPosee2, listaQuiere2, tienePeroNoQuiere2);

		list_destroy(listaQuiere2);
		list_destroy(listaPosee2);

		diferenciaYCargarLista(leFaltanParaObj1, tienePeroNoQuiere2, pokemonesDe2QueQuiere1);

		list_destroy(leFaltanParaObj1);
		list_destroy(tienePeroNoQuiere2);


		if(!list_is_empty(pokemonesDe2QueQuiere1)){
			if(tengoAlgunPokemonQueQuiere2(entrenador, entrenador2)){
				list_destroy(pokemonesDe2QueQuiere1);
				return entrenador2;
			}
			list_add(sublistasPosiblesProveedoresDePokemon, entrenador2);
		}

		list_destroy(pokemonesDe2QueQuiere1);
	}

	pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);

	t_entrenador* entrenadorProveedor = list_get(sublistasPosiblesProveedoresDePokemon, 0);
	list_destroy(sublistasPosiblesProveedoresDePokemon);
	return entrenadorProveedor;
}

int tengoAlgunPokemonQueQuiere2(t_entrenador* entrenador1,t_entrenador* entrenador2){

	t_list* listaQuiere1 = list_duplicate(entrenador1->pokemonesQueQuiere);
	t_list* listaPosee1 = list_duplicate(entrenador1->pokemonesQuePosee);

	t_list* tienePeroNoQuiere1 = list_create();

	diferenciaYCargarLista(listaPosee1, listaQuiere1, tienePeroNoQuiere1);

	t_list* listaQuiere2 = list_duplicate(entrenador2->pokemonesQueQuiere);
	t_list* listaPosee2 = list_duplicate(entrenador2->pokemonesQuePosee);

	t_list* leFaltanParaObj2 = list_create();

	diferenciaYCargarLista(listaQuiere2, listaPosee2, leFaltanParaObj2);

	int tamanioFaltaParaObj2 = list_size(leFaltanParaObj2);
	int tamanioTienePeroNoQuiere1 = list_size(tienePeroNoQuiere1);

	list_destroy(listaQuiere1);
	list_destroy(listaPosee1);
	list_destroy(listaQuiere2);
	list_destroy(listaPosee2);

	for(int a=0; a< tamanioFaltaParaObj2; a++){

		for(int b=0; b<tamanioTienePeroNoQuiere1; b++){

			if(sonIguales(list_get(leFaltanParaObj2, a), list_get(tienePeroNoQuiere1, b))){
				list_destroy(leFaltanParaObj2);
				list_destroy(tienePeroNoQuiere1);
				return true;
			}
		}

	}

	list_destroy(leFaltanParaObj2);
	list_destroy(tienePeroNoQuiere1);

	return false;
}

