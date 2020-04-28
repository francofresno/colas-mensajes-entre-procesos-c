#include <stdlib.h>
#include "cola.h"
#include "rutinas.h"
#include "interfaz.h"

/*
 Funcion que se encarga de crear un nodo a partir de hilos de entrada
 Entrada hilo: hilo de tipo hilo
 Salida: retorna un nodo de tipo NodoHilo que la identifica
*/
NodoHilo CrearNodo (pthread_t* hilo) {
    NodoHilo nodo = (NodoHilo *) malloc (sizeof (NodoHilo));
    nodo->hilo = hilo;
    nodo->anterior = nodo->siguiente = NULL;
    return nodo;
}

/*
 Funcion que se encarga eliminar o destruir el hilo perteneciente al nodo
 Entrada nodo: es rl nodo de tipo NodoHilo que queremos eliminar
*/
void DestruirNodo (NodoHilo nodo) {
    nodo->hilo = NULL;
    nodo->anterior = nodo->siguiente = NULL;
    free (nodo);
}

/*
 Funcion que se encarga de contruir un cola
 Salida: retorna un apuntador a una cola
// t_queue* queue_create();

/*
 Funcion que se encarga de eliminar una cola
 Entrada cola: cola
queue_destroy(t_queue*);

/*
 Funcion que se encarga de consultar un hilo a traves del PID que lo identifica
 Entrada cola: cola de prioridad del planificar de tipo Cola
 Entrada PID: identificador del hilo.
 Salida: un apuntador a hilo
*/
pthread_t* consultarPID (Cola* cola, long PID) { //si ya hay un hilo con ese id lo devuelve, sino devuelve null
    cola->cabeza = cola->primero;
    while (cola->cabeza) {
        if (cola->cabeza->hilo->PID == PID)
            return cola->cabeza->hilo;
        cola->cabeza = cola->cabeza->siguiente;
    }
    return NULL;
}

/*
 Funcion que se encarga de insertar el hilo dentro de la cola

void* queue_push(t_queue *, void *element);;

/*
 Funcion que permite consultar el primer hilo de la cola
 Salida: apuntador al primer hilo
void *queue_peek(t_queue *);

/*
 Funcion que permite eliminar al primer hilo de la cola
 Entrada cola: cola
 void * queue_pop (t_queue *);



/*
 Funcion que permite eliminar un hilo de la cola de acuerdo a su PID
 Entrada cola: cola de prioridad del planificador
 Entrada PID: identificador del hilo
*/
void Eliminarhilo (Cola* cola, long PID) {
    cola->cabeza = cola->primero;
    NodoHilo eliminado;
    while (cola->cabeza) {
        if (cola->cabeza->hilo->PID == PID){
            eliminado = cola->cabeza;
            if (cola->cabeza == cola->primero) {
                cola->primero = cola->primero->siguiente;
            } else if (cola->cabeza == cola->ultimo) {
                cola->ultimo = cola->ultimo->anterior;
            } else {
                eliminado->anterior->siguiente = eliminado->siguiente;
                eliminado->siguiente->anterior = eliminado->anterior;
            }
            DestruirNodo (eliminado);
            if (!cola->primero)
                cola->ultimo = NULL;
            else {
                cola->ultimo->siguiente = NULL;
                cola->primero->anterior = NULL;
            }
        }
        cola->cabeza = cola->cabeza->siguiente;
    }
}

/*
 Funcion que permite colocar al primer nodo de la cola al final del mismo
 y se le asigna del valor de 'E' a su estado
 Entrada cola: cola de prioridad del planificador
 Salida: apuntador al hilo desplazado
*/
pthread_t* DesplazarNodo (Cola* cola) {
    cola->ultimo->siguiente = cola->primero;
    cola->ultimo->siguiente->anterior = cola->ultimo;
    cola->ultimo = cola->ultimo->siguiente;
    cola->primero = cola->primero->siguiente;
    cola->primero->anterior = NULL;
    cola->ultimo->siguiente = NULL;

    cola->ultimo->hilo->estado = 'E';

    return cola->ultimo->hilo;
}

/*
Funcion que se encarga de construir el planificador de hilos a partir de un
 archivo de entrada, se crean las respectivas colas, con los distintos hilos
 almacenados en el archivo.
 Entrada filename: es una cadena de caracteres que contiene el nombre del archivo
         de entrada
 Salida: un apuntador al planificador de hilos
*/
EstructuraColas* Construye(char* filename) {
    EstructuraColas* s = (EstructuraColas *) malloc (sizeof (EstructuraColas));

    s->cola = queue_create();
    s->enEjecucion = NULL;

    pthread_t entrenador;

    FILE* fp = fopen (filename, "r");

    if(fp == NULL)
        msg_ErrorOpenFile();
    else
        msg_readFile();

    while (config_get_array_value(filename,"r")) {
        entrenador = pthread_create (& entrenador , NULL , hola , NULL ); //arreglar ese hola
        queue_push(& (s->cola), &entrenador); //no sabemos si es un puntero en los paramentros #tipoDeDatoProblems
    }

    msg_endReadFile ();
    fclose (fp);

    return s;
}

}
