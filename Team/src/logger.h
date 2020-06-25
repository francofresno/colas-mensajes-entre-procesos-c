/*
 * logger.h
 *
 *  Created on: 12 jun. 2020
 *      Author: utnso
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include<stdint.h>
#include<commons/log.h>

t_log* LOGGER;

void log_inicio_reintento_conexion_broker();
void log_resultado_proceso_reintento_conexion_broker(int);
void log_movimiento_entrenador(uint32_t, uint32_t, uint32_t);
void log_intercambio_pokemones(uint32_t, uint32_t);

#endif /* LOGGER_H_ */
