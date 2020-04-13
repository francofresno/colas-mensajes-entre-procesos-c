/*
 * utils.h
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

	typedef enum {
		NEW_POKEMON = 1,
		APPEARED_POKEMON = 2,
		CATCH_POKEMON = 3,
		CAUGHT_POKEMON = 4,
		GET_POKEMON = 5,
		LOCALIZED_POKEMON = 6
	} op_code;

	typedef struct
	{
		int size;
		void* stream;
	} t_buffer;

	typedef struct
	{
		op_code codigo_operacion;
		t_buffer* buffer;
	} t_paquete;

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(op_code codigo, void* mensaje, int socket_cliente);
t_paquete* recibir_paquete(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);

#endif /* UTILS_H_ */
