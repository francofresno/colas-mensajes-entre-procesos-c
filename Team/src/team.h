/*
 * team.h
 *
 *  Created on: 18 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include "funcionesUtilesTeam.h"

// IP y PUERTO de team para iniciar servidor
#define IP "127.0.0.2"
#define PUERTO "6012"

#define TEAM_LOG "team.log"
#define TEAM_NAME "team" // No estamos seguras porq cada proceso tema tiene su propio archivo de log
#define TEAM_CONFIG "team.config"

#include "nuestras-commons/conexion.h"

const static struct {
	op_code codigoOperacion;
	const char* str;
} conversionCodigoOp[] = {

		{CATCH_POKEMON, "CATCH_POKEMON"},
		{GET_POKEMON, "GET_POKEMON"},

};

pthread_t thread;

t_log* iniciar_logger(void);

t_config* leer_config(void);

void serve_client(int*);

void process_request(int, uint32_t, void*, int);

op_code stringACodigoOperacion(const char*);

void enviarMensajeGetABroker();

t_list* eliminarRepetidos();

void enviarMensajeGet(t_nombrePokemon*);

//void terminar_programa(int socket, t_log* logger, t_config* config);

#endif /* TEAM_H_ */
