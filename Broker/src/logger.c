/*
 ============================================================================
 Name        : Logger
 Author      : Fran and Co
 Description : Logger
 ============================================================================
 */

#include "logger.h"

void log_nuevo_suscriptor(uint32_t id_proceso, op_code cola, t_log* logger)
{
	char log_msg[100];
	sprintf(log_msg, "El proceso id: %d se ha suscripto a la cola id: %d",id_proceso, cola);
	log_info(logger,log_msg);
}

void log_nuevo_mensaje(uint32_t id, op_code cola, t_log* logger)
{
	char log_msg[100];
	sprintf(log_msg, "El mensaje id %d ha llegado a la cola id: %d", id, cola);
	log_info(logger,log_msg);
}

void log_mensaje_a_suscriptor(uint32_t id_suscriptor, uint32_t id_mensaje, t_log* logger)
{
	char log_msg[100];
	sprintf(log_msg, "El mensaje id: %d ha sido enviado al suscriptor id: %d", id_suscriptor, id_mensaje);
	log_info(logger,log_msg);
}

void log_ack_de_suscriptor(uint32_t id_suscriptor, uint32_t id_mensaje, t_log* logger)
{
	char log_msg[100];
	sprintf(log_msg, "El suscriptor id: %d ha recibido el mensaje id: %d", id_suscriptor, id_mensaje);
	log_info(logger,log_msg);
}
