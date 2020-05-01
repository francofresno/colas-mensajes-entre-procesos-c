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
#include<pthread.h>

#include "nuestras-commons/conexion.h"
#include "nuestras-commons/mensajes.h"
#include "messages_queues.h"

#define BROKER_LOG "broker.log"
#define BROKER_NAME "broker"
#define BROKER_CONFIG "broker.config"

uint32_t ID_COUNTER;
pthread_mutex_t mutex_id_counter = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread;

t_queue* NEW_POKEMON_QUEUE;
t_queue* APPEARED_POKEMON_QUEUE;
t_queue* CATCH_POKEMON_QUEUE;
t_queue* CAUGHT_POKEMON_QUEUE;
t_queue* GET_POKEMON_QUEUE;
t_queue* LOCALIZED_POKEMON_QUEUE;
t_list* NEW_POKEMON_SUBSCRIBERS;
t_list* APPEARED_POKEMON_SUBSCRIBERS;
t_list* CATCH_POKEMON_SUBSCRIBERS;
t_list* CAUGHT_POKEMON_SUBSCRIBERS;
t_list* GET_POKEMON_SUBSCRIBERS;
t_list* LOCALIZED_POKEMON_SUBSCRIBERS;

int init_server(t_config* config);
void init_message_queues();
void init_suscriber_lists();
t_log* iniciar_logger(void);
t_config* leer_config(void);

int esperar_cliente(int socket_servidor);
void serve_client(int* socket_cliente);
void process_request(int cod_op, uint32_t id_correlativo, void* paqueteRecibido, int socket_cliente);
void suscribir(t_suscripcion_msg* estructuraSuscripcion);
void informar_a_suscriptores(op_code codigo, void* mensaje, uint32_t id, uint32_t id_correlativo, t_list* suscriptores);

uint32_t generar_id();

void terminar_programa(int socket, t_log* logger, t_config* config);

#endif /* BROKER_H_ */
