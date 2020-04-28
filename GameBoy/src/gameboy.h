/*
 ============================================================================
 Name        : GameBoy
 Author      : Fran and Co
 Description : Header Proceso GameBoy
 ============================================================================
 */

#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include "nuestras-commons/conexion.h"
#include "nuestras-commons/mensajes.h"

#define GAMEBOY_LOG "gameboy.log"
#define GAMEBOY_NAME "gameboy"
#define GAMEBOY_CONFIG "gameboy.config"

const static struct {
	op_code codigoOperacion;
	const char* str;
} conversionCodigoOp[] = {
		{NEW_POKEMON, "NEW_POKEMON"},
		{APPEARED_POKEMON, "APPEARED_POKEMON"},
		{CATCH_POKEMON, "CATCH_POKEMON"},
		{GET_POKEMON, "GET_POKEMON"},
		{LOCALIZED_POKEMON, "LOCALIZED_POKEMON"},
		{CAUGHT_POKEMON, "CAUGHT_POKEMON"}
};

typedef enum {
			BROKER = 1,
			TEAM = 2,
			GAMECARD = 3,
			ERROR_PROCESO = 9
} process_code;

const static struct {
	process_code codigoProceso;
	const char* str;
} conversionCodigoProceso[] = {
		{BROKER, "BROKER"},
		{TEAM, "TEAM"},
		{GAMECARD, "GAMECARD"}
};

t_log* iniciar_logger(void);
t_config* leer_config(void);
void terminar_programa(int socket, t_log* logger, t_config* config);
op_code stringACodigoOperacion(const char* string);
process_code stringACodigoProceso(const char* string);

#endif /* GAMEBOY_H_ */
