#include "mensajes.h"

t_buffer* serializar_new_pokemon_msg(t_new_pokemon_msg* new_pokemon_msg) {

	int tam_nombre_lenght = sizeof(new_pokemon_msg->nombre_pokemon.nombre_lenght);
	int tam_nombre = new_pokemon_msg->nombre_pokemon.nombre_lenght;
	int tam_coordx = sizeof(new_pokemon_msg->coords.posx);
	int tam_coordy = sizeof(new_pokemon_msg->coords.posy);
	int tam_cantidad = sizeof(new_pokemon_msg->cantidad);

	int tamano_mensaje = tam_nombre_lenght + tam_nombre + tam_coordx + tam_coordy + tam_cantidad;

	int offset = 0;
	void* stream = malloc(tamano_mensaje);

	memcpy(stream + offset, &(new_pokemon_msg->nombre_pokemon.nombre_lenght), tam_nombre_lenght);
	offset += sizeof(tam_nombre_lenght);
	memcpy(stream + offset, new_pokemon_msg->nombre_pokemon.nombre, tam_nombre);
	offset += sizeof(tam_nombre);
	memcpy(stream + offset, &(new_pokemon_msg->coords.posx), tam_coordx);
	offset += sizeof(tam_coordx);
	memcpy(stream + offset, &(new_pokemon_msg->coords.posy), tam_coordy);
	offset += sizeof(tam_coordy);
	memcpy(stream + offset, &(new_pokemon_msg->cantidad), tam_cantidad);

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = tamano_mensaje;
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, stream, buffer->size);

	free(stream);

	return buffer;
}


t_new_pokemon_msg* deserializar_new_pokemon_msg(int socket_cliente) {

	t_new_pokemon_msg* new_pokemon_msg = malloc(new_pokemon_msg);

	recv(socket_cliente, &(new_pokemon_msg->nombre_pokemon.nombre_lenght), sizeof(new_pokemon_msg->nombre_pokemon.nombre_lenght), MSG_WAITALL);
	recv(socket_cliente, new_pokemon_msg->nombre_pokemon.nombre, sizeof(new_pokemon_msg->nombre_pokemon.nombre_lenght), MSG_WAITALL);
	recv(socket_cliente, &(new_pokemon_msg->coords.posx), sizeof(new_pokemon_msg->coords.posx), MSG_WAITALL);
	recv(socket_cliente, &(new_pokemon_msg->coords.posy), sizeof(new_pokemon_msg->coords.posy), MSG_WAITALL);
	recv(socket_cliente, &(new_pokemon_msg->cantidad), sizeof(new_pokemon_msg->cantidad), MSG_WAITALL);

	return new_pokemon_msg;
}

