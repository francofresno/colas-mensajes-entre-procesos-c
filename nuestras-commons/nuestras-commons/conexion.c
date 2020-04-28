#include "conexion.h"


///////////////////////
// ---- Cliente ---- //
///////////////////////

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


//////////////////////
// ---- Server ---- //
//////////////////////

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

//////////////////////////////////
// ---- Envíos de mensajes ---- //
//////////////////////////////////

void enviar_mensaje(op_code codigoOperacion, uint32_t id, uint32_t id_correlativo, void* new_pokemon_msg, int socket)
{
	int bytes;
	void* paqueteAEnviar = serializar_paquete(codigoOperacion, id, id_correlativo, new_pokemon_msg, &bytes);

	send(socket, paqueteAEnviar, bytes, 0);
}

void* serializar_paquete(op_code codigo_operacion, uint32_t id, uint32_t id_correlativo, void* estructura, int* bytes)
{
	int offset = 0;
	*bytes = sizeof(uint32_t);
	void* a_enviar;

	switch(codigo_operacion)
	{
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = estructura;
			*bytes += sizeof(uint32_t)*6 + estructuraNew->nombre_pokemon.nombre_lenght + 1;
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, &codigo_operacion, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, bytes, sizeof(uint32_t), &offset);
			serializar_nombre(a_enviar, estructuraNew->nombre_pokemon, &offset);
			serializar_variable(a_enviar, &(estructuraNew->coordenadas.posX), sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &(estructuraNew->coordenadas.posY), sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &(estructuraNew->cantidad_pokemons), sizeof(uint32_t), &offset);
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estructuraAppeared = estructura;
			*bytes += sizeof(uint32_t)*6 + estructuraAppeared->nombre_pokemon.nombre_lenght;
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, &codigo_operacion, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, bytes, sizeof(int), &offset);
			serializar_nombre(a_enviar, estructuraAppeared->nombre_pokemon, &offset);
			serializar_variable(a_enviar, &(estructuraAppeared->coordenadas.posX), sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &(estructuraAppeared->coordenadas.posY), sizeof(uint32_t), &offset);
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estructuraGet = estructura;
			*bytes += sizeof(uint32_t)*4 + estructuraGet->nombre_pokemon.nombre_lenght;
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, &codigo_operacion, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, bytes, sizeof(int), &offset);
			serializar_nombre(a_enviar, estructuraGet->nombre_pokemon, &offset);
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = estructura;
			*bytes += 2*sizeof(uint32_t)*(estructuraLocalized->cantidad_coordenadas) + sizeof(uint32_t)*5 + estructuraLocalized->nombre_pokemon.nombre_lenght;
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, &codigo_operacion, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, bytes, sizeof(int), &offset);
			serializar_nombre(a_enviar, estructuraLocalized->nombre_pokemon, &offset);

			serializar_variable(a_enviar, &(estructuraLocalized->cantidad_coordenadas), sizeof(uint32_t), &offset);
			for(int i = 0; i < estructuraLocalized->cantidad_coordenadas; i++)
			{
				serializar_variable(a_enviar, &(estructuraLocalized->coordenadas[i].posX), sizeof(uint32_t), &offset);
				serializar_variable(a_enviar, &(estructuraLocalized->coordenadas[i].posY), sizeof(uint32_t), &offset);
			}

			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estructuraCatch = estructura;
			*bytes += sizeof(uint32_t)*6 + estructuraCatch->nombre_pokemon.nombre_lenght;
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, &codigo_operacion, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, bytes, sizeof(int), &offset);
			serializar_nombre(a_enviar, estructuraCatch->nombre_pokemon, &offset);
			serializar_variable(a_enviar, &(estructuraCatch->coordenadas.posX), sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &(estructuraCatch->coordenadas.posY), sizeof(uint32_t), &offset);
			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estructuraCaught = estructura;
			*bytes += sizeof(uint32_t)*4;
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, &codigo_operacion, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, bytes, sizeof(int), &offset);
			serializar_variable(a_enviar, &(estructuraCaught->atrapado), sizeof(uint32_t), &offset);
			break;
		default: printf("\n[!] Error en el codigo de operacion al serializar paquete.\n"); break; //TODO esto tiene que ir a un log
	}

	return a_enviar;
}

void serializar_variable(void* a_enviar, void* a_serializar, int tamanio, int *offset)
{
	memcpy(a_enviar + *offset, a_serializar, tamanio);
	*offset += tamanio;
}

void serializar_nombre(void* aEnviar, t_nombrePokemon nombrePokemon, int *offset)
{
	serializar_variable(aEnviar, &(nombrePokemon.nombre_lenght), sizeof(uint32_t), offset);
	serializar_variable(aEnviar, nombrePokemon.nombre, nombrePokemon.nombre_lenght, offset);
}


/////////////////////////////////////
// ---- Recepción de mensajes ---- //
/////////////////////////////////////

