/*
 * logger.c
 *
 *  Created on: 12 jun. 2020
 *      Author: utnso
 */

#include "logger.h"

void log_reintento_conexion_broker()
{
	char log_msg[100];
	sprintf(log_msg, "Inicio de proceso de reintento de conexion con broker");
	log_info(LOGGER,log_msg);
}
