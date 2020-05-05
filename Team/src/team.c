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

	int socket_cliente = crear_conexion ("127.0.0.1", "6011"); //creamos la conexion con el broker



	puts("Soy un team!");

	fflush(stdout);

//	while(1)
//	{
//			int socket_cliente = esperar_cliente(socket_servidor);
//			pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
//			pthread_detach(thread);
//	}



	//Hito 2




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





//crear un pokemon a partir del nombre --> string --> struct --> string splitSS
