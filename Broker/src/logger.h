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

void log_nuevo_suscriptor(uint32_t id_proceso, op_code cola, t_log* logger);
void log_nuevo_mensaje(uint32_t id, op_code cola, t_log* logger);
void log_mensaje_a_suscriptor(uint32_t id_suscriptor, uint32_t id_mensaje, t_log* logger);
void log_ack_de_suscriptor(uint32_t id_suscriptor, uint32_t id_mensaje, t_log* logger);

#endif /* LOGGER_H_ */
