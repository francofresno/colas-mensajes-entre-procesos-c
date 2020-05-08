/*
 ============================================================================
 Name        : Team
 Author      : Fran and Co
 Description : Proceso Team
 ============================================================================
 */

#include "team.h"

int main(void) {

	t_config* config;

	config = leer_config();

	int socket_broker = crear_conexion ("127.0.0.1", "6011"); //creamos la conexion con el broker



	puts("Soy un team!\n");

	fflush(stdout);

	//Hito 2


	int socket_servidor = iniciar_servidor(IP, PUERTO);

	while(1) {

		int socket_cliente = esperar_cliente(socket_servidor);
		if(socket_cliente > 0) {
			pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
			pthread_detach(thread);
		}
	}


	return EXIT_SUCCESS;
}


t_log* iniciar_logger(void)
{
	//TODO catchear si == NULL
	return log_create(TEAM_LOG, TEAM_NAME, true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	//TODO catchear si == NULL
	return config_create(TEAM_CONFIG);

}

void serve_client(int* socket_cliente)
{
	t_paquete* paquete_recibido = recibir_paquete(*socket_cliente);

	process_request(paquete_recibido->codigo_operacion, paquete_recibido->id_correlativo, paquete_recibido->mensaje, *socket_cliente);
}

void process_request(int cod_op, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente)
{
	switch(cod_op)
	{

		case APPEARED_POKEMON: ;

		puts("Llego un appeared al Team!\n");

			break;

		case LOCALIZED_POKEMON: ;

		puts("Llego un localized al Team!\n");

			break;

		case CAUGHT_POKEMON: ;

		puts("Llego un caught al Team!\n");

			break;
	}
}


