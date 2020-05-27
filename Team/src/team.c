/*
 ============================================================================
 Name        : Team
 Author      : Fran and Co
 Description : Proceso Team
 ============================================================================
 */

#include "team.h"

extern t_list* entrenadores;
//extern t_list* hilosEntrenadores;

int main(void) {

	t_config* config = leer_config();

	ponerEntrenadoresEnLista(config);

		int tamanio = list_size(entrenadores);

		for (int b = 0; b < tamanio; b++) {
			t_entrenador* entrenador = (t_entrenador*) list_get(entrenadores, b);
			printf("La cantidad de pokemones del entrenador de la posicion %d es %d\n", b, entrenador->cantidad_pokemons);
		}

//	char* ipBroker = config_get_string_value(config, "IP_BROKER");
//	char* puertoBroker = config_get_string_value(config, "PUERTO_BROKER");

////	op_code codigoOperacion;
////	int socket_cliente;
//
	puts("Soy un team!\n");
//
//	fflush(stdout);
//
//	int socket_servidor = iniciar_servidor(IP, PUERTO);
//
//	while(1) {
//
//		int socket_cliente = esperar_cliente(socket_servidor);
//		if(socket_cliente > 0) {
//			pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
//			pthread_detach(thread);
//		}
//	}


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
	char* nombre_recibido = NULL;

	t_paquete* paquete_recibido = recibir_paquete(*socket_cliente, &nombre_recibido);

	process_request(paquete_recibido->codigo_operacion, paquete_recibido->id_correlativo, paquete_recibido->mensaje, *socket_cliente);

	free_paquete_recibido(nombre_recibido, paquete_recibido);
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

op_code stringACodigoOperacion(const char* string)
{
	for(int i = 0; i < sizeof(conversionCodigoOp) / sizeof(conversionCodigoOp[0]); i++)
	{
		if(!strcmp(string, conversionCodigoOp[i].str))
			return conversionCodigoOp[i].codigoOperacion;
	}
	return ERROR_CODIGO;
}


