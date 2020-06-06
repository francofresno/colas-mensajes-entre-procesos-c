/*
 ============================================================================
 Name        : GameCard
 Author      : Fran and Co
 Description : Proceso Game Card
 ============================================================================
 */

#include "gamecard.h"

char* ip;
char* puerto;
int tiempoReconexion;

int main(void) {

	t_config* config = config_create(GAMECARD_CONFIG);
	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");
	tiempoReconexion =  config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");

	printf("%s\n%s\n",ip,puerto);

	t_datosHilo datosHiloNP;
	datosHiloNP.id_hilo = config_get_int_value(config, "ID_HILO_NP");
	datosHiloNP.tipoCola = NEW_POKEMON;
	t_datosHilo datosHiloGP;
	datosHiloGP.id_hilo = config_get_int_value(config, "ID_HILO_GP");
	datosHiloGP.tipoCola = GET_POKEMON;
	t_datosHilo datosHiloCP;
	datosHiloCP.id_hilo = config_get_int_value(config, "ID_HILO_CP");
	datosHiloCP.tipoCola = CATCH_POKEMON;

	pthread_create(&threadNewPokemon, NULL, (void*)conectarseYSuscribirse, &datosHiloNP);
	printf("Creado threadNewPokemon\n");
	fflush(stdout);
	pthread_create(&threadGetPokemon, NULL, (void*)conectarseYSuscribirse, &datosHiloGP);
	printf("Creado threadGetPokemon\n");
	fflush(stdout);
	pthread_create(&threadCatchPokemon, NULL, (void*)conectarseYSuscribirse, &datosHiloCP);
	printf("Creado threadCatchPokemon\n");
	fflush(stdout);
	pthread_join(threadNewPokemon, NULL);
	pthread_join(threadGetPokemon, NULL);
	pthread_join(threadCatchPokemon, NULL);
	config_destroy(config);
	return EXIT_SUCCESS;
}

t_config* leer_config(void)
{
	return config_create(GAMECARD_CONFIG);
}

void conectarseYSuscribirse(t_datosHilo* datosHilo)
{
	printf("Intento de conexion\n");
	fflush(stdout);
	int socket_cliente = crear_conexion(ip, puerto);
	printf("Llegue al principio del if");
	t_suscripcion_msg estructuraSuscripcion;
	estructuraSuscripcion.id_proceso = datosHilo->id_hilo;
	estructuraSuscripcion.tipo_cola = datosHilo->tipoCola;
	int status = suscribirse_a_cola(&estructuraSuscripcion, socket_cliente);

	if(status < 0)
	{
		printf("Conexion fallida. Reintentando...");
		sleep(tiempoReconexion);
		printf("Termino el sleep");
		conectarseYSuscribirse(datosHilo);
	}
	else
	{
		printf("Conectado correctamente\n");
		recepcionMensajesDeCola(socket_cliente);
	}
}

void recepcionMensajesDeCola(int socket_cliente)
{
	uint32_t cant_paquetes;
	t_list* paquetes = respuesta_suscripcion_obtener_paquetes(socket_cliente, &cant_paquetes);

	for(int i=0; i < cant_paquetes; i++)
	{
		t_paquete* paquete_recibido = list_get(paquetes, i);
		printf("----------------------\n");
		printf("Paquete: %d\n", i);
		printf("COD OP: %d\n", paquete_recibido->codigo_operacion);
		printf("ID: %d\n", paquete_recibido->id);
		printf("ID_CORRELATIVO: %d\n", paquete_recibido->id_correlativo);
		free(paquete_recibido);
	}

	free(paquetes);

	while(1)
	{
		char* nombre_recibido = NULL;
		t_paquete* paquete_recibido = recibir_paquete(socket_cliente, &nombre_recibido);
		printf("----------------------\n");
		printf("COD OP: %d\n", paquete_recibido->codigo_operacion);
		printf("ID: %d\n", paquete_recibido->id);
		printf("ID_CORRELATIVO: %d\n", paquete_recibido->id_correlativo);
		informar_ack(socket_cliente);
		free(paquete_recibido);
	}
}
