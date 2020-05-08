/*
 * team.h
 *
 *  Created on: 18 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include "funcionesUtilesTeam.h"

// IP y PUERTO de team para iniciar servidor
#define IP "127.0.0.2"
#define PUERTO "6012"

#define TEAM_LOG "team.log"
#define TEAM_NAME "team" // No estamos seguras porq cada proceso tema tiene su propio archivo de log
#define TEAM_CONFIG "team.config"

pthread_t thread;

typedef struct
{
	uint32_t id_entrenador;
	t_coordenadas* coordenadas;
	t_list* pokemonesQuePosee;
	t_list* pokemonesQueQuiere;
	uint32_t cantidad_pokemons;
} t_entrenador;

t_log* iniciar_logger(void);
t_config* leer_config(void);
int esperar_cliente(int socket_servidor);
void serve_client(int* socket_cliente);
void process_request(int cod_op,int cliente_fd); //Que hacer cuando nos llegue un msj
void terminar_programa(int socket, t_log* logger, t_config* config);

#endif /* TEAM_H_ */
