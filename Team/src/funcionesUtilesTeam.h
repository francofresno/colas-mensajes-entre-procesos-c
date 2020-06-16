/*
 * funcionesUtilesTeam.h
 *
 *  Created on: 4 may. 2020
 *      Author: utnso
 */

#ifndef FUNCIONESUTILESTEAM_H_
#define FUNCIONESUTILESTEAM_H_

#include "planificador.h"

uint32_t ID_ENTRENADORES;

t_list* organizarPokemones(char**);

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

void ponerEntrenadoresEnLista(t_config*);

void crearHilosEntrenadores();

t_entrenador* crear_entrenador(uint32_t, t_coordenadas*, t_list*, t_list*, uint32_t, status_code);

t_list* organizarPokemones(char**);

t_nombrePokemon* crear_pokemon(char*);

uint32_t generar_id();

void ejecutarEntrenador(t_entrenador*);

int llegoAlObjetivo(t_entrenador*);

void moverAlEntrenador(uint32_t);

void evaluarEstadoPrevioAAtrapar(t_entrenador*);

void atraparPokemon(t_entrenador*);

void hacerObjetivoTeam(t_list*,t_list*);

t_list* aplanarDobleLista(t_list*);

void contiene(t_list*, t_list*);

int sonIguales(t_nombrePokemon*, t_nombrePokemon*);

t_entrenador* entrenadorMasCercano(t_newPokemon*);

void buscarPokemon(t_newPokemon*);



#endif /* FUNCIONESUTILESTEAM_H_ */
