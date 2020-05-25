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
int esperar_cliente(int socket_servidor);
void serve_client(int* socket_cliente);
void process_request(int cod_op, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente); //Que hacer cuando nos llegue un msj
void terminar_programa(int socket, t_log* logger, t_config* config);

#endif /* TEAM_H_ */
