/*
 * funcionesUtilesTeam.h
 *
 *  Created on: 4 may. 2020
 *      Author: utnso
 */

#ifndef FUNCIONESUTILESTEAM_H_
#define FUNCIONESUTILESTEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include "nuestras-commons/conexion.h"



#include<pthread.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>

// Mutex
pthread_mutex_t mutex_id_entrenadores = PTHREAD_MUTEX_INITIALIZER;

uint32_t ID_ENTRENADORES;

t_list* organizarPokemones(char**);


typedef enum{
	NEW = 1,
	READY = 2,
	BLOCKED = 3,
	EXEC = 4,
	FINISHED = 5
}status_code;

typedef struct
{
	uint32_t id_entrenador;
	t_coordenadas* coordenadas;
	t_list* pokemonesQuePosee;
	t_list* pokemonesQueQuiere;
	uint32_t cantidad_pokemons;
	status_code estado;
} t_entrenador;

typedef struct
{
	t_queue* colaListos;
	t_queue* colaBloqueados;
	t_queue* colaEnEjecucion;
	t_queue* colaFinalizados;

} t_estructuraCola;

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

t_entrenador* crear_entrenador(uint32_t, t_coordenadas*, t_list*, t_list*, uint32_t, status_code);

t_queue* ponerEntrenadoresEnCola(t_config*);

t_nombrePokemon* crear_pokemon(char*);

void buscarPokemones();

uint32_t generar_id();

algoritmo_code stringACodigoAlgoritmo(const char* string);

void planificarSegunFifo(t_list* listaDeEntrenadores);

#endif /* FUNCIONESUTILESTEAM_H_ */
