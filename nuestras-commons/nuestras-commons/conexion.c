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

//void* serializar_paquete(t_paquete* paquete, int tamanio_buffer, int *bytes)
//{
//	int offset = 0;
//	*bytes = tamanio_buffer + sizeof(int);
//	void* a_enviar = malloc(*bytes);
//
//	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(int));
//	offset += sizeof(int);
//	memcpy(a_enviar + offset, paquete->stream, tamanio_buffer);
//
//	return a_enviar;
//}
//
//void enviar_mensaje(op_code codigo_op, t_buffer* buffer, int socket_servidor) {
//	t_paquete* paquete = malloc(sizeof(paquete));
//	paquete->codigo_operacion = codigo_op;
//	paquete->stream = malloc(buffer->size);
//
//	memcpy(paquete->stream, buffer->stream, buffer->size);
//
//	int cant_bytes;
//	void* a_enviar = serializar_paquete(paquete, buffer->size, &cant_bytes);
//
//	send(socket_servidor, a_enviar, cant_bytes, 0);
//
//	free(a_enviar);
//	free(paquete->stream);
//	free(paquete);
//	free(buffer->stream);
//	free(buffer);
//}

void liberar_conexion(int socket)
{
	close(socket);
}

void enviar_mensaje(op_code codigoOperacion, void* new_pokemon_msg, int socket)
{
	int bytes;
	void* paqueteAEnviar = serializar_paquete(codigoOperacion, new_pokemon_msg, &bytes);
	send(socket, paqueteAEnviar, bytes, 0);

	//free(paqueteAEnviar);
}

void* serializar_paquete(op_code codigoOperacion, void* estructura, int* bytes)
{
	int offset = 0;
	*bytes = sizeof(uint32_t);
	void* aEnviar;
	switch(codigoOperacion)
	{
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = estructura;
			printf("Nombre length: %d\nNombre: %s\nPosX: %d\nPosY: %d\nCantidad: %d\n", estructuraNew->nombre_pokemon.nombre_lenght, estructuraNew->nombre_pokemon.nombre, estructuraNew->coordenadas.posX, estructuraNew->coordenadas.posY, estructuraNew->cantidad_pokemons);
			*bytes += sizeof(uint32_t)*5 + sizeof(estructuraNew->nombre_pokemon.nombre);
			aEnviar = malloc(sizeof(*bytes));
			offset = serializar_entero(aEnviar, codigoOperacion, offset);
			offset = serializar_nombre(aEnviar, estructuraNew->nombre_pokemon, offset);
			offset = serializar_entero(aEnviar, estructuraNew->coordenadas.posX, offset);
			offset = serializar_entero(aEnviar, estructuraNew->coordenadas.posY, offset);
			offset = serializar_entero(aEnviar, estructuraNew->cantidad_pokemons, offset);
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estructuraAppeared = estructura;
			*bytes += sizeof(uint32_t)*3 + sizeof(&(estructuraAppeared->nombre_pokemon.nombre));
			aEnviar = malloc(sizeof(*bytes));
			offset = serializar_entero(aEnviar, codigoOperacion, offset);
			offset = serializar_nombre(aEnviar, estructuraAppeared->nombre_pokemon, offset);
			offset = serializar_entero(aEnviar, estructuraAppeared->coordenadas.posX, offset);
			offset = serializar_entero(aEnviar, estructuraAppeared->coordenadas.posY, offset);
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estructuraGet = estructura;
			*bytes += sizeof(uint32_t) + sizeof(&(estructuraGet->nombre_pokemon.nombre));
			aEnviar = malloc(sizeof(*bytes));
			offset = serializar_entero(aEnviar, codigoOperacion, offset);
			offset = serializar_nombre(aEnviar, estructuraGet->nombre_pokemon, offset);
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = estructura;
			*bytes += sizeof(uint32_t)*(2+2*estructuraLocalized->cantidad_coordenadas) + sizeof(&(estructuraLocalized->nombre_pokemon.nombre));
			aEnviar = malloc(sizeof(*bytes));
			offset = serializar_entero(aEnviar, codigoOperacion, offset);
			offset = serializar_nombre(aEnviar, estructuraLocalized->nombre_pokemon, offset);
			offset = serializar_entero(aEnviar, estructuraLocalized->cantidad_coordenadas, offset);
			for(int i = 0; i < estructuraLocalized->cantidad_coordenadas; i++)
			{
				offset = serializar_entero(aEnviar, estructuraLocalized->coordenadas[i].posX, offset);
				offset = serializar_entero(aEnviar, estructuraLocalized->coordenadas[i].posY, offset);
			}
			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estructuraCatch = estructura;
			*bytes += sizeof(uint32_t)*2 + sizeof(&(estructuraCatch->nombre_pokemon.nombre));
			aEnviar = malloc(sizeof(*bytes));
			offset = serializar_entero(aEnviar, codigoOperacion, offset);
			offset = serializar_nombre(aEnviar, estructuraCatch->nombre_pokemon, offset);
			offset = serializar_entero(aEnviar, estructuraCatch->coordenadas.posX, offset);
			offset = serializar_entero(aEnviar, estructuraCatch->coordenadas.posY, offset);
			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estructuraCaught = estructura;
			*bytes += sizeof(uint32_t);
			aEnviar = malloc(sizeof(*bytes));
			offset = serializar_entero(aEnviar, codigoOperacion, offset);
			offset = serializar_entero(aEnviar, estructuraCaught->atrapado, offset);
			break;
		default: printf("\n[!] Error en el codigo de operacion al serializar paquete.\n"); break;
	}
	return aEnviar;
}

