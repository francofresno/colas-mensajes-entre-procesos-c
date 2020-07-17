/*
 * planificador.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#include "planificador.h"

pthread_mutex_t mutex_atrapados = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pendientes = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_objetivoTeam = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_entrenadores = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_listaNuevos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaReady = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaBloqueadosDeadlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaBloqueadosEsperandoMensaje = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaBloqueadosEsperandoPokemones = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listaFinalizados = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_cantidadDeadlocks = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cantidadCambiosContexto = PTHREAD_MUTEX_INITIALIZER;

void planificarCaught() {

	int cantidadCaughts1 = 0;
	pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
	if(!(list_is_empty(listaBloqueadosEsperandoMensaje))){
		int j = list_size(listaBloqueadosEsperandoMensaje);
		for(int i=0; i<j ; i++){
			t_entrenador* entrenador = list_get(listaBloqueadosEsperandoMensaje, i);

			if(entrenador->puedeAtrapar){
				entrenador->estado = READY;
				log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "llegó un caught que le permite atrapar al pokemon", "READY");

				pthread_mutex_lock(&mutex_listaReady);
				list_add(listaReady, entrenador);
				pthread_mutex_unlock(&mutex_listaReady);
				list_remove(listaBloqueadosEsperandoMensaje, i);
				cantidadCaughts1++;
			}

			if((entrenador->pokemonInstantaneo) == NULL){
				pthread_mutex_lock(&mutex_listaBloqueadosEsperandoPokemones);
				list_add(listaBloqueadosEsperandoPokemones, entrenador);
				pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoPokemones);
				list_remove(listaBloqueadosEsperandoMensaje, i);
			}
		}
	}
	pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje);

	for (int i=0; i < cantidadCaughts1; i++) {
		planificarSegun();
	}
}

void planificarSegun() {

	switch (stringACodigoAlgoritmo(algoritmoPlanificacion)) {

		case FIFO:
			planificarSegunFifo();

			break;

		case RR:
			if (planificarSegunRR()){
				planificarSegun();
			}
			break;

		case SJFCD:
			if (planificarSegunSJFConDesalojo()) {
				planificarSegun();
			}
			break;

		case SJFSD:
			planificarSegunSJFSinDesalojo();
			break;

		case ERROR_CODIGO_ALGORITMO:
			puts("Se recibio mal el codigo\n");
			break;

		default:
			puts("Error desconocido\n");
			break;

	}

}

void planificarSegunFifo() {

	int distancia;

	pthread_mutex_lock(&mutex_listaReady);

	sem_wait(&sem_planificar);

	sem_init(&sem_esperarCaught, 0, 0);

	t_entrenador* entrenador = (t_entrenador*) list_remove(listaReady, 0);

	entrenador->estado = EXEC;

	pthread_mutex_lock(&mutex_cantidadCambiosContexto);
	cantidadCambiosDeContexto+=1;
	pthread_mutex_unlock(&mutex_cantidadCambiosContexto);

	log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "fue seleccionado para ejecutar", "EXEC");

	if(entrenador->puedeAtrapar) {
		// Esto es un caught 1
		sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
		sem_post(semaforoDelEntrenador);

		sem_wait(&sem_esperarCaught);
		verificarTieneTodoLoQueQuiere(entrenador);

	} else {
		//Esto es un appeared o un localized
		sem_init(&sem_entrenadorMoviendose, 0, 0);
		sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
		sem_post(semaforoDelEntrenador);

		entrenador->misCiclosDeCPU++;

		sem_wait(&sem_entrenadorMoviendose);
		distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);

		while (distancia != 0 && distancia != -1) {
			sem_post(semaforoDelEntrenador);
			sem_wait(&sem_entrenadorMoviendose);
			distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo != NULL ? entrenador->pokemonInstantaneo->coordenadas : NULL);
			entrenador->misCiclosDeCPU++;
		}

		sem_wait(&sem_esperarCaught);
		if(entrenador->idMensajeCaught){
			entrenador->estado = BLOCKED;
			log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "se queda esperando un caught", "BLOCKED");

			pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
			list_add(listaBloqueadosEsperandoMensaje, entrenador);
			pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje);

		} else{
			verificarTieneTodoLoQueQuiere(entrenador);
		}
	}
	sem_post(&sem_planificar);

	pthread_mutex_unlock(&mutex_listaReady);

	chequearDeadlock(FIFO);

	chequearSiEstaDisponible(entrenador);

}

void planificarSegunSJFSinDesalojo(){

	int distancia;

	sem_wait(&sem_planificar);

	pthread_mutex_lock(&mutex_listaReady);

	ordenarListaPorEstimacion(listaReady);

	sem_init(&sem_esperarCaught, 0, 0);

	t_entrenador* entrenador = (t_entrenador*) list_remove(listaReady, 0);

	//Cambia los datos del entrenador para cuando calcule su prox rafaga
	double estimadoProxRafaga = alfa * (entrenador->rafagaAnteriorReal) + (1-alfa)*(entrenador->estimacionInicial);
	entrenador->estimacionInicial = estimadoProxRafaga;

	entrenador->estado = EXEC;

	pthread_mutex_lock(&mutex_cantidadCambiosContexto);
	cantidadCambiosDeContexto+=1;
	pthread_mutex_unlock(&mutex_cantidadCambiosContexto);

	log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "fue seleccionado para ejecutar", "EXEC");

	if(entrenador->puedeAtrapar) {
		// Esto es un caught 1
		sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
		sem_post(semaforoDelEntrenador);

		sem_wait(&sem_esperarCaught);
		verificarTieneTodoLoQueQuiere(entrenador);

	} else {
		sem_init(&sem_entrenadorMoviendose, 0, 0);
		//Esto es un appeared o un localized
		sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);

		//Cambia los datos del entrenador para cuando calcule su prox rafaga
		entrenador->rafagaAnteriorReal = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);

		sem_post(semaforoDelEntrenador);

		entrenador->misCiclosDeCPU++;

		sem_wait(&sem_entrenadorMoviendose);
		distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);

		while (distancia != 0 && distancia != -1) {
			sem_post(semaforoDelEntrenador);
			sem_wait(&sem_entrenadorMoviendose);
			distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo != NULL ? entrenador->pokemonInstantaneo->coordenadas : NULL);
			entrenador->misCiclosDeCPU++;
		}

		sem_wait(&sem_esperarCaught);
		if(entrenador->idMensajeCaught){
			entrenador->estado = BLOCKED;
			log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "se queda esperando un caught", "BLOCKED");

			pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
			list_add(listaBloqueadosEsperandoMensaje, entrenador);
			pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje);

		} else{
			verificarTieneTodoLoQueQuiere(entrenador);
		}
	}
	sem_post(&sem_planificar);


	pthread_mutex_unlock(&mutex_listaReady);

	chequearDeadlock(SJFSD);

	chequearSiEstaDisponible(entrenador);

}

int planificarSegunRR(){

	int distancia;

	pthread_mutex_lock(&mutex_listaReady);

	sem_wait(&sem_planificar);

	sem_init(&sem_esperarCaught, 0, 0);

	t_entrenador* entrenador = (t_entrenador*) list_remove(listaReady, 0);

	entrenador->estado = EXEC;

	pthread_mutex_lock(&mutex_cantidadCambiosContexto);
	cantidadCambiosDeContexto+=1;
	pthread_mutex_unlock(&mutex_cantidadCambiosContexto);

	log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "fue seleccionado para ejecutar", "EXEC");

	if(entrenador->puedeAtrapar) {
		// Esto es un caught 1
		sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
		sem_post(semaforoDelEntrenador);

		sem_wait(&sem_esperarCaught);
		verificarTieneTodoLoQueQuiere(entrenador);

	} else {
		sem_init(&sem_entrenadorMoviendose, 0, 0);
		//Esto es un appeared o un localized
		sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
		sem_post(semaforoDelEntrenador);

		entrenador->misCiclosDeCPU++;

		sem_wait(&sem_entrenadorMoviendose);
		distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);

		entrenador->quantumDisponible -=1;

		while (((distancia != 0) && (entrenador->quantumDisponible)>0) && (distancia != -1)) {
			sem_post(semaforoDelEntrenador);
			sem_wait(&sem_entrenadorMoviendose);
			distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo != NULL ? entrenador->pokemonInstantaneo->coordenadas : NULL);
			entrenador->misCiclosDeCPU++;
			entrenador->quantumDisponible -= 1;
		}

		if(((entrenador->quantumDisponible)==0) && (!llegoAlObjetivoPokemon(entrenador))){
			pthread_mutex_lock(&mutex_listaReady);
			list_add(listaReady, entrenador);
			entrenador->estado = READY;
			pthread_mutex_unlock(&mutex_listaReady);
			entrenador->quantumDisponible = quantum;
			return 1;
		} else{
			sem_wait(&sem_esperarCaught);
			if(entrenador->idMensajeCaught){
				entrenador->estado = BLOCKED;
				log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "se queda esperando un caught", "BLOCKED");

				pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
				list_add(listaBloqueadosEsperandoMensaje, entrenador);
				pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje);

			} else{
				verificarTieneTodoLoQueQuiere(entrenador);
			}
		}
		sem_post(&sem_planificar);
		}


	pthread_mutex_unlock(&mutex_listaReady);

	chequearDeadlock(RR);

	chequearSiEstaDisponible(entrenador);

	return 0;
}

int planificarSegunSJFConDesalojo(){

	int distancia;

	sem_wait(&sem_planificar);

	pthread_mutex_lock(&mutex_listaReady);

	ordenarListaPorEstimacion(listaReady);

	sem_init(&sem_esperarCaught, 0, 0);

	t_entrenador* entrenador = (t_entrenador*) list_remove(listaReady, 0);

	//Cambia los datos del entrenador para cuando calcule su prox rafaga
	double estimadoProxRafaga = alfa * (entrenador->rafagaAnteriorReal) + (1-alfa)*(entrenador->estimacionInicial);
	entrenador->estimacionInicial = estimadoProxRafaga;

	entrenador->estado = EXEC;

	pthread_mutex_lock(&mutex_cantidadCambiosContexto);
	cantidadCambiosDeContexto+=1;
	pthread_mutex_unlock(&mutex_cantidadCambiosContexto);

	log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "fue seleccionado para ejecutar", "EXEC");

	if(entrenador->puedeAtrapar) {
		// Esto es un caught 1
		sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
		sem_post(semaforoDelEntrenador);

		sem_wait(&sem_esperarCaught);
		verificarTieneTodoLoQueQuiere(entrenador);

	} else {
		sem_init(&sem_entrenadorMoviendose, 0, 0);
		//Esto es un appeared o un localized
		sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);

		//Cambia los datos del entrenador para cuando calcule su prox rafaga
		entrenador->rafagaAnteriorReal = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);

		sem_post(semaforoDelEntrenador);

		entrenador->misCiclosDeCPU++;

		sem_wait(&sem_entrenadorMoviendose);
		distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);

		int tamanio_ready_actual = list_size(listaReady) - 1;
		while (distancia != 0 && distancia != -1) {

			if (tamanio_ready_actual < list_size(listaReady)) {
					int distanciaQueLeQueda = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);
					entrenador->rafagaAnteriorReal = entrenador->rafagaAnteriorReal - distanciaQueLeQueda;
					entrenador->estado = READY;

					list_add(listaReady, entrenador);
					pthread_mutex_unlock(&mutex_listaReady);
					sem_post(&sem_planificar);
					return 1;
			}

			sem_post(semaforoDelEntrenador);
			sem_wait(&sem_entrenadorMoviendose);
			distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo != NULL ? entrenador->pokemonInstantaneo->coordenadas : NULL);
			entrenador->misCiclosDeCPU++;
		}

		sem_wait(&sem_entrenadorMoviendose);
		distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);

		while (distancia != 0 && distancia != -1) {
			sem_post(semaforoDelEntrenador);
			sem_wait(&sem_entrenadorMoviendose);
			distancia = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo != NULL ? entrenador->pokemonInstantaneo->coordenadas : NULL);
			entrenador->misCiclosDeCPU++;
		}







		sem_wait(&sem_esperarCaught);
		if(entrenador->idMensajeCaught){
			entrenador->estado = BLOCKED;
			log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "se queda esperando un caught", "BLOCKED");

			pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
			list_add(listaBloqueadosEsperandoMensaje, entrenador);
			pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje);

		} else{
			verificarTieneTodoLoQueQuiere(entrenador);
		}
	}
	sem_post(&sem_planificar);


	pthread_mutex_unlock(&mutex_listaReady);

	chequearDeadlock(SJFCD);

	chequearSiEstaDisponible(entrenador);

	return 0;
}

void chequearSiEstaDisponible(t_entrenador* entrenador){

	bool estaEnLista(void* elemento){
			t_entrenador* otroEntrenador = (t_entrenador*) elemento;
			return otroEntrenador->id_entrenador == entrenador->id_entrenador;
		}

		pthread_mutex_lock(&mutex_listaBloqueadosEsperandoMensaje);
		bool estaEnListaCatch = list_any_satisfy(listaBloqueadosEsperandoMensaje, estaEnLista);
		pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoMensaje);

		pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
		bool estaEnListaDeadlock = list_any_satisfy(listaBloqueadosDeadlock, estaEnLista);
		pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);

		if(!estaEnListaCatch &&  !estaEnListaDeadlock){
			sem_post(&sem_buscarEntrenadorMasCercano);
		}
}

void chequearDeadlock(int algoritmo) {
	pthread_mutex_lock(&mutex_objetivoTeam);
	int tamanioObjetivoTeam = list_size(objetivoTeam);
	pthread_mutex_unlock(&mutex_objetivoTeam);

	pthread_mutex_lock(&mutex_atrapados);
	int tamanioAtrapados = list_size(atrapados);
	pthread_mutex_unlock(&mutex_atrapados);

	if(tamanioObjetivoTeam == tamanioAtrapados){ //o cumplio el objetivo o hay deadlock

		pthread_mutex_lock(&mutex_entrenadores);
		int tamanioEntrenadores = list_size(entrenadores);
		pthread_mutex_unlock(&mutex_entrenadores);

		pthread_mutex_lock(&mutex_listaFinalizados);
		int tamanioFinalizados = list_size(listaFinalizados);
		pthread_mutex_unlock(&mutex_listaFinalizados);

		if(tamanioEntrenadores == tamanioFinalizados){

			int ciclosCPUTotales = 0;
			char* cantidadCiclosCPUPorEntrenador =  string_new();

			for(int i=0; i< list_size(entrenadores);i++){

				t_entrenador* entrenador = list_get(entrenadores, i);
				ciclosCPUTotales += entrenador->misCiclosDeCPU;


				string_append_with_format(&cantidadCiclosCPUPorEntrenador, " el entrenador %d consumio: ", i);
				string_append_with_format(&cantidadCiclosCPUPorEntrenador, "%d", entrenador->misCiclosDeCPU);
				string_append(&cantidadCiclosCPUPorEntrenador, ";");

			}

			log_resultado_team("el team cumplió el objetivo", ciclosCPUTotales, cantidadCambiosDeContexto, cantidadCiclosCPUPorEntrenador, cantidadDeadlocksResueltos);//TODO return exit success?

		} else {
			int distancia;
			int tamanioDeadlock;

			pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
			switch (algoritmo) {

				case FIFO:
					log_inicio_algoritmo_deadlock();

					tamanioDeadlock = list_size(listaBloqueadosDeadlock);
					for (int b = 0; b < tamanioDeadlock; b++) {

						t_entrenador* entrenador = (t_entrenador*) list_remove(listaBloqueadosDeadlock, 0);
						entrenador->estado = EXEC;

						pthread_mutex_lock(&mutex_cantidadCambiosContexto);
						cantidadCambiosDeContexto+=1;
						pthread_mutex_unlock(&mutex_cantidadCambiosContexto);

						log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "va a intercambiar pokemones con otro entrenador", "EXEC");

						t_entrenador* entrenadorConQuienIntercambiar = elegirConQuienIntercambiar(entrenador);

						sacarEntrenadorDeLista(entrenadorConQuienIntercambiar, listaBloqueadosDeadlock);

						sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
						sem_post(semaforoDelEntrenador);

						entrenador->misCiclosDeCPU++;

						distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
						int distanciaAnterior = distancia;

						while (distancia != 0) {
							if (distancia != distanciaAnterior) {
								sem_post(semaforoDelEntrenador);
								entrenador->misCiclosDeCPU++;
							}
							distanciaAnterior = distancia;
							distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
						}

						entrenador->misCiclosDeCPU = entrenador->misCiclosDeCPU +5;

						pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);
						verificarTieneTodoLoQueQuiere(entrenador);
						verificarTieneTodoLoQueQuiere(entrenadorConQuienIntercambiar);

						pthread_mutex_lock(&mutex_cantidadDeadlocks);
						if(entrenador->estado == BLOCKED){
							if(entrenadorConQuienIntercambiar->estado == BLOCKED){
								log_fin_algoritmo_deadlock("ambos entrenadores siguen en deadlock.\n");
							} else{
								cantidadDeadlocksResueltos+=1;
								log_fin_algoritmo_deadlock("el entrenador elegido sigue en deadlock, sin embargo el elegido para intercambiar finalizó.\n");
							}

						} else{
							cantidadDeadlocksResueltos+=1;
							if(entrenadorConQuienIntercambiar->estado == BLOCKED){
								log_fin_algoritmo_deadlock("el entrenador que ejecutó pasó a estado finalizado, sin embargo el entrenador con el que intercambió sigue en deadlock.\n");
							} else{
								cantidadDeadlocksResueltos+=1;
								log_fin_algoritmo_deadlock("ambos entrenadores finalizaron, consiguiendo los pokemones que desean.\n");
							}
						}
						pthread_mutex_unlock(&mutex_cantidadDeadlocks);

					}

					break;

				case RR:
					log_inicio_algoritmo_deadlock();

					tamanioDeadlock = list_size(listaBloqueadosDeadlock);
					for (int b = 0; b < tamanioDeadlock; b++) {

						t_entrenador* entrenador = (t_entrenador*) list_remove(listaBloqueadosDeadlock, 0);
						entrenador->estado = EXEC;

						pthread_mutex_lock(&mutex_cantidadCambiosContexto);
						cantidadCambiosDeContexto+=1;
						pthread_mutex_unlock(&mutex_cantidadCambiosContexto);

						log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "va a intercambiar pokemones con otro entrenador", "EXEC");

						t_entrenador* entrenadorConQuienIntercambiar = elegirConQuienIntercambiar(entrenador);

						distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);

						if(distancia!=0){
							entrenador->quantumIntercambio = 5;
						}

						sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);
						sem_post(semaforoDelEntrenador);

						entrenador->misCiclosDeCPU++;

						distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
						int distanciaAnterior = distancia;

						entrenador->quantumDisponible-=1;

						while ((distancia != 0) && ((entrenador->quantumDisponible)>0)) {

							if (distancia != distanciaAnterior) {
								sem_post(semaforoDelEntrenador);
								entrenador->misCiclosDeCPU++;
								entrenador->quantumDisponible-=1;
							}
							distanciaAnterior = distancia;
							distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
						}

						if(entrenador->quantumIntercambio){
							list_add(listaBloqueadosDeadlock, entrenador);
							entrenador->estado = BLOCKED;
							entrenador->quantumDisponible = quantum;
						} else{

							entrenador->quantumIntercambio = 5;

							sacarEntrenadorDeLista(entrenadorConQuienIntercambiar, listaBloqueadosDeadlock);

							pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);
							verificarTieneTodoLoQueQuiere(entrenador);
							verificarTieneTodoLoQueQuiere(entrenadorConQuienIntercambiar);

							pthread_mutex_lock(&mutex_cantidadDeadlocks);
							if(entrenador->estado == BLOCKED){
								if(entrenadorConQuienIntercambiar->estado == BLOCKED){
									log_fin_algoritmo_deadlock("ambos entrenadores siguen en deadlock.\n");
								} else{
									cantidadDeadlocksResueltos+=1;
									log_fin_algoritmo_deadlock("el entrenador elegido sigue en deadlock, sin embargo el elegido para intercambiar finalizó.\n");
								}

							} else{
								cantidadDeadlocksResueltos+=1;
								if(entrenadorConQuienIntercambiar->estado == BLOCKED){
									log_fin_algoritmo_deadlock("el entrenador que ejecutó pasó a estado finalizado, sin embargo el entrenador con el que intercambió sigue en deadlock.\n");
								} else{
									cantidadDeadlocksResueltos+=1;
									log_fin_algoritmo_deadlock("ambos entrenadores finalizaron, consiguiendo los pokemones que desean.\n");
								}
							}
							pthread_mutex_unlock(&mutex_cantidadDeadlocks);

						}

					}


					break;

				case SJFCD:

					log_inicio_algoritmo_deadlock();

					ordenarListaPorEstimacion(listaBloqueadosDeadlock);

					tamanioDeadlock = list_size(listaBloqueadosDeadlock);
					for (int b = 0; b < tamanioDeadlock; b++) {

						t_entrenador* entrenador = (t_entrenador*) list_remove(listaBloqueadosDeadlock, 0);
						entrenador->estado = EXEC;

						//Cambia los datos del entrenador para cuando calcule su prox rafaga
						double estimadoProxRafaga = alfa * (entrenador->rafagaAnteriorReal) + (1-alfa)*(entrenador->estimacionInicial);
						entrenador->estimacionInicial = estimadoProxRafaga;

						pthread_mutex_lock(&mutex_cantidadCambiosContexto);
						cantidadCambiosDeContexto+=1;
						pthread_mutex_unlock(&mutex_cantidadCambiosContexto);

						log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "va a intercambiar pokemones con otro entrenador", "EXEC");

						t_entrenador* entrenadorConQuienIntercambiar = elegirConQuienIntercambiar(entrenador);

						sacarEntrenadorDeLista(entrenadorConQuienIntercambiar, listaBloqueadosDeadlock);

						sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);

						//Cambia los datos del entrenador para cuando calcule su prox rafaga
						entrenador->rafagaAnteriorReal = 5 + distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);

						sem_post(semaforoDelEntrenador);

						entrenador->misCiclosDeCPU++;

						distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
						int distanciaAnterior = distancia;

						int fueDesalojado = 0;
						pthread_mutex_lock(&mutex_listaReady);
						int tamanio_ready_actual = list_size(listaReady) - 1;
						while (distancia != 0) {
							if (distancia != distanciaAnterior) {
								if (tamanio_ready_actual < list_size(listaReady)) {
									int distanciaQueLeQueda = distanciaA(entrenador->coordenadas, entrenador->pokemonInstantaneo->coordenadas);
									entrenador->rafagaAnteriorReal = entrenador->rafagaAnteriorReal - distanciaQueLeQueda - 5;
									entrenador->estado = BLOCKED;

									list_add(listaBloqueadosDeadlock, entrenador);
									list_add(listaBloqueadosDeadlock, entrenadorConQuienIntercambiar);
									sem_post(&sem_planificar);
									fueDesalojado = 1;
									break;
								}
								sem_post(semaforoDelEntrenador);
								entrenador->misCiclosDeCPU++;
							}
							distanciaAnterior = distancia;
							distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
						}
						pthread_mutex_unlock(&mutex_listaReady);

						if (!fueDesalojado) {
							entrenador->misCiclosDeCPU = entrenador->misCiclosDeCPU + 5;

							pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);
							verificarTieneTodoLoQueQuiere(entrenador);
							verificarTieneTodoLoQueQuiere(entrenadorConQuienIntercambiar);

							pthread_mutex_lock(&mutex_cantidadDeadlocks);
							if(entrenador->estado == BLOCKED){
								if(entrenadorConQuienIntercambiar->estado == BLOCKED){
									log_fin_algoritmo_deadlock("ambos entrenadores siguen en deadlock.\n");
								} else{
									cantidadDeadlocksResueltos+=1;
									log_fin_algoritmo_deadlock("el entrenador elegido sigue en deadlock, sin embargo el elegido para intercambiar finalizó.\n");
								}

							} else{
								cantidadDeadlocksResueltos+=1;
								if(entrenadorConQuienIntercambiar->estado == BLOCKED){
									log_fin_algoritmo_deadlock("el entrenador que ejecutó pasó a estado finalizado, sin embargo el entrenador con el que intercambió sigue en deadlock.\n");
								} else{
									cantidadDeadlocksResueltos+=1;
									log_fin_algoritmo_deadlock("ambos entrenadores finalizaron, consiguiendo los pokemones que desean.\n");
								}
							}
							pthread_mutex_unlock(&mutex_cantidadDeadlocks);
						} else {
							pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);
						}

					}

					break;
				case SJFSD:

					log_inicio_algoritmo_deadlock();

					ordenarListaPorEstimacion(listaBloqueadosDeadlock);

					tamanioDeadlock = list_size(listaBloqueadosDeadlock);
					for (int b = 0; b < tamanioDeadlock; b++) {

						t_entrenador* entrenador = (t_entrenador*) list_remove(listaBloqueadosDeadlock, 0);
						entrenador->estado = EXEC;

						//Cambia los datos del entrenador para cuando calcule su prox rafaga
						double estimadoProxRafaga = alfa * (entrenador->rafagaAnteriorReal) + (1-alfa)*(entrenador->estimacionInicial);
						entrenador->estimacionInicial = estimadoProxRafaga;

						pthread_mutex_lock(&mutex_cantidadCambiosContexto);
						cantidadCambiosDeContexto+=1;
						pthread_mutex_unlock(&mutex_cantidadCambiosContexto);

						log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "va a intercambiar pokemones con otro entrenador", "EXEC");

						t_entrenador* entrenadorConQuienIntercambiar = elegirConQuienIntercambiar(entrenador);

						sacarEntrenadorDeLista(entrenadorConQuienIntercambiar, listaBloqueadosDeadlock);

						sem_t* semaforoDelEntrenador = (sem_t*) list_get(sem_entrenadores_ejecutar, entrenador->id_entrenador);

						//Cambia los datos del entrenador para cuando calcule su prox rafaga
						entrenador->rafagaAnteriorReal = 5 + distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);

						sem_post(semaforoDelEntrenador);

						entrenador->misCiclosDeCPU++;

						distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
						int distanciaAnterior = distancia;

						while (distancia != 0) {
							if (distancia != distanciaAnterior) {
								sem_post(semaforoDelEntrenador);
								entrenador->misCiclosDeCPU++;
							}
							distanciaAnterior = distancia;
							distancia = distanciaA(entrenador->coordenadas, entrenadorConQuienIntercambiar->coordenadas);
						}

						entrenador->misCiclosDeCPU = entrenador->misCiclosDeCPU +5;

						pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);
						verificarTieneTodoLoQueQuiere(entrenador);
						verificarTieneTodoLoQueQuiere(entrenadorConQuienIntercambiar);

						pthread_mutex_lock(&mutex_cantidadDeadlocks);
						if(entrenador->estado == BLOCKED){
							if(entrenadorConQuienIntercambiar->estado == BLOCKED){
								log_fin_algoritmo_deadlock("ambos entrenadores siguen en deadlock.\n");
							} else{
								cantidadDeadlocksResueltos+=1;
								log_fin_algoritmo_deadlock("el entrenador elegido sigue en deadlock, sin embargo el elegido para intercambiar finalizó.\n");
							}

						} else{
							cantidadDeadlocksResueltos+=1;
							if(entrenadorConQuienIntercambiar->estado == BLOCKED){
								log_fin_algoritmo_deadlock("el entrenador que ejecutó pasó a estado finalizado, sin embargo el entrenador con el que intercambió sigue en deadlock.\n");
							} else{
								cantidadDeadlocksResueltos+=1;
								log_fin_algoritmo_deadlock("ambos entrenadores finalizaron, consiguiendo los pokemones que desean.\n");
							}
						}
						pthread_mutex_unlock(&mutex_cantidadDeadlocks);

					}

					break;

				case ERROR_CODIGO_ALGORITMO:

					puts("Se recibio mal el codigo\n");
					break;

				default:

					puts("Error desconocido\n");

					break;

				}

		}
	}
}

void ordenarListaPorEstimacion(t_list* list) {

	bool ordenarMenorCicloDeCPU(void* elemento1, void* elemento2){
		t_entrenador* entrenador1 = (t_entrenador*) elemento1;
		t_entrenador* entrenador2 = (t_entrenador*) elemento2;

		double estimadoProxRafaga1 = alfa * (entrenador1->rafagaAnteriorReal) + (1-alfa)*(entrenador1->estimacionInicial);
		double estimadoProxRafaga2 = alfa * (entrenador2->rafagaAnteriorReal) + (1-alfa)*(entrenador2->estimacionInicial);

		return estimadoProxRafaga1 <= estimadoProxRafaga2;
	}
	list_sort(list, ordenarMenorCicloDeCPU);
}

int distanciaA(t_coordenadas* desde, t_coordenadas* hasta){

	if (desde == NULL || hasta == NULL) {
		return -1;
	}

	int distanciaX = abs(desde->posX - hasta->posX);
	int distanciaY = abs(desde->posY - hasta->posY);

	return distanciaX + distanciaY;
}

int tieneTodoLoQueQuiere(t_entrenador* entrenador){

	t_list* listaTodoLoQueQuiere = list_duplicate(entrenador->pokemonesQueQuiere);
	t_list* listaTodoLoQuePosee = list_duplicate(entrenador->pokemonesQuePosee);
	t_list* diferencia = list_create();

	diferenciaYCargarLista(listaTodoLoQueQuiere, listaTodoLoQuePosee, diferencia);

	list_destroy(listaTodoLoQueQuiere);
	list_destroy(listaTodoLoQuePosee);

	return list_is_empty(diferencia);

}

void diferenciaYCargarLista(t_list* listaA, t_list* listaB, t_list* listaACargar){ 		//listaGrande A lista chica B

	int a = list_size(listaA);

	for(int i=0; i < a; i++){

		int b = list_size(listaB);
		int j=0;

		while((j < b) && (!sonIguales(list_get(listaB,j), list_get(listaA, i)))){
			j++;
		}

		if(j==b){
			list_add(listaACargar, list_get(listaA, i));
		}else{
			list_remove(listaB, j);
		}

	}
}

int sonIguales(t_nombrePokemon* pokemon1, t_nombrePokemon* pokemon2){
	return strcmp(pokemon1->nombre, pokemon2->nombre) == 0;
}

algoritmo_code stringACodigoAlgoritmo(const char* string) {
	for (int i = 0;
			i < sizeof(conversionAlgoritmo) / sizeof(conversionAlgoritmo[0]);
			i++) {
		if (!strcmp(string, conversionAlgoritmo[i].str))
			return conversionAlgoritmo[i].codigo_algoritmo;
	}
	return ERROR_CODIGO_ALGORITMO;
}

void inicializarListasDeEstados(){

	listaNuevos = list_create();
	listaReady = list_create();
	listaBloqueadosDeadlock= list_create();
	listaBloqueadosEsperandoMensaje= list_create();
	listaBloqueadosEsperandoPokemones = list_create();
	listaFinalizados = list_create();

}

void verificarTieneTodoLoQueQuiere(t_entrenador* entrenador){
	if(entrenador->cantidad_pokemons == list_size(entrenador->pokemonesQueQuiere)){
		if(tieneTodoLoQueQuiere(entrenador)){
			entrenador->estado = FINISHED;
			log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "tiene todo lo que quiere", "FINISHED"); //las wachas

			pthread_mutex_lock(&mutex_listaFinalizados);
			list_add(listaFinalizados, entrenador);
			pthread_mutex_unlock(&mutex_listaFinalizados);
		} else{
			entrenador->estado = BLOCKED;
			log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "entró en deadlock", "BLOCKED");

			pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
			list_add(listaBloqueadosDeadlock, entrenador);
			pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);
		}
	}else{
		entrenador->estado = BLOCKED;
		log_entrenador_cambio_de_cola_planificacion(entrenador->id_entrenador, "no tiene todos los pokemones que quiere", "BLOCKED");

		pthread_mutex_lock(&mutex_listaBloqueadosEsperandoPokemones);
		list_add(listaBloqueadosEsperandoPokemones, entrenador);
		pthread_mutex_unlock(&mutex_listaBloqueadosEsperandoPokemones);
	}
}

void sacarEntrenadorDeLista(t_entrenador* entrenador, t_list* lista){
	int a = list_size(lista);
	for(int i=0; i<a ; i++){
		t_entrenador* entrenadorDeLista = list_get(lista, i);
		if(entrenador->id_entrenador == entrenadorDeLista->id_entrenador){
			list_remove(lista, i);
			break;
		}
	}
}

t_entrenador* elegirConQuienIntercambiar(t_entrenador* entrenador){

	t_list* listaQuiere1 = list_duplicate(entrenador->pokemonesQueQuiere);
	t_list* listaPosee1 = list_duplicate(entrenador->pokemonesQuePosee);

	t_list* leFaltanParaObj1 = list_create();

	diferenciaYCargarLista(listaQuiere1, listaPosee1, leFaltanParaObj1);

	list_destroy(listaQuiere1);
	list_destroy(listaPosee1);

	t_list* tienePeroNoQuiere2 = list_create();
	t_list* pokemonesDe2QueQuiere1 = list_create();

	t_list* sublistasPosiblesProveedoresDePokemon = list_create();

	pthread_mutex_lock(&mutex_listaBloqueadosDeadlock);
	int tamanioDeadlock = list_size(listaBloqueadosDeadlock);

	for(int a=0; a< tamanioDeadlock; a++){

		t_entrenador* entrenador2 = list_get(listaBloqueadosDeadlock, a);

		t_list* listaQuiere2 = list_duplicate(entrenador2->pokemonesQueQuiere);
		t_list* listaPosee2 = list_duplicate(entrenador2->pokemonesQuePosee);

		diferenciaYCargarLista(listaPosee2, listaQuiere2, tienePeroNoQuiere2);

		list_destroy(listaQuiere2);
		list_destroy(listaPosee2);

		diferenciaYCargarLista(leFaltanParaObj1, tienePeroNoQuiere2, pokemonesDe2QueQuiere1);

		list_destroy(leFaltanParaObj1);
		list_destroy(tienePeroNoQuiere2);


		if(!list_is_empty(pokemonesDe2QueQuiere1)){
			if(tengoAlgunPokemonQueQuiere2(entrenador, entrenador2)){
				list_destroy(pokemonesDe2QueQuiere1);
				return entrenador2;
			}
			list_add(sublistasPosiblesProveedoresDePokemon, entrenador2);
		}

		list_destroy(pokemonesDe2QueQuiere1);
	}

	pthread_mutex_unlock(&mutex_listaBloqueadosDeadlock);

	t_entrenador* entrenadorProveedor = list_get(sublistasPosiblesProveedoresDePokemon, 0);
	list_destroy(sublistasPosiblesProveedoresDePokemon);
	return entrenadorProveedor;
}

int tengoAlgunPokemonQueQuiere2(t_entrenador* entrenador1,t_entrenador* entrenador2){

	t_list* listaQuiere1 = list_duplicate(entrenador1->pokemonesQueQuiere);
	t_list* listaPosee1 = list_duplicate(entrenador1->pokemonesQuePosee);

	t_list* tienePeroNoQuiere1 = list_create();

	diferenciaYCargarLista(listaPosee1, listaQuiere1, tienePeroNoQuiere1);

	t_list* listaQuiere2 = list_duplicate(entrenador2->pokemonesQueQuiere);
	t_list* listaPosee2 = list_duplicate(entrenador2->pokemonesQuePosee);

	t_list* leFaltanParaObj2 = list_create();

	diferenciaYCargarLista(listaQuiere2, listaPosee2, leFaltanParaObj2);

	int tamanioFaltaParaObj2 = list_size(leFaltanParaObj2);
	int tamanioTienePeroNoQuiere1 = list_size(tienePeroNoQuiere1);

	list_destroy(listaQuiere1);
	list_destroy(listaPosee1);
	list_destroy(listaQuiere2);
	list_destroy(listaPosee2);

	for(int a=0; a< tamanioFaltaParaObj2; a++){

		for(int b=0; b<tamanioTienePeroNoQuiere1; b++){

			if(sonIguales(list_get(leFaltanParaObj2, a), list_get(tienePeroNoQuiere1, b))){
				list_destroy(leFaltanParaObj2);
				list_destroy(tienePeroNoQuiere1);
				return true;
			}
		}

	}

	list_destroy(leFaltanParaObj2);
	list_destroy(tienePeroNoQuiere1);

	return false;
}

int llegoAlObjetivoPokemon(t_entrenador* entrenador){

	uint32_t posicionXEntrenador = entrenador->coordenadas->posX;
	uint32_t posicionYEntrenador = entrenador->coordenadas->posY;

	uint32_t posicionXPokemon = entrenador->pokemonInstantaneo->coordenadas->posX;
	uint32_t posicionYPokemon = entrenador->pokemonInstantaneo->coordenadas->posY;

	return (posicionXEntrenador == posicionXPokemon) && (posicionYEntrenador == posicionYPokemon);
}

int llegoAlObjetivoEntrenador(t_entrenador* entrenador1, t_entrenador* entrenador2){

	uint32_t posicionXEntrenador1 = entrenador1->coordenadas->posX;
	uint32_t posicionYEntrenador1 = entrenador1->coordenadas->posY;

	uint32_t posicionXEntrenador2 = entrenador2->coordenadas->posX;
	uint32_t posicionYEntrenador2 = entrenador2->coordenadas->posY;

	return (posicionXEntrenador1 == posicionXEntrenador2) && (posicionYEntrenador1 == posicionYEntrenador2);
}
