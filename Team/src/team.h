/*
 * team.h
 *
 *  Created on: 18 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/collections/queue.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <pthread.h>

#include "nuestras-commons/conexion.h"
#include "nuestras-commons/mensajes.h"

// IP y PUERTO de team para iniciar servidor
#define IP "127.0.0.2"
#define PUERTO "6012"

#define TEAM_LOG "team.log"
#define TEAM_NAME "team" // No estamos seguras porq cada proceso tema tiene su propio archivo de log
#define TEAM_CONFIG "team.config"

pthread_t thread;

t_log* iniciar_logger(void);
t_config* leer_config(void);
int esperar_cliente(int socket_servidor);
void serve_client(int* socket_cliente);
void process_request(int cod_op,int cliente_fd); //Que hacer cuando nos llegue un msj
void terminar_programa(int socket, t_log* logger, t_config* config);

#endif /* TEAM_H_ */
