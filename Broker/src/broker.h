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
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<pthread.h>

#include "nuestras-commons/conexion.h"
#include "nuestras-commons/mensajes.h"
#include "messages_queues.h"
#include "logger.h"

#define BROKER_NAME "broker"
#define BROKER_CONFIG "broker.config"

uint32_t ID_COUNTER;
pthread_t thread;

// Colas de mensajes y listas de suscriptores
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


// Mutex
pthread_mutex_t mutex_id_counter = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_new_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_appeared_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_get_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_localized_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_catch_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_caught_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_new_susc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_appeared_susc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_get_susc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_localized_susc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_catch_susc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_caught_susc = PTHREAD_MUTEX_INITIALIZER;

// Arrays
t_queue* COLAS_MENSAJES[7];
t_list* SUSCRIPTORES_MENSAJES[7];
pthread_mutex_t MUTEX_COLAS[7];
pthread_mutex_t MUTEX_SUSCRIPTORES[7];

// Loggers y config
t_log* logger;
t_config* config;


int init_server();
void init_message_queues();
void init_suscriber_lists();
void init_logger();
void init_config();

int esperar_cliente(int socket_servidor);
void serve_client(int* socket_cliente);
void process_request(int cod_op, uint32_t id_correlativo, void* paqueteRecibido, int socket_cliente);
void suscribir_a_cola(t_suscripcion_msg* estructuraSuscripcion, int socket_suscriptor);
t_list* informar_a_suscriptores(op_code codigo, void* mensaje, uint32_t id, uint32_t id_correlativo, t_list* suscriptores, pthread_mutex_t mutex);
t_enqueued_message** responder_a_suscriptor_nuevo(op_code codigo, t_queue* queue, t_subscriber* subscriber);
/*
 *  @NAME: enviar_mensajes_encolados_a_suscriptor_nuevo
 *  @RETURN: -1 en caso de falla o 0 en caso de éxito
 */
void enviar_mensajes_encolados(uint32_t cantidad_mensajes, uint32_t tamanio_stream, void** paquetes_serializados,
		int* tamanio_paquetes, t_enqueued_message** mensajes_encolados, t_subscriber* subscriber);
void remover_suscriptor_si_es_temporal(t_list* subscribers, t_subscriber* subscriber, uint32_t tiempo, pthread_mutex_t mutex);
void recibir_ack(t_enqueued_message** mensajes_encolados, int cantidad_mensajes, t_subscriber* subscriber);

uint32_t generar_id();

void terminar_programa(int socket, t_log* logger);

#endif /* BROKER_H_ */
