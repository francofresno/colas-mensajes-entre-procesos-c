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
#define IP "127.0.0.4"
#define PUERTO "6014"

t_log* iniciar_logger(void);
t_config* leer_config(void);
void terminar_programa(int socket, t_log* logger, t_config* config);

#endif /* GAMEBOY_H_ */
