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

int enviar_mensaje(op_code codigoOperacion, uint32_t id, uint32_t id_correlativo, void* mensaje, int socket_envio)
{
	uint32_t bytes;
	void* paqueteAEnviar = serializar_paquete(codigoOperacion, id, id_correlativo, mensaje, &bytes);
	return send(socket_envio, paqueteAEnviar, bytes + sizeof(bytes), 0);
	//free(paqueteAEnviar);
}

void* serializar_paquete(op_code codigo_operacion, uint32_t id, uint32_t id_correlativo, void* estructura, uint32_t* bytes)
{
	int offset = 0;
	*bytes = sizeof(codigo_operacion) + sizeof(id) + sizeof(id_correlativo);
	void* a_enviar;

	switch(codigo_operacion)
	{
		case SUSCRIPCION: ;
			t_suscripcion_msg* estSuscripcion = estructura;
			*bytes += sizeof(estSuscripcion->id_proceso)
					+ sizeof(estSuscripcion->tipo_cola);
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, bytes, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &codigo_operacion, sizeof(codigo_operacion), &offset);
			serializar_variable(a_enviar, &id, sizeof(id), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(id_correlativo), &offset);
			serializar_variable(a_enviar, &(estSuscripcion->id_proceso), sizeof(estSuscripcion->id_proceso), &offset);
			serializar_variable(a_enviar, &(estSuscripcion->tipo_cola), sizeof(estSuscripcion->tipo_cola), &offset);
			break;
		case NEW_POKEMON: ;
			t_newPokemon_msg* estNew = estructura;
			*bytes += estNew->nombre_pokemon.nombre_lenght
					+ sizeof(estNew->nombre_pokemon.nombre_lenght)
					+ sizeof(estNew->cantidad_pokemons)
					+ sizeof(estNew->coordenadas.posX)
					+ sizeof(estNew->coordenadas.posY);
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, bytes, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &codigo_operacion, sizeof(codigo_operacion), &offset);
			serializar_variable(a_enviar, &id, sizeof(id), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(id_correlativo), &offset);
			serializar_nombre(a_enviar, estNew->nombre_pokemon, &offset);
			serializar_variable(a_enviar, &(estNew->coordenadas.posX), sizeof(estNew->coordenadas.posX), &offset);
			serializar_variable(a_enviar, &(estNew->coordenadas.posY), sizeof(estNew->coordenadas.posY), &offset);
			serializar_variable(a_enviar, &(estNew->cantidad_pokemons), sizeof(estNew->cantidad_pokemons), &offset);
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estAppeared = estructura;
			*bytes += estAppeared->nombre_pokemon.nombre_lenght
					+ sizeof(estAppeared->nombre_pokemon.nombre_lenght)
					+ sizeof(estAppeared->coordenadas.posX)
					+ sizeof(estAppeared->coordenadas.posY);
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, bytes, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &codigo_operacion, sizeof(codigo_operacion), &offset);
			serializar_variable(a_enviar, &id, sizeof(id), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(id_correlativo), &offset);
			serializar_nombre(a_enviar, estAppeared->nombre_pokemon, &offset);
			serializar_variable(a_enviar, &(estAppeared->coordenadas.posX), sizeof(estAppeared->coordenadas.posX), &offset);
			serializar_variable(a_enviar, &(estAppeared->coordenadas.posY), sizeof(estAppeared->coordenadas.posY), &offset);
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estGet = estructura;
			*bytes += estGet->nombre_pokemon.nombre_lenght
					+ sizeof(estGet->nombre_pokemon.nombre_lenght);
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, bytes, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &codigo_operacion, sizeof(codigo_operacion), &offset);
			serializar_variable(a_enviar, &id, sizeof(id), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(id_correlativo), &offset);
			serializar_nombre(a_enviar, estGet->nombre_pokemon, &offset);
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estLocalized = estructura;
			*bytes += estLocalized->nombre_pokemon.nombre_lenght
					+ sizeof(estLocalized->nombre_pokemon.nombre_lenght)
					+ sizeof(estLocalized->cantidad_coordenadas)
					+ (estLocalized->cantidad_coordenadas)*sizeof(estAppeared->coordenadas.posX)*sizeof(estAppeared->coordenadas.posY);
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, bytes, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &codigo_operacion, sizeof(codigo_operacion), &offset);
			serializar_variable(a_enviar, &id, sizeof(id), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(id_correlativo), &offset);
			serializar_nombre(a_enviar, estLocalized->nombre_pokemon, &offset);

			serializar_variable(a_enviar, &(estLocalized->cantidad_coordenadas), sizeof(estLocalized->cantidad_coordenadas), &offset);
			for(int i = 0; i < estLocalized->cantidad_coordenadas; i++) {
				serializar_variable(a_enviar, &(estLocalized->coordenadas[i].posX), sizeof(estLocalized->coordenadas[i].posX), &offset);
				serializar_variable(a_enviar, &(estLocalized->coordenadas[i].posY), sizeof(estLocalized->coordenadas[i].posY), &offset);
			}

			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estCatch = estructura;
			*bytes += estCatch->nombre_pokemon.nombre_lenght
					+ sizeof(estCatch->nombre_pokemon.nombre_lenght)
					+ sizeof(estCatch->coordenadas.posX)
					+ sizeof(estCatch->coordenadas.posY);
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, bytes, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &codigo_operacion, sizeof(codigo_operacion), &offset);
			serializar_variable(a_enviar, &id, sizeof(id), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(id_correlativo), &offset);
			serializar_nombre(a_enviar, estCatch->nombre_pokemon, &offset);
			serializar_variable(a_enviar, &(estCatch->coordenadas.posX), sizeof(estCatch->coordenadas.posX), &offset);
			serializar_variable(a_enviar, &(estCatch->coordenadas.posY), sizeof(estCatch->coordenadas.posY), &offset);
			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estCaught = estructura;
			*bytes += sizeof(estCaught->atrapado);
			a_enviar = malloc(sizeof(*bytes));

			serializar_variable(a_enviar, bytes, sizeof(uint32_t), &offset);
			serializar_variable(a_enviar, &codigo_operacion, sizeof(codigo_operacion), &offset);
			serializar_variable(a_enviar, &id, sizeof(id), &offset);
			serializar_variable(a_enviar, &id_correlativo, sizeof(id_correlativo), &offset);
			serializar_variable(a_enviar, &(estCaught->atrapado), sizeof(estCaught->atrapado), &offset);
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
	serializar_variable(aEnviar, &(nombrePokemon.nombre_lenght), sizeof(nombrePokemon.nombre_lenght), offset);
	serializar_variable(aEnviar, nombrePokemon.nombre, nombrePokemon.nombre_lenght, offset);
}


