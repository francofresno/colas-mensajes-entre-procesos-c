/*
 * funcionesUtilesTeam.h
 *
 *  Created on: 4 may. 2020
 *      Author: utnso
 */

#ifndef FUNCIONESUTILESTEAM_H_
#define FUNCIONESUTILESTEAM_H_

#include "planificador.h"

//Variables Globales
uint32_t ID_ENTRENADORES;

// Listas
t_list* entrenadores;
t_list* hilosEntrenadores;
t_list* objetivoTeam;
t_list* atrapados;
t_list* pendientes;
t_list* id_mensajeGet;
t_list* id_mensajeCatch;

// Mutexs
extern pthread_mutex_t mutex_id_entrenadores;
extern pthread_mutex_t mutex_entrenador;
extern pthread_mutex_t mutex_hay_pokemones;

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

t_list* organizarPokemones(char**);

void ponerEntrenadoresEnLista(t_config*);

void crearHilosEntrenadores();

t_entrenador* crear_entrenador(uint32_t, t_coordenadas*, t_list*, t_list*, uint32_t, status_code);

t_list* organizarPokemones(char**);

t_nombrePokemon* crear_pokemon(char*);

uint32_t generar_id();

void ejecutarEntrenador(t_entrenador*);

int llegoAlObjetivo(t_entrenador*);

void moverAlEntrenadorHastaUnPokemon(uint32_t);

void evaluarEstadoPrevioAAtrapar(t_entrenador*);

void atraparPokemon(t_entrenador*);

void hacerObjetivoTeam(t_list*,t_list*);

t_list* aplanarDobleLista(t_list*);

t_entrenador* entrenadorMasCercano(t_newPokemon*);

void buscarPokemon(t_newPokemon*);

void moverAlEntrenadorHastaOtroEntrenador(uint32_t, uint32_t);

void intercambiarPokemones(uint32_t, uint32_t);

void dameTuPokemon(t_entrenador*, t_entrenador*);

t_entrenador* elegirConQuienIntercambiar(t_entrenador*);

int tengoAlgunPokemonQueQuiere2(t_entrenador*,t_entrenador*);

#endif /* FUNCIONESUTILESTEAM_H_ */
