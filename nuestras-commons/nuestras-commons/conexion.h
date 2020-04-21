#ifndef CONEXION_H_
#define CONEXION_H_

	#include<stdio.h>
	#include<stdlib.h>
	#include<signal.h>
	#include<unistd.h>
	#include<sys/socket.h>
	#include<netdb.h>
	#include<string.h>

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
	t_paquete* recibir_paquete(int socket);

	// Server
	int iniciar_servidor(char *ip, char* puerto);
	int esperar_cliente(int socket_servidor);
	int recibir_codigo_operacion(int socket_cliente);
	void* recibir_mensaje(int socket_cliente, int* size);

	// Generales
	void* serializar_paquete(t_paquete* paquete, int tamanio_buffer, int *bytes);
	void enviar_mensaje(op_code codigo_op, t_buffer* buffer, int socket_servidor);
	void liberar_conexion(int socket);


#endif /* CONEXION_H_ */
