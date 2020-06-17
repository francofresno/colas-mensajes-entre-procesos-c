/*
 * planificador.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#include "planificador.h"


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

	for (int b = 0; b < tamanio; b++) {

		t_entrenador* entrenador = (t_entrenador*) list_get(listaReady, b);
		entrenador->estado=EXEC;

		sem_wait(&sem_planificar); //inicializa en?
		do{
			distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);
			sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
			sem_post(semaforoDelEntrenador);//TODO hacer impide que otro entrenador ejecute a la par
		}while(distancia !=0);
		sem_post(&sem_planificar);

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

int distanciaA(t_coordenadas* desde, t_coordenadas* hasta){

	int distanciaX = abs(desde->posX - hasta->posX);
	int distanciaY = abs(desde->posY - hasta->posY);

	return distanciaX + distanciaY;
}

void inicializarListasDeEstados(){

listaNuevos = list_create();
listaReady = list_create();
listaBloqueadosDeadlock= list_create();
listaBloqueadosEsperandoMensaje= list_create();
listaBloqueadosEsperandoPokemones = list_create();
listaFinalizados = list_create();

}

