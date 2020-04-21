#include "conexion.h"


// ---- Especificas de Cliente ---- //
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
		printf("error\n");

	freeaddrinfo(server_info);

	return socket_cliente;
}

//t_paquete* recibir_paquete(int socket)
//{
//	t_paquete* paquete_recibido = malloc(sizeof(paquete_recibido));
//	paquete_recibido->buffer = malloc(sizeof(paquete_recibido->buffer));
//	paquete_recibido->buffer->stream = malloc(paquete_recibido->buffer->size);
//
//	recv(socket,&(paquete_recibido->codigo_operacion),sizeof(int),0);
//	recv(socket,&(paquete_recibido->buffer->size),sizeof(int),0);
//	recv(socket,(paquete_recibido->buffer->stream),(paquete_recibido->buffer->size),0);
//
//	return paquete_recibido;
//}
// ---- END Especificas de Cliente ---- //


// ---- Especificas de Server ---- //
int iniciar_servidor(char *ip, char* puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	return socket_cliente;
}

int recibir_codigo_operacion(int socket_cliente) {
	int cod_op;

	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;

	return cod_op;
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void* buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}
// ---- END Especificas de Server ---- //


//void* serializar_paquete(t_paquete* paquete, int *bytes)
//{
//	int offset = 0;
//	*bytes = paquete->buffer->size + sizeof(int) * 2;
//	void* a_enviar = malloc(*bytes);
//
//	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(int));
//	offset += sizeof(int);
//	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
//	offset += sizeof(int);
//	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
//
//	return a_enviar;
//}

//void enviar_mensaje(op_code codigo_op, t_buffer* buffer, int socket_servidor)
//{
//	t_paquete* paquete = malloc(sizeof(paquete));
//	paquete->codigo_operacion = codigo_op;
//	paquete->buffer = malloc(buffer->size + sizeof(buffer->size));
//
//	memcpy(paquete->buffer, buffer, buffer->size + sizeof(buffer->size));
//
//	int cant_bytes;
//	void* a_enviar = serializar_paquete(paquete, &cant_bytes);
//
//	send(socket_servidor, a_enviar, cant_bytes, 0);
//
//	free(a_enviar);
//	free(paquete->buffer->stream);
//	free(paquete->buffer);
//	free(paquete);
//
//}

void* serializar_paquete(t_paquete* paquete, int tamanio_buffer, int *bytes)
{
	int offset = 0;
	*bytes = tamanio_buffer + sizeof(int);
	void* a_enviar = malloc(*bytes);

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->stream, tamanio_buffer);

	return a_enviar;
}

void enviar_mensaje(op_code codigo_op, t_buffer* buffer, int socket_servidor) {
	t_paquete* paquete = malloc(sizeof(paquete));
	paquete->codigo_operacion = codigo_op;
	paquete->stream = malloc(buffer->size);
	
	memcpy(paquete->stream, buffer->stream, buffer->size);
	
	int cant_bytes;
	void* a_enviar = serializar_paquete(paquete, buffer->size, &cant_bytes);
	
	send(socket_servidor, a_enviar, cant_bytes, 0);

	free(a_enviar);
	free(paquete->stream);
	free(paquete);
	free(buffer->stream);
	free(buffer);
}

void liberar_conexion(int socket)
{
	close(socket);
}

