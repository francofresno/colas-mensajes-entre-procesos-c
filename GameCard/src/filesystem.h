/*
 * filesystem.h
 *
 *  Created on: 17 jun. 2020
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <pthread.h>
// INCLUDES FILESYSTEM
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
// INCLUDES LOCALES
#include "nuestras-commons/conexion.h"
#include "nuestras-commons/mensajes.h"

char* PUNTO_MONTAJE;

void verificarMetadata(char*);
char* obtenerRutaTotal(char*);
void configuracionInicial(void);
void verificarDirectorio(char*);
void verificarBitmap(char* pathActual, t_config* config);
void verificarBloques(char* pathDirectorio);
char* verificarPokemon(t_nombrePokemon nombrePokemon);
void testearConfig(t_config* config);


#endif /* FILESYSTEM_H_ */
