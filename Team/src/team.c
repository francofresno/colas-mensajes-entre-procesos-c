/*
 ============================================================================
 Name        : Team
 Author      : Fran and Co
 Description : Proceso Team
 ============================================================================
*/

#include "team.h"

pthread_mutex_t mutex_send = PTHREAD_MUTEX_INITIALIZER;

int main(void) {

	inicializarListas();
	inicializarSemaforosYMutex();
	t_config* config = leer_config();
	inicializarConfig(config);

	ponerEntrenadoresEnLista(config);
	crearHilosEntrenadores(); //TODO fijarnos si anda.


	enviarMensajeGetABroker();

	suscribirseAColas();

	puts("Soy un team!\n");

	int socket_servidor = iniciar_servidor(IP_TEAM, PUERTO_TEAM);
	quedarseALaEscucha(&socket_servidor);


	return EXIT_SUCCESS;
}
void quedarseALaEscucha(int* socket_servidor) {
	while(1) {
		int socket_potencial = esperar_cliente(*socket_servidor);
		if(socket_potencial > 0) {
			int* socket_cliente = (int*) malloc(sizeof(int));
			*socket_cliente = socket_potencial;
			pthread_create(&thread,NULL,(void*)serve_client,socket_cliente);
			pthread_detach(thread);
		}
	}
}

