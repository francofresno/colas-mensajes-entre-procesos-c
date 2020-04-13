#include "utils.h"

void* serializar_paquete(t_paquete* paquete, int *bytes)
{
	int offset = 0;
	*bytes = paquete->buffer->size + sizeof(int) * 2;
	void* a_enviar = malloc(*bytes);

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	return a_enviar;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(op_code codigo, void* mensaje, int socket_cliente)
{
	t_buffer* buffer=malloc(sizeof(t_buffer));
			buffer->size = sizeof(mensaje);
			buffer->stream = malloc(buffer->size);
			memcpy(buffer->stream, mensaje, buffer->size);

			t_paquete* paquete = malloc(sizeof(paquete));
			paquete->codigo_operacion = codigo;
			paquete->buffer = malloc(buffer->size + sizeof(buffer->size));

			memcpy(paquete->buffer, buffer, buffer->size + sizeof(buffer->size));

			int cant_bytes;
			void* a_enviar = serializar_paquete(paquete,&cant_bytes);

			send(socket_cliente,a_enviar,cant_bytes,0);

			free(a_enviar);
			free(paquete->buffer->stream);
			free(paquete->buffer);
			free(paquete);

			return;
}

t_paquete* recibir_paquete(int socket_cliente)
{
	t_paquete* paquete_recibido = malloc(sizeof(paquete_recibido));
	recv(socket_cliente,&(paquete_recibido->codigo_operacion),sizeof(int),0);
	paquete_recibido->buffer=malloc(sizeof(paquete_recibido->buffer));
	recv(socket_cliente,&(paquete_recibido->buffer->size),sizeof(int),0);
	paquete_recibido->buffer->stream = malloc(paquete_recibido->buffer->size);
	recv(socket_cliente,(paquete_recibido->buffer->stream),(paquete_recibido->buffer->size),0);

	return paquete_recibido;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