/////////////////////////////////////
// ---- Recepción de mensajes ---- //
/////////////////////////////////////

uint32_t recibir_cantidad_bytes(int socket_cliente)
{
	uint32_t bytes;
	recv(socket_cliente, &bytes, sizeof(bytes), MSG_WAITALL); //TODO validar recv -1
	return bytes;
}

t_paquete* recibir_paquete(int socket_cliente)
{
	uint32_t bytes = recibir_cantidad_bytes(socket_cliente);

	t_paquete* paquete_recibido = malloc(sizeof(paquete_recibido));
	void* stream = malloc(bytes);
	recv(socket_cliente, stream, bytes, MSG_WAITALL); //TODO chequear status

	int offset = 0;
	copiar_variable(&(paquete_recibido->codigo_operacion), stream, &offset, sizeof(paquete_recibido->codigo_operacion));
	copiar_variable(&(paquete_recibido->id), stream, &offset, sizeof(paquete_recibido->id));
	copiar_variable(&(paquete_recibido->id_correlativo), stream, &offset, sizeof(paquete_recibido->id_correlativo));

	// Le resto a "bytes" los bytes que ya copie del stream
	paquete_recibido->mensaje = malloc(bytes - offset);

	switch(paquete_recibido->codigo_operacion)
	{
		case SUSCRIPCION: ;
			t_suscripcion_msg* estSuscripcion = malloc(sizeof(estSuscripcion));

			copiar_variable(&(estSuscripcion->id_proceso), stream, &offset, sizeof(estSuscripcion->id_proceso));
			copiar_variable(&(estSuscripcion->tipo_cola), stream, &offset, sizeof(estSuscripcion->tipo_cola));

			paquete_recibido->mensaje = estSuscripcion;
			break;
		case NEW_POKEMON: ;
			t_newPokemon_msg* estructuraNew = malloc(sizeof(estructuraNew));
			estructuraNew->nombre_pokemon.nombre = malloc(sizeof(estructuraNew->nombre_pokemon.nombre)); //TODO revisar esta asignacion

			copiar_nombre(&(estructuraNew->nombre_pokemon), stream, &offset);
			copiar_coordenadas(&(estructuraNew->coordenadas), stream, &offset);
			copiar_variable(&(estructuraNew->cantidad_pokemons), stream, &offset, sizeof(estructuraNew->cantidad_pokemons));

			paquete_recibido->mensaje = estructuraNew;
			printf("Recibi un new pkm \n");
			break;
		case APPEARED_POKEMON: ;
			t_appearedPokemon_msg* estructuraAppeared = malloc(sizeof(estructuraAppeared));
			estructuraAppeared->nombre_pokemon.nombre = malloc(sizeof(estructuraAppeared->nombre_pokemon.nombre));

			copiar_nombre(&(estructuraAppeared->nombre_pokemon), stream, &offset);
			copiar_coordenadas(&(estructuraAppeared->coordenadas), stream, &offset);

			paquete_recibido->mensaje = estructuraAppeared;
			break;
		case GET_POKEMON: ;
			t_getPokemon_msg* estructuraGet = malloc(sizeof(estructuraGet));
			estructuraGet->nombre_pokemon.nombre = malloc(sizeof(estructuraGet->nombre_pokemon.nombre));

			copiar_nombre(&(estructuraGet->nombre_pokemon), stream, &offset);

			paquete_recibido->mensaje = estructuraGet;
			break;
		case LOCALIZED_POKEMON: ;
			t_localizedPokemon_msg* estructuraLocalized = malloc(sizeof(estructuraLocalized));
			estructuraLocalized->nombre_pokemon.nombre = malloc(sizeof(estructuraLocalized->nombre_pokemon.nombre));

			copiar_nombre(&(estructuraLocalized->nombre_pokemon), stream, &offset);
			copiar_variable(&(estructuraLocalized->cantidad_coordenadas), stream, &offset, sizeof(estructuraLocalized->cantidad_coordenadas));
			estructuraLocalized->coordenadas = malloc( 2 * (estructuraLocalized->cantidad_coordenadas) * sizeof(uint32_t) );
			copiar_variable(estructuraLocalized->coordenadas, stream, &offset, 2 * (estructuraLocalized->cantidad_coordenadas) * sizeof(uint32_t));

			paquete_recibido->mensaje = estructuraLocalized;
			break;
		case CATCH_POKEMON: ;
			t_catchPokemon_msg* estructuraCatch = malloc(sizeof(estructuraCatch));
			estructuraCatch->nombre_pokemon.nombre = malloc(sizeof(estructuraCatch->nombre_pokemon.nombre));

			copiar_nombre(&(estructuraCatch->nombre_pokemon), stream, &offset);
			copiar_coordenadas(&(estructuraCatch->coordenadas), stream, &offset);

			paquete_recibido->mensaje = estructuraCatch;
			break;
		case CAUGHT_POKEMON: ;
			t_caughtPokemon_msg* estructuraCaught = malloc(sizeof(estructuraCaught));

			copiar_variable(&(estructuraCaught->atrapado), stream, &offset, sizeof(estructuraCaught->atrapado));

			paquete_recibido->mensaje = estructuraCaught;
			break;
		default: printf("Error codigo op"); break;
	}
	//free(stream);
	return(paquete_recibido);
}

