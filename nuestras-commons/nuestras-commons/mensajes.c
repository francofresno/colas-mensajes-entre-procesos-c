#include "mensajes.h"

//t_buffer* serializar_new_pokemon_msg(t_newPokemon_msg* new_pokemon_msg) {
//
//	int tam_nombre_lenght = sizeof(new_pokemon_msg->nombre_pokemon->nombre_lenght);
//	int tam_nombre = new_pokemon_msg->nombre_pokemon->nombre_lenght;
//	int tam_coordx = sizeof(new_pokemon_msg->coordenadas->posX);
//	int tam_coordy = sizeof(new_pokemon_msg->coordenadas->posY);
//	int tam_cantidad = sizeof(new_pokemon_msg->cantidad_pokemons);
//
//	int tamano_mensaje = tam_nombre_lenght + tam_nombre + tam_coordx + tam_coordy + tam_cantidad;
//
//	int offset = 0;
//	void* stream = malloc(tamano_mensaje);
//
//	memcpy(stream + offset, &(new_pokemon_msg->nombre_pokemon->nombre_lenght), tam_nombre_lenght);
//	offset += tam_nombre_lenght;
//	memcpy(stream + offset, new_pokemon_msg->nombre_pokemon->nombre, tam_nombre);
//	offset += tam_nombre;
//	memcpy(stream + offset, &(new_pokemon_msg->coordenadas->posX), tam_coordx);
//	offset += tam_coordx;
//	memcpy(stream + offset, &(new_pokemon_msg->coordenadas->posY), tam_coordy);
//	offset += tam_coordy;
//	memcpy(stream + offset, &(new_pokemon_msg->cantidad_pokemons), tam_cantidad);
//
//	t_buffer* buffer = malloc(sizeof(t_buffer));
//	buffer->size = tamano_mensaje;
//	buffer->stream = malloc(buffer->size);
//	memcpy(buffer->stream, stream, buffer->size);
//
//	free(stream);
//
//	return buffer;
//}
//
//t_newPokemon_msg* deserializar_new_pokemon_msg(int socket_cliente) {
//
//	t_newPokemon_msg* new_pokemon_msg = malloc(sizeof(new_pokemon_msg));
//	t_nombrePokemon* nombre_pokemon = malloc(sizeof(nombre_pokemon));
//	nombre_pokemon->nombre = malloc(sizeof(nombre_pokemon->nombre));
//	t_coordenadas* coordenadas = malloc(sizeof(coordenadas));
//	new_pokemon_msg->nombre_pokemon = *nombre_pokemon;
//	new_pokemon_msg->coordenadas = *coordenadas;
//
//	recv(socket_cliente, &(new_pokemon_msg->nombre_pokemon.nombre_lenght), sizeof(new_pokemon_msg->nombre_pokemon.nombre_lenght), MSG_WAITALL);
//	recv(socket_cliente, new_pokemon_msg->nombre_pokemon.nombre, new_pokemon_msg->nombre_pokemon.nombre_lenght, MSG_WAITALL);
//	recv(socket_cliente, &(new_pokemon_msg->coordenadas.posX), sizeof(new_pokemon_msg->coordenadas.posX), MSG_WAITALL);
//	recv(socket_cliente, &(new_pokemon_msg->coordenadas.posY), sizeof(new_pokemon_msg->coordenadas.posY), MSG_WAITALL);
//	recv(socket_cliente, &(new_pokemon_msg->cantidad_pokemons), sizeof(new_pokemon_msg->cantidad_pokemons), MSG_WAITALL);
//
//	return new_pokemon_msg;
//}