int serializar_entero(void* aEnviar, int entero, int offset)
{
	memcpy(aEnviar + offset, &(entero), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	return offset;
}

int serializar_nombre(void* aEnviar, t_nombrePokemon nombrePokemon, int offset)
{
	offset = serializar_entero(aEnviar, nombrePokemon.nombre_lenght, offset);
	memcpy(aEnviar + offset, nombrePokemon.nombre, nombrePokemon.nombre_lenght);
	offset += nombrePokemon.nombre_lenght;
	return offset;
}

void* recibir_paquete(op_code codigoOperacion, int socket)
{
	void* paqueteRecibido;
	switch(codigoOperacion)
	{
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = malloc(sizeof(estructuraNew));
			estructuraNew->nombre_pokemon.nombre = malloc(sizeof(estructuraNew->nombre_pokemon.nombre));

			recibir_nombre(socket, &(estructuraNew->nombre_pokemon));
			recibir_coordenadas(socket, &(estructuraNew->coordenadas));
			recv(socket, &(estructuraNew->cantidad_pokemons), sizeof(int), MSG_WAITALL);
			paqueteRecibido = estructuraNew;
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estructuraAppeared = malloc(sizeof(estructuraAppeared));
			estructuraAppeared->nombre_pokemon.nombre = malloc(sizeof(estructuraAppeared->nombre_pokemon.nombre));

			recibir_nombre(socket, &(estructuraAppeared->nombre_pokemon));
			recibir_coordenadas(socket, &(estructuraAppeared->coordenadas));
			paqueteRecibido = estructuraAppeared;
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estructuraGet = malloc(sizeof(estructuraGet));
			estructuraGet->nombre_pokemon.nombre = malloc(sizeof(estructuraGet->nombre_pokemon.nombre));

			recibir_nombre(socket, &(estructuraGet->nombre_pokemon));
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = malloc(sizeof(estructuraLocalized));
			estructuraLocalized->nombre_pokemon.nombre = malloc(sizeof(estructuraLocalized->nombre_pokemon.nombre));

			recibir_nombre(socket, &(estructuraLocalized->nombre_pokemon));
			recv(socket, &(estructuraLocalized->cantidad_coordenadas), sizeof(int), MSG_WAITALL);
			for(int i = 0; i < estructuraLocalized->cantidad_coordenadas; i++)
			{
				recibir_coordenadas(socket, &(estructuraLocalized->coordenadas[i]));
			}
			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estructuraCatch = malloc(sizeof(estructuraCatch));
			estructuraCatch->nombre_pokemon.nombre = malloc(sizeof(estructuraCatch->nombre_pokemon.nombre));

			recibir_nombre(socket, &(estructuraCatch->nombre_pokemon));
			recibir_coordenadas(socket, &(estructuraCatch->coordenadas));
			paqueteRecibido = estructuraCatch;
			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estructuraCaught = malloc(sizeof(estructuraCaught));

			recv(socket, &(estructuraCaught->atrapado), sizeof(int), MSG_WAITALL);
			break;
	}
	return paqueteRecibido;
}

void recibir_nombre(int socket, t_nombrePokemon* estructuraNombre)
{
	recv(socket, &(estructuraNombre->nombre_lenght), sizeof(int), MSG_WAITALL);
	estructuraNombre->nombre = malloc(estructuraNombre->nombre_lenght);
	recv(socket, estructuraNombre->nombre, estructuraNombre->nombre_lenght, MSG_WAITALL);
}

void recibir_coordenadas(int socket, t_coordenadas* estructuraCoordenadas)
{
	recv(socket, &(estructuraCoordenadas->posX), sizeof(int), MSG_WAITALL);
	recv(socket, &(estructuraCoordenadas->posY), sizeof(int), MSG_WAITALL);
}




