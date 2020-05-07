/* #ifndef cola_h
#define cola_h


typedef struct NodoHilo {
	pthread_t* hilo;
	NodoHilo* anterior;
	NodoHilo* siguiente;
} NodoHilo;

typedef struct Cola {
	NodoHilo  primero;
	NodoHilo cabeza;
	NodoHilo ultimo;
} Cola;

typedef struct EstructuraColas {
	Cola*  cola;
    Cola*  enEjecucion;
} EstructuraColas;

EstructuraColas* Construye(char* filename); //esta en cola.c

NodoHilo CrearNodo (pthread_t* hilo);

void DestruirNodo (NodoHilo hilo);

pthread_t* consultarPID (Cola* cola, long pid); //esta en cola.c

pthread_t* Consultar (Cola* cola);

void Eliminarhilo (Cola* cola, long pid);

pthread_t* DesplazarNodo (Cola* cola);

#endif //cola_h

*/
