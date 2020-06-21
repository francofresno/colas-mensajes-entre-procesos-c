/*
 * planificador.h
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<string.h>
#include "nuestras-commons/conexion.h"
#include "nuestras-commons/mensajes.h"

#include "logger.h"

#include<pthread.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>

#include<semaphore.h>

char* algoritmoPlanificacion;
int quantum;
int estimacionInicial;
double alfa;
int retardoCPU;

//Listas de entrenadores segun estado
t_list* listaNuevos;
t_list* listaReady;
t_list* listaBloqueadosDeadlock;
t_list* listaBloqueadosEsperandoMensaje;
t_list* listaBloqueadosEsperandoPokemones;
t_list* listaFinalizados;


//listas de pokemones
t_list* atrapados;
t_list* pendientes;
t_list* objetivoTeam;

//semaforos
t_list* sem_entrenadores_ejecutar;
sem_t sem_planificar;
extern pthread_mutex_t mutex_atrapados;

typedef enum{
	NEW = 1,
	READY = 2,
	BLOCKED = 3,
	EXEC = 4,
	FINISHED = 5
}status_code;

typedef enum{
	FIFO = 1,
	RR = 2,
	SJFCD = 3,
	SJFSD = 4,
	ERROR_CODIGO_ALGORITMO = 9

}algoritmo_code;

const static struct {
	algoritmo_code codigo_algoritmo;
	const char* str;
} conversionAlgoritmo[] = {
		{FIFO, "FIFO"},
		{RR, "RR"},
		{SJFSD, "SJF-SD"},
		{SJFCD, "SJF-CD"}

};

typedef struct
{
	t_nombrePokemon* pokemon;
	t_coordenadas* coordenadas;
} t_newPokemon;

typedef struct
{
	uint32_t id_entrenador;
	t_coordenadas* coordenadas;
	t_list* pokemonesQuePosee;
	t_list* pokemonesQueQuiere;
	uint32_t cantidad_pokemons;
	t_newPokemon* pokemonInstantaneo;
	status_code estado;

} t_entrenador;

void planificarSegun();

void planificarSegunFifo();

algoritmo_code stringACodigoAlgoritmo(const char*);

int distanciaA(t_coordenadas*, t_coordenadas*);

int tieneTodoLoQueQuiere(t_entrenador*);

void diferenciaYCargarLista(t_list*, t_list*, t_list*);

int sonIguales(t_nombrePokemon*, t_nombrePokemon*);

void inicializarListasDeEstados();

#endif /* PLANIFICADOR_H_ */