void copiar_nombre(t_nombrePokemon* estructuraNombre, void* stream, int* offset)
{
	estructuraNombre->nombre = malloc(estructuraNombre->nombre_lenght);

	copiar_variable(&(estructuraNombre->nombre_lenght), stream, offset, sizeof(estructuraNombre->nombre_lenght));
	copiar_variable(estructuraNombre->nombre, stream, offset, estructuraNombre->nombre_lenght);
}

void copiar_coordenadas(t_coordenadas* estructuraCoordenadas, void* stream, int* offset)
{
	copiar_variable(&(estructuraCoordenadas->posX), stream, offset, sizeof(estructuraCoordenadas->posX));
	copiar_variable(&(estructuraCoordenadas->posY), stream, offset, sizeof(estructuraCoordenadas->posY));
}

void copiar_variable(void* variable, void* stream, int* offset, int size)
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
	void* a_enviar = malloc(sizeof(id_msg));
	serializar_variable(a_enviar, &id_msg, sizeof(id_msg), &offset);

	int status = send(socket_cliente, a_enviar, sizeof(id_msg), 0);
	printf("Envie %d y el status fue: %d\n", id_msg, status);
	free(a_enviar);
}

uint32_t recibir_id(int socket_cliente)
{
	uint32_t id;
	recv(socket_cliente, &id, sizeof(id), MSG_WAITALL);
	return id;
}

/////////////////////////////
// ---- Suscripciones ---- //
/////////////////////////////

int suscribirse_a_cola(t_suscripcion_msg* estructuraSuscripcion, int socket_servidor)
{
	return enviar_mensaje(SUSCRIPCION, 0, 0, (void*) estructuraSuscripcion, socket_servidor);
}

void responder_a_suscripcion(uint32_t cantidad_a_enviar, t_paquete paquetes[], int socket_envio)
{
	// Envio la cantidad de paquetes que se enviaran
	send(socket_envio, &cantidad_a_enviar, sizeof(cantidad_a_enviar), 0);

	if (cantidad_a_enviar > 0) {
		// Recorro los mensajes y los envio 1 por 1
		for (int i=0; i < cantidad_a_enviar; i++) {
			t_paquete paquete = paquetes[i];
			enviar_mensaje(paquete.codigo_operacion, paquete.id, paquete.id_correlativo, paquete.mensaje, socket_envio);
		}
	}
}

/////////////////////
// ---- Otros ---- //
/////////////////////


void liberar_conexion(int socket)
{
	close(socket);
}

