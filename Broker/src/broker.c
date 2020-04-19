/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Proceso Broker
 ============================================================================
 */

#include "broker.h"


int main(void) {

	//t_config* config = leer_config();

	int socket_servidor = iniciar_servidor(IP, PUERTO);

	printf("Broker!");
	fflush(stdout);

	while(1)
		esperar_cliente(socket_servidor);

}

t_log* iniciar_logger(void)
{
	//TODO catchear si == NULL
	return log_create(BROKER_LOG, BROKER_NAME, true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	//TODO catchear si == NULL
	return config_create(BROKER_CONFIG);

}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);
}

void serve_client(int* socket_cliente)
{
	int cod_op;
	if(recv(*socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket_cliente);
}

void process_request(int cod_op, int cliente_fd) {
	int size;
	t_new_message* new_pokemon_msg;

	switch (cod_op) {
		case NEW_POKEMON:
			new_pokemon_msg = malloc(sizeof(new_pokemon_msg));
			new_pokemon_msg = (t_new_message*) recibir_mensaje(cliente_fd, &size);

			printf("%s",new_pokemon_msg->nombre_pokemon.nombre);
			fflush(stdout);

			free(new_pokemon_msg);
			break;
		case APPEARED_POKEMON:
			printf("APPEARED_POKEMON");
			fflush(stdout);
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		default:
			printf("default");
			fflush(stdout);
			break;
	}
}

void terminar_programa(int socket_servidor, t_log* logger, t_config* config)
{
	liberar_conexion(socket_servidor);
	log_destroy(logger);
	config_destroy(config);
}