void* recibir_paquete(op_code codigoOperacion, int socket)
{
	void* mensajeRecibido;
	int bytes;
	int offset = 0;
	recv(socket, &bytes, sizeof(int), MSG_WAITALL);
	void* stream = malloc(bytes);
	recv(socket, stream, bytes, MSG_WAITALL);

	switch(codigoOperacion)
	{
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = malloc(sizeof(estructuraNew));
			estructuraNew->nombre_pokemon.nombre = malloc(sizeof(estructuraNew->nombre_pokemon.nombre));

			copiar_nombre(socket, &(estructuraNew->nombre_pokemon), stream, &offset);
			copiar_coordenadas(socket, &(estructuraNew->coordenadas), stream, &offset);
			copiar_variable(socket, &(estructuraNew->cantidad_pokemons), stream, &offset, sizeof(uint32_t));

			mensajeRecibido = estructuraNew;
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estructuraAppeared = malloc(sizeof(estructuraAppeared));
			estructuraAppeared->nombre_pokemon.nombre = malloc(sizeof(estructuraAppeared->nombre_pokemon.nombre));

			copiar_nombre(socket, &(estructuraAppeared->nombre_pokemon), stream, &offset);
			copiar_coordenadas(socket, &(estructuraAppeared->coordenadas), stream, &offset);

			mensajeRecibido = estructuraAppeared;
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estructuraGet = malloc(sizeof(estructuraGet));
			estructuraGet->nombre_pokemon.nombre = malloc(sizeof(estructuraGet->nombre_pokemon.nombre));

			copiar_nombre(socket, &(estructuraGet->nombre_pokemon), stream, &offset);

			mensajeRecibido = estructuraGet;
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = malloc(sizeof(estructuraLocalized));
			estructuraLocalized->nombre_pokemon.nombre = malloc(sizeof(estructuraLocalized->nombre_pokemon.nombre));

			copiar_nombre(socket, &(estructuraLocalized->nombre_pokemon), stream, &offset);
			copiar_variable(socket, &(estructuraLocalized->cantidad_coordenadas), stream, &offset, sizeof(uint32_t));
			estructuraLocalized->coordenadas = malloc(sizeof(uint32_t) * estructuraLocalized->cantidad_coordenadas * 2);
			copiar_variable(socket, estructuraLocalized->coordenadas, stream, &offset,sizeof(uint32_t) * estructuraLocalized->cantidad_coordenadas * 2);


			mensajeRecibido = estructuraLocalized;
			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estructuraCatch = malloc(sizeof(estructuraCatch));
			estructuraCatch->nombre_pokemon.nombre = malloc(sizeof(estructuraCatch->nombre_pokemon.nombre));

			copiar_nombre(socket, &(estructuraCatch->nombre_pokemon), stream, &offset);
			copiar_coordenadas(socket, &(estructuraCatch->coordenadas), stream, &offset);

			mensajeRecibido = estructuraCatch;
			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estructuraCaught = malloc(sizeof(estructuraCaught));

			copiar_variable(socket, &(estructuraCaught->atrapado), stream, &offset, sizeof(uint32_t));

			mensajeRecibido = estructuraCaught;
			break;
		default: printf("Error codigo op"); break;
	}
	return mensajeRecibido;
}

int recibir_codigo_operacion(int socket_cliente)
{
	int cod_op;

	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;

	return cod_op;
}

uint32_t recibir_id(int socket_cliente)
{
	int id;
	recv(socket_cliente, &id, sizeof(uint32_t), MSG_WAITALL);
	return id;
}

void copiar_nombre(int socket, t_nombrePokemon* estructuraNombre, void* stream, int* offset)
{
	estructuraNombre->nombre = malloc(estructuraNombre->nombre_lenght);

	copiar_variable(socket, &(estructuraNombre->nombre_lenght), stream, offset, sizeof(uint32_t));
	copiar_variable(socket, estructuraNombre->nombre, stream, offset, estructuraNombre->nombre_lenght);
}

void copiar_coordenadas(int socket, t_coordenadas* estructuraCoordenadas, void* stream, int* offset)
{
	copiar_variable(socket, &(estructuraCoordenadas->posX), stream, offset, sizeof(uint32_t));
	copiar_variable(socket, &(estructuraCoordenadas->posY), stream, offset, sizeof(uint32_t));
}

void copiar_variable(int socket, void* variable, void* stream, int* offset, int size)
{
	memcpy(variable, stream + *offset, size);
	*offset += size;
}

/////////////////////////////////
// ---- Respuesta mensaje ---- //
/////////////////////////////////

void enviar_id_respuesta(uint32_t id_msg, int socket_cliente)
{
	int offset = 0;
	void* a_enviar = malloc(sizeof(uint32_t));
	serializar_variable(a_enviar, &id_msg, sizeof(uint32_t), &offset);

	send(socket_cliente, a_enviar, sizeof(uint32_t), 0);
}

/////////////////////
// ---- Otros ---- //
/////////////////////

void liberar_conexion(int socket)
{
	close(socket);
}

