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
pthread_mutex_t mutex_id_counter = PTHREAD_MUTEX_INITIALIZER;

uint32_t ID_COUNTER;

t_list* organizarPokemones(char**);

t_entrenador* crear_entrenador(uint32_t, t_coordenadas*, t_list*, t_list*, uint32_t);

t_queue* ponerEntrenadoresEnCola(t_config*);

t_nombrePokemon* crear_pokemon(char*);

void buscarPokemones();

uint32_t generar_id();

#endif /* FUNCIONESUTILESTEAM_H_ */
