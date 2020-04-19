/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Header Proceso Broker
 ============================================================================
 */

#ifndef BROKER_H_
#define BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <pthread.h>

#include "nuestras-commons/conexion.h"
#include "nuestras-commons/mensajes.h"

#define BROKER_LOG "broker.log"
#define BROKER_NAME "broker"
#define BROKER_CONFIG "broker.config"
#define IP "127.0.0.1"
#define PUERTO "6011"

pthread_t thread;

t_log* iniciar_logger(void);
t_config* leer_config(void);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket_cliente);
void process_request(int cod_op,int cliente_fd);
void terminar_programa(int socket, t_log* logger, t_config* config);

#endif /* BROKER_H_ */
