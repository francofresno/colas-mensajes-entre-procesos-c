/*
 ============================================================================
 Name        : Logger
 Author      : Fran and Co
 Description : Logger
 ============================================================================
 */

#include "logger.h"

void log_new_subscriber(uint32_t id_subscriber, op_code queue, t_log* logger)
{
	char log_msg[100];
	sprintf(log_msg, "El proceso id: %d se ha suscripto a la cola id: %d",id_subscriber, queue);
	log_info(logger,log_msg);
}

void log_new_message(uint32_t id_message, op_code queue, t_log* logger)
{
	char log_msg[100];
	sprintf(log_msg, "El mensaje id: %d ha llegado a la cola id: %d", id_message, queue);
	log_info(logger,log_msg);
}

void log_message_to_subscriber(uint32_t id_subscriber, uint32_t id_message, t_log* logger)
{
	char log_msg[100];
	sprintf(log_msg, "El mensaje id: %d ha sido enviado al suscriptor id: %d", id_subscriber, id_message);
	log_info(logger,log_msg);
}

void log_ack_from_subscriber(uint32_t id_subscriber, uint32_t id_message, t_log* logger)
{
	char log_msg[100];
	sprintf(log_msg, "El suscriptor id: %d ha recibido el mensaje id: %d", id_subscriber, id_message);
	log_info(logger,log_msg);
}