t_log* iniciar_logger(void)
{
	return log_create(TEAM_LOG, TEAM_NAME, true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	return config_create(TEAM_CONFIG);

}

void inicializarConfig(t_config* config){

	IP_TEAM = config_get_string_value(config, "IP_TEAM");
	PUERTO_TEAM = config_get_string_value(config, "PUERTO_TEAM");
	ID_TEAM = config_get_int_value(config, "ID");
	TIEMPO_RECONEXION = config_get_int_value(config, "TIEMPO_RECONEXION");
	ipBroker = config_get_string_value(config, "IP_BROKER");
	puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
	algoritmoPlanificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	quantum= config_get_int_value(config, "QUANTUM");
	estimacionInicial= config_get_int_value(config, "ESTIMACION_INICIAL");
	alfa = config_get_double_value(config, "ALPHA");
	retardoCPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
	char* team_log = config_get_string_value(config, "LOG_FILE");
	LOGGER = log_create(team_log, PUERTO_TEAM, false, LOG_LEVEL_INFO);
}

void inicializarSemaforosYMutex(){
	sem_init(&sem_planificar, 0, 1);
}

void suscribirseAppeared(){
	suscribirseA(APPEARED_POKEMON);
}

void suscribirseCaught(){
	suscribirseA(CAUGHT_POKEMON);
}

void suscribirseLocalized(){
	suscribirseA(LOCALIZED_POKEMON);
}

void suscribirseAColas(){

	pthread_create(&thread, NULL, (void*)suscribirseAppeared, NULL);
	pthread_detach(thread);
	pthread_create(&thread, NULL, (void*)suscribirseCaught, NULL);
	pthread_detach(thread);
	pthread_create(&thread, NULL, (void*)suscribirseLocalized, NULL);
	pthread_join(thread, NULL);

}

void suscribirseA(op_code tipo_cola){

	printf("===============\n");
	printf("TEST SUSCRIPCION A COLA\n");
	printf("===============\n");
	fflush(stdout);

	pthread_mutex_lock(&mutex_send);
	int socket_cliente = crear_conexion(ipBroker, puertoBroker);
	while(socket_cliente<=0){
		sleep(TIEMPO_RECONEXION);
		log_reintento_conexion_broker();
		socket_cliente = crear_conexion(ipBroker, puertoBroker);
	}
	printf("Conexion con broker en socket %d\n", socket_cliente);

	t_suscripcion_msg* estructuraSuscripcion = malloc(sizeof(t_suscripcion_msg));
	estructuraSuscripcion->id_proceso = ID_TEAM;
	estructuraSuscripcion->tipo_cola = tipo_cola;
	estructuraSuscripcion->temporal = 0;

	int status_susc = suscribirse_a_cola(estructuraSuscripcion, socket_cliente);
	pthread_mutex_unlock(&mutex_send);

	printf("status de envio de susc %d\n", status_susc);

	uint32_t cant_paquetes;
	t_list* paquetes = respuesta_suscripcion_obtener_paquetes(socket_cliente, &cant_paquetes);
	printf("Recibi %d mensajes\n", cant_paquetes);
	fflush(stdout);

	int status_ack=informar_ack(socket_cliente);
	printf("Informe ACK con status %d \n", status_ack);

	for(int i = 0; i<cant_paquetes; i++){
		t_paquete* paquete_recibido = list_get(paquetes, i);
		process_request(paquete_recibido->codigo_operacion, paquete_recibido->id, paquete_recibido->mensaje, socket_cliente);
		printf("Recibi un mensaje por haberme suscripto: %d\n", paquete_recibido->codigo_operacion);
	}

	free(estructuraSuscripcion);

	printf("---------Recepciones por suscripcion---------cola %d\n", tipo_cola);

	while(1){
		char*nombre_recibido = NULL;
		uint32_t tamanioRecibido;
		t_paquete* paquete_recibido = recibir_paquete(socket_cliente,&nombre_recibido, &tamanioRecibido);

		if(paquete_recibido == NULL){
			sleep(TIEMPO_RECONEXION);
			log_reintento_conexion_broker();
			suscribirseA(tipo_cola);
		}

		printf("------------------------\n");
		printf("COD OP: %d\n", paquete_recibido->codigo_operacion);
		printf("ID: %d\n", paquete_recibido->id);
		printf("ID_CORRELATIVO: %d\n", paquete_recibido->id_correlativo);
		process_request(paquete_recibido->codigo_operacion, paquete_recibido->id, paquete_recibido->mensaje, socket_cliente);

		int status_ack = informar_ack(socket_cliente);
		printf("informe ACK con status %d\n", status_ack);
	}

}

void serve_client(int* socket_cliente)
{
	char* nombre_recibido = NULL;
	uint32_t tamanioRecibido;

	t_paquete* paquete_recibido = recibir_paquete(*socket_cliente, &nombre_recibido, &tamanioRecibido);

	process_request(paquete_recibido->codigo_operacion, paquete_recibido->id_correlativo, paquete_recibido->mensaje, *socket_cliente);

	free_paquete_recibido(nombre_recibido, paquete_recibido);
}

void process_request(int cod_op, uint32_t id_correlativo, void* mensaje_recibido, int socket_cliente)
{
	switch(cod_op)
	{
		case APPEARED_POKEMON: ;

			t_appearedPokemon_msg* mensajeAppeared = (t_appearedPokemon_msg*) mensaje_recibido;

			t_nombrePokemon pokemon = mensajeAppeared->nombre_pokemon;

			t_coordenadas coordenadas = mensajeAppeared->coordenadas;

			requiere(&pokemon, &coordenadas);

		break;

		case LOCALIZED_POKEMON: ;

			t_localizedPokemon_msg* mensajeLocalized = (t_localizedPokemon_msg*) mensaje_recibido;

	//		t_nombrePokemon nombre_pokemon;
	//			uint32_t cantidad_coordenadas;
	//			t_coordenadas* coordenadas;
	//		} t_localizedPokemon_msg;

			printf("Llego un localized con nombre: %s\n", mensajeLocalized->nombre_pokemon.nombre);
			 //TODO no es para appeard (planificar)


			break;

		case CAUGHT_POKEMON: ;

		t_caughtPokemon_msg* mensajeCaught = (t_caughtPokemon_msg*) mensaje_recibido;
		printf("Llego un caught al Team con Atrapado: %d\n", mensajeCaught->atrapado);

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

void enviarMensajeGetABroker(){

	t_list* objetivoTeamSinRepe = eliminarRepetidos();

	int tamanioObjTeamSinRepetidos = list_size(objetivoTeamSinRepe);


	for(int a=0; a< tamanioObjTeamSinRepetidos ; a++){
		t_nombrePokemon* pokemon = (t_nombrePokemon*) list_get(objetivoTeamSinRepe, a);
		enviarMensajeGet(pokemon);
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

void enviarMensajeGet(t_nombrePokemon* pokemon){
	t_getPokemon_msg* estructuraPokemon = malloc(sizeof(t_nombrePokemon));
	estructuraPokemon->nombre_pokemon = *pokemon;
	int socket_cliente = crear_conexion(ipBroker, puertoBroker);
	int status = enviar_mensaje(GET_POKEMON, 0, 0, estructuraPokemon, socket_cliente);

	if(status>=0){
		esperarIdGet(socket_cliente);
	}

	liberar_conexion(socket_cliente);
}

void enviarMensajeCatch(t_newPokemon* pokemon){


	t_catchPokemon_msg* estructuraPokemon = malloc(sizeof(t_catchPokemon_msg));

	estructuraPokemon->coordenadas = *(pokemon->coordenadas);
	estructuraPokemon->nombre_pokemon = *(pokemon->pokemon);
	int socket_cliente = crear_conexion(ipBroker, puertoBroker);
	int status = enviar_mensaje(CATCH_POKEMON, 0, 0, estructuraPokemon, socket_cliente);

	if(status>=0){
		esperarIdCatch(socket_cliente);
	}

	liberar_conexion(socket_cliente);
}

void inicializarListas(){
	id_mensajeGet = list_create();
	id_mensajeCatch = list_create();
	atrapados = list_create();
	objetivoTeam = list_create();
}

void esperarIdGet(int socket_cliente){
	uint32_t* id_respuesta = malloc(sizeof(uint32_t));
	*id_respuesta = recibir_id(socket_cliente);
	printf("recibi el id %d\n", *id_respuesta);
	list_add(id_mensajeGet,(void*) id_respuesta);

}

void esperarIdCatch(int socket_cliente){
	uint32_t* id_respuesta = malloc(sizeof(uint32_t));
	*id_respuesta = recibir_id(socket_cliente);
	printf("recibi el id %d\n", *id_respuesta);
	list_add(id_mensajeCatch,(void*) id_respuesta);

}

void requiere(t_nombrePokemon* pokemon, t_coordenadas* coordenadas){

	int a = list_size(pendientes);
	int j=0;

	for(int i=0; i < a; i++){

		if(!sonIguales(pokemon, list_get(pendientes, i))){
			j++;
		}
	}

	if(j!=a){
		t_newPokemon* pokemonNuevo = malloc(sizeof(t_newPokemon));
		pokemonNuevo->pokemon = pokemon;
		pokemonNuevo->coordenadas = coordenadas;
		//buscarPokemon(pokemonNuevo);hace lo que tenga que hacer --> poner a planificar al entrenador dormido o listo (con coordenadas y pokemon)
	}
}

