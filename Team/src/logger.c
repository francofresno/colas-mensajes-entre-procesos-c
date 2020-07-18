/*
 * logger.c
 *
 *  Created on: 12 jun. 2020
 *      Author: utnso
 */

#include "logger.h"

void log_entrenador_cambio_de_cola_planificacion(uint32_t id_entrenador, char* razon, char* cola)
{
	char log_msg[100];
	sprintf(log_msg, "El entrenador con ID %d cambió a la cola %s porque: %s", id_entrenador, cola, razon);
	log_info(LOGGER,log_msg);
}

void log_movimiento_entrenador(uint32_t id, uint32_t coordX, uint32_t coordY)
{
	char log_msg[100];
	sprintf(log_msg, "El entrenador con ID %d se movió a la posición [%d,%d]", id, coordX, coordY);
	log_info(LOGGER,log_msg);
}

void log_atrapo_al_pokemon(uint32_t id_entrenador, char* nombrePokemon, uint32_t coordX, uint32_t coordY)
{
	char log_msg[100];
	sprintf(log_msg, "El entrenador con ID %d atrapó al Pokemon %s en la posición [%d,%d]", id_entrenador, nombrePokemon, coordX, coordY);
	log_info(LOGGER,log_msg);
}

void log_intercambio_pokemones(uint32_t id_entrenador1, uint32_t id_entrenador2)
{
	char log_msg[100];
	sprintf(log_msg, "El entrenador con ID %d realizó un intercambio con el entrenador con ID %d", id_entrenador1, id_entrenador2);
	log_info(LOGGER,log_msg);
}

void log_inicio_algoritmo_deadlock()
{
	char log_msg[100];
	sprintf(log_msg, "El algoritmo de detección de deadlock ha iniciado");
	log_info(LOGGER,log_msg);
}

void log_fin_algoritmo_deadlock(char* resultado)
{
	printf("a logear deadlock\n");
	char log_msg[100];
	sprintf(log_msg, "El resultado del algoritmo de deadlock fue: %s", resultado);
	log_info(LOGGER,log_msg);
}

void log_llegada_appeared(uint32_t id_corr, char* nombrePokemon, uint32_t x, uint32_t y)
{
	char* log_msg = "Llegó un mensaje 'Appeared' a Team y sus datos son: \n ID Correlativo: %d \n Nombre Pokemon: %s \n Coordenadas [%d,%d]";
	log_info(LOGGER,log_msg, id_corr, nombrePokemon, x, y);
}

void log_llegada_localized(uint32_t id_corr, char* nombre, uint32_t cantidadCoords, char* datos)
{
	char* log_msg = "Llegó un mensaje 'Localized' a Team y sus datos son: \n ID Correlativo: %d \n Nombre Pokemon: %s \n Cantidad de Coordenadas: %d \n Coordenadas: %s";
	log_info(LOGGER,log_msg, id_corr, nombre, cantidadCoords, datos);
}

void log_llegada_caught(uint32_t id_corr, uint32_t atrapado)
{
	char* estadoAtrapado = atrapado ? "" : "no ";
	char* log_msg = "Llegó un mensaje 'Caught' a Team y sus datos son: \n ID Correlativo: %d \n y el entrenador %spuede atraparlo";
	log_info(LOGGER,log_msg, id_corr, estadoAtrapado);
}

void log_resultado_team(char* resultado, int ciclosCPUTotales, int cantidadCambiosContexto, char* cantCiclosCPUPorEntrenador, int cantDeadlocks)
{
	printf("logeo resultado\n");
	char* log_msg = "El resultado final es: %s /n Ciclos de CPU totales: %d \n Cantidad de cambios de contexto: %d \n Cantidad de ciclos de CPU realizados por entrenador: %s \n Deadlocks producidos y resueltos: %d ";
	log_info(LOGGER,log_msg, resultado, ciclosCPUTotales, cantidadCambiosContexto, cantCiclosCPUPorEntrenador, cantDeadlocks);
}

void log_error_comunicacion_con_broker()
{
	char log_msg[100];
	sprintf(log_msg, "Hubo un error de comunicación con Broker y se realizará la operación default");
	log_info(LOGGER,log_msg);
}

void log_inicio_reintento_conexion_broker()
{
	char log_msg[100];
	sprintf(log_msg, "Inicio de proceso de reintento de conexion con Broker");
	log_info(LOGGER,log_msg);
}

void log_resultado_proceso_reintento_conexion_broker(int socket)
{
	char* mensaje = socket <= 0 ? "No se pudo reconectar con el Broker" : "Reconexión exitosa con Broker";
	char log_msg[100];
	sprintf(log_msg, "%s", mensaje);
	log_info(LOGGER,log_msg);
}
