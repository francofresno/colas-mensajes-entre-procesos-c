/*
 * logger.c
 *
 *  Created on: 12 jun. 2020
 *      Author: utnso
 */

#include "logger.h"

void log_inicio_reintento_conexion_broker()
{
	char log_msg[100];
	sprintf(log_msg, "Inicio de proceso de reintento de conexion con broker");
	log_info(LOGGER,log_msg);
}

void log_resultado_proceso_reintento_conexion_broker(int socket)
{
	char log_msg[100];
	if(socket<=0){
		sprintf(log_msg, "No se pudo reconectar con el broker");
	} else{
		sprintf(log_msg, "Reconexión exitosa con broker");
	}
	log_info(LOGGER,log_msg);
}

void log_movimiento_entrenador(uint32_t id, uint32_t coordX, uint32_t coordY)
{
	char log_msg[100];
	sprintf(log_msg, "El entrenador con ID %d se movió a la posición [%d,%d]", id, coordX, coordY);
	log_info(LOGGER,log_msg);
}

void log_intercambio_pokemones(uint32_t id_entrenador1, uint32_t id_entrenador2)
{
	char log_msg[100];
	sprintf(log_msg, "El entrenador con ID %d realizó un intercambio con el entrenador con ID %d", id_entrenador1, id_entrenador2);
	log_info(LOGGER,log_msg);
}

// void log_llegada_mensaje(int cod_op)
//{
//	char log_msg[100];
//	sprintf(log_msg, "Llegó un mensaje a Team ");
//	log_info(LOGGER,log_msg);
//}
