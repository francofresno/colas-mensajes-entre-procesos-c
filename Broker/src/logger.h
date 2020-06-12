/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Header Logger
 ============================================================================
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include<commons/log.h>
#include "nuestras-commons/mensajes.h"

t_log* LOGGER;

void log_new_connection(int socket_client, t_log* logger);
void log_new_subscriber(uint32_t id_proceso, op_code cola, t_log* logger);
void log_new_message(uint32_t id, op_code cola, t_log* logger);
void log_message_to_subscriber(uint32_t id_suscriptor, uint32_t id_mensaje, t_log* logger);
void log_ack_from_subscriber(uint32_t id_suscriptor, uint32_t id_mensaje, t_log* logger);
void log_new_message_in_memory(uint32_t id_message, int partition_base, t_log* logger);
void log_deleted_partition(int partition_base, t_log* logger);
void log_compactation(t_log* logger);
void log_buddy_association(int base_buddy_1, int base_buddy_2, t_log* logger);
void log_dump(t_log* logger);

#endif /* LOGGER_H_ */
