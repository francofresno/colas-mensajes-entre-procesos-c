#ifndef CONEXION_H_
#define CONEXION_H_

	#include<stdio.h>
	#include<stdlib.h>
	#include<signal.h>
	#include<unistd.h>
	#include<sys/socket.h>
	#include<netdb.h>
	#include<string.h>

	#include "mensajes.h"

	// ---- Códigos de operación ---- //
	typedef enum {
		NEW_POKEMON = 1,
		APPEARED_POKEMON = 2,
		CATCH_POKEMON = 3,
		CAUGHT_POKEMON = 4,
		GET_POKEMON = 5,
		LOCALIZED_POKEMON = 6
	} op_code;
	// ---- END Códigos de operación ---- //

	typedef struct
	{
		int size;
		void* stream;
	} t_buffer;

	typedef struct
	{
		op_code codigo_operacion;
		//t_buffer* buffer;
		void* stream;
	} t_paquete;

	// Cliente
	int crear_conexion(char *ip, char* puerto);

	// Server
	int iniciar_servidor(char *ip, char* puerto);
	int esperar_cliente(int socket_servidor);
	int recibir_codigo_operacion(int socket_cliente);
	void* recibir_mensaje(int socket_cliente, int* size);

	// Generales
	void liberar_conexion(int socket);
	void enviar_mensaje(op_code codigoOperacion, void* estructura, int socket);
	void* serializar_paquete(op_code codigoOperacion, void* estructura, int* bytes);
	void serializar_variable(void* a_enviar, void* a_serializar, int tamanio, int *offset);
	void serializar_nombre(void* aEnviar, t_nombrePokemon nombrePokemon, int *offset);
	void* recibir_paquete(op_code codigoOperacion, int socket);
	void recibir_variable(int socket, void* a_serializar, int tamanio);
	void recibir_nombre(int socket, t_nombrePokemon* estructuraNombre);
	void recibir_coordenadas(int socket, t_coordenadas* estructuraCoordenadas);



#endif /* CONEXION_H_ */
