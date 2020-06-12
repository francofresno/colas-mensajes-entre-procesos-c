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
#include<string.h>
#include "nuestras-commons/conexion.h"



#include<pthread.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>

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

//typedef struct
//{
//	t_list* listaNuevos;
//	t_list* listaReady;
//	t_list* listaBloqueadosEsperandoPokemones;
//	t_list* listaBloqueadosDeadlock;
//	t_list* listaBloqueadosEsperandoMensaje;
//	t_list* listaFinalizados;
//
//} t_estructuraListas;

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

void ponerEntrenadoresEnLista(t_config*);

void inicializarListasDeEstados();

void crearHilosEntrenadores();

t_entrenador* crear_entrenador(uint32_t, t_coordenadas*, t_list*, t_list*, uint32_t, status_code);

t_list* organizarPokemones(char**);

uint32_t generar_id();

t_nombrePokemon* crear_pokemon(char*);

void ejecutarEntrenador(t_entrenador*);

void hacerObjetivoTeam(t_list*,t_list*);

t_list* aplanarDobleLista(t_list*);

void contiene(t_list*, t_list*);

int sonIguales(t_nombrePokemon*, t_nombrePokemon*);

t_entrenador* entrenadorMasCercano(t_newPokemon*);

int distanciaA(t_coordenadas*, t_coordenadas*);

void planificarSegun();

void planificarSegunFifo();

algoritmo_code stringACodigoAlgoritmo(const char*);



#endif /* FUNCIONESUTILESTEAM_H_ */
