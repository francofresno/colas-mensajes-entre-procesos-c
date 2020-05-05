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

	typedef struct
	{
		op_code codigo_operacion;
		uint32_t id;
		uint32_t id_correlativo;
		void* mensaje;
	} t_paquete;

	// Cliente
	int crear_conexion(char *ip, char* puerto);

	// Server
	int iniciar_servidor(char *ip, char* puerto);
	int esperar_cliente(int socket_servidor);

	// Generales
	void liberar_conexion(int un_socket);
	void suscribirse_a_cola(t_suscripcion_msg* estructuraSuscripcion, int socket_servidor);

	// Enviar mensaje
	void enviar_mensaje(op_code codigoOperacion, uint32_t id, uint32_t id_correlativo, void* mensaje, int un_socket);
	void* serializar_paquete(op_code codigoOperacion, uint32_t id, uint32_t id_correlativo, void* estructura, uint32_t* bytes);
	void serializar_variable(void* a_enviar, void* a_serializar, int tamanio, int *offset);
	void serializar_nombre(void* aEnviar, t_nombrePokemon nombrePokemon, int *offset);

	// Recibir mensaje
	uint32_t recibir_cantidad_bytes(int socket_cliente);
	t_paquete* recibir_paquete(int socket_cliente);
	void copiar_nombre(t_nombrePokemon* estructuraNombre, void* stream, int* offset);
	void copiar_coordenadas(t_coordenadas* estructuraCoordenadas, void* stream, int* offset);
	void copiar_variable(void* variable, void* stream, int* offset, int size);

	// Respuestas
	void enviar_id_respuesta(uint32_t id_msg, int socket_cliente);
	uint32_t recibir_id(int socket_cliente);

#endif /* CONEXION_H_ */
