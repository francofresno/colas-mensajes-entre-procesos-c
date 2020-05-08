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
#include "nuestras-commons/mensajes.h"


#include<pthread.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>

// Mutex
pthread_mutex_t mutex_id_entrenadores = PTHREAD_MUTEX_INITIALIZER;

uint32_t ID_ENTRENADORES;

t_list* organizarPokemones(char**);

typedef struct
{
	uint32_t id_entrenador;
	t_coordenadas* coordenadas;
	t_list* pokemonesQuePosee;
	t_list* pokemonesQueQuiere;
	uint32_t cantidad_pokemons;
} t_entrenador;

t_entrenador* crear_entrenador(uint32_t, t_coordenadas*, t_list*, t_list*, uint32_t);

t_queue* ponerEntrenadoresEnCola(t_config*);

t_nombrePokemon* crear_pokemon(char*);

void buscarPokemones();

uint32_t generar_id();

#endif /* FUNCIONESUTILESTEAM_H_ */
