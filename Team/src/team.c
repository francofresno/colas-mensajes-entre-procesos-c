/*
 ============================================================================
 Name        : Team
 Author      : Fran and Co
 Description : Proceso Team
 ============================================================================
*/

#include "team.h"

extern t_list* entrenadores;
extern t_list* objetivoTeam;
t_list* atrapados;
t_list* pendientes;
//extern t_list* hilosEntrenadores;
t_list* id_mensajeGet;

char* ipBroker;
char* puertoBroker;
char* ID_TEAM;

extern pthread_mutex_t mutex_hay_pokemones;

int main(void) {

	inicializarBinarios();
	inicializarListas();
	t_config* config = leer_config();

	ID_TEAM = config_get_string_value(config, "ID");

	ponerEntrenadoresEnLista(config);

	ipBroker = config_get_string_value(config, "IP_BROKER");
	puertoBroker = config_get_string_value(config, "PUERTO_BROKER");

	suscribirseAColas(config); //TODO terminar y checkear
	enviarMensajes();




////	op_code codigoOperacion;
////	int socket_cliente;
//
	puts("Soy un team!\n");
//
//	fflush(stdout);
//
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
	return log_create(TEAM_LOG, TEAM_NAME, true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	return config_create(TEAM_CONFIG);

}

void suscribirseAColas(){ //TODO HILOS
	suscribirseA(APPEARED_POKEMON);
	suscribirseA(LOCALIZED_POKEMON);
	suscribirseA(CAUGHT_POKEMON);

}

void suscribirseA(op_code* codigo){

	op_code tipo_cola = *codigo;
	printf("===============\n");
	printf("TEST SUSCRIPCION A COLA\n");
	printf("===============\n");
	fflush(stdout);

	int socket_cliente = crear_conexion(ipBroker, puertoBroker);
	printf("Conexion con broker en socket %d\n", socket_cliente);

	t_suscripcion_msg* estructuraSuscripcion = malloc(sizeof(*estructuraSuscripcion));
	estructuraSuscripcion->id_proceso = ID_TEAM;
	estructuraSuscripcion->tipo_cola = tipo_cola;
	estructuraSuscripcion->tiempo=0;

	int status_susc = suscribirse_a_cola(estructuraSuscripcion, socket_cliente);
	printf("status de envio de susc %d\n", status_susc);

	uint32_t cant_paquetes;
	t_list* paquetes = respuesta_suscripcion_obtener_paquetes(socket_cliente, &cant_paquetes);
	printf("Recibi %d mensajes\n", cant_paquetes);
	fflush(stdout);

	int status_ack=informar_ask(socket_cliente);
	printf("Informe ACK con status %d \n", status_ack);

	for(int i = 0; i<cant_paquetes; i++){
		t_paquete* paquete_recibido = list_get(paquetes, i);
		printf("Fijarse que hacer con los paquetes");
	}

	free(estructuraSuscripcion);

	printf("---------Recepciones por suscripcion---------");

	while(1){
		char*nombre_recibido = NULL;
		t_paquete*paquete_recibido = recibir_paquete(socket_cliente,&nombre_recibido);

		if(paquete_recibido == NULL){
			break; //TODO VER COMO RECONECTARNOS CON TIEMPO DE RECONECCION

		}

		printf("------------------------\n");
		printf("COD OP: %d\n", paquete_recibido->codigo_operacion);
		printf("ID: %d\n", paquete_recibido->id);
		printf("ID_CORRELATIVO: %d\n", paquete_recibido->id_correlativo);

		int status_ack = informar_ack(socket_cliente);
		printf("informe ACK con status %d\n", status_ack);
	}

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

		t_appearedPokemon_msg* estructura = malloc(sizeof(t_appearedPokemon_msg));

		t_nombrePokemon pokemon = estructura->nombre_pokemon;

		t_coordenadas coordenadas = estructura->coordenadas;

		requiere(&pokemon, coordenadas);


			break;

		case LOCALIZED_POKEMON: ;

//		t_nombrePokemon nombre_pokemon;
//			uint32_t cantidad_coordenadas;
//			t_coordenadas* coordenadas;
//		} t_localizedPokemon_msg;

		puts("Llego un localized al Team!\n");
		pthread_mutex_unlock(&mutex_hay_pokemones);


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

void enviarMensajes(){
	enviarMensajeGetABroker();

}

void enviarMensajeGetABroker(){

	t_list* objetivoTeamSinRepe = list_create();
	objetivoTeamSinRepe = eliminarRepetidos();

	int tamanioObjTeamSinRepetidos = list_size(objetivoTeamSinRepe);
	pthread_t pthread_id;

	for(int a=0; a< tamanioObjTeamSinRepetidos ; a++){
		t_nombrePokemon* pokemon = (t_nombrePokemon*) list_get(objetivoTeamSinRepe, a);
		pthread_create(&pthread_id, NULL, (void*) enviarMensajeGet, pokemon);
		pthread_detach(pthread_id);
	}
}

t_list* eliminarRepetidos(){

	int a = list_size(objetivoTeam);
	t_list* objetivoTeamSinRepetidos = list_create();
	int c=0;

	for(int i=0; i < a; i++){

		int k=0;
		while((k<c) && (sonIguales(list_get(objetivoTeamSinRepetidos,k), list_get(objetivoTeam, i))!=0)){
			k++;
		}

		if(k==c){
			list_add(objetivoTeamSinRepetidos, (t_nombrePokemon*)list_get(objetivoTeam, i));
			c++;
		}
	}

	return objetivoTeamSinRepetidos;
}

void enviarMensajeGet(t_nombrePokemon* pokemon){ //TODO fijarse CON DEBUG
	t_getPokemon_msg* estructuraPokemon = malloc(sizeof(t_getPokemon_msg));
	estructuraPokemon->nombre_pokemon = *pokemon;
	int socket_cliente = crear_conexion(ipBroker, puertoBroker);
	int status = enviar_mensaje(GET_POKEMON, 0, 0, estructuraPokemon, socket_cliente);

	if(status>0){
		esperarId(socket_cliente);
	}else{
		//TODO default no existen locaciones para la especie requerida
	}


	liberar_conexion(socket_cliente);
}

void inicializarBinarios(){
	pthread_mutex_lock(&mutex_hay_pokemones);
}

void inicializarListas(){
	id_mensajeGet = list_create();
	atrapados = list_create();
	pendientes = list_create();
}

void esperarId(int socket_cliente){
	uint32_t id_respuesta = recibir_id(socket_cliente);
	printf("El id recibido de broker es: %d\n", id_respuesta);
	list_add(id_mensajeGet, &id_respuesta);
}


void requiere(t_nombrePokemon* pokemon, t_coordenadas coordenadas){

	diferencia();
	int a = list_size(pendientes);
	int j=0;

	for(int i=0; i < a; i++){

		if(sonIguales(pokemon, list_get(pendientes, i))!=0){
			j++;
		}
	}

	if(j!=a){

		list_add(atrapados, pokemon);
		//hace lo que tenga que hacer --> poner a planificar al entrenador dormido o listo (con coordenadas y pokemon)
	}
}

void diferencia(){ 		//llenar lista pendientes  //TODO probar
	int a = list_size(objetivoTeam);
	int b = list_size(atrapados);

	for(int i=0; i < a; i++){

		int j=0;

		while((j < b) && (sonIguales(list_get(atrapados,j), list_get(objetivoTeam, i))!=0)){
			j++;
		}

		if(j==b){
			list_add(pendientes, (t_nombrePokemon*)list_get(objetivoTeam, i));
		}
	}

}



