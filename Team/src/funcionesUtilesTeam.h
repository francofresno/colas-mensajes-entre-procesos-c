/*
 * funcionesUtilesTeam.h
 *
 *  Created on: 4 may. 2020
 *      Author: utnso
 */

#ifndef FUNCIONESUTILESTEAM_H_
#define FUNCIONESUTILESTEAM_H_

#include "nuestras-commons/conexion.h"
#include "nuestras-commons/mensajes.h"
#include "team.h"

#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>

t_list* organizarPokemones(char**);

t_entrenador crear_entrenador(uint32_t, t_coordenadas*, t_list*,t_list*, uint32_t);

t_queue* ponerEntrenadoresEnCola(t_config*);

t_nombrePokemon crear_pokemon();


#endif /* FUNCIONESUTILESTEAM_H_ */
