#include "interfaz.h"
#include "cola.h"

/*


/*
 Funcion que imprime las opciones del eliminar hilo(hilo cualquiera
 o hilo en ejecucion)
 Entrada s: apuntador al planificador de hilos
 Entrada salida: nombre del archivo de salida
*/
void menuInterno(EstructuraColas* s, char* salida){

    int opcion;
    long pid;
    short prio;

    printf("\n1.- Eliminar hilo");
    printf("\n2.- Elminar ultimo hilo ejecutado");
    printf("\n0.- Regresar al menu principal");
    printf("\n\nIntroduzca una opcion\n>>> ");

    scanf("%d", &opcion);
    fflush(stdin);

    switch (opcion) {

        case 1:
            printf("Introduzca el PID del hilo a eliminar: \n>>> ");
            fflush(stdin);
            scanf("%ld", &pid);

            printf("Marque el numero correspondiente a la cola de prioridad del hilo a eliminar:\n\n0.- Cola 0. \n1.- Cola 1. \n2.- Cola 2. \n3.- Cola 3. \n4.- Cola 4. \n5.- Cola 5.\n>>> ");
            fflush(stdin);
            scanf("%hd", &prio);

            Elimhilo (s, pid, prio);
            s->enEjecucion = NULL;

            printf("\n-----> Eliminacion completada.\n");

            menu(s, salida);
            break;

        case 2:
            if (s->enEjecucion) {
                ElimHiloEjecutando(s);
                printf("\n-----> Eliminación completada.");
            }
            else
                printf("\nOperacion cancelada: no hay hilos en ejecucion.");

            menu(s, salida);
            break;

        case 0:
            printf("\nDevuelta al menu principal.");
            menu(s, salida);
            break;

        default:
            printf("\nNo ha introducido una opcion valida. Vuelva a intentarlo.");
            menuInterno(s, salida);
            break;
    }
}
/*
 Funcion que permite buscar todos los hilos dentro de las colas que conforman
 el planificador de hilos para e imprimir todos sus atributos por pantalla
 Entrada s: puntero al planificador de hilos
*/

void Imprime(EstructuraColas *s){
    pthread_t* hilo;
    s->cola->cabeza = s->cola->primero;

    while (s->cola->cabeza) {
        hilo = s->cola->cabeza->hilo;
        Imprimehilo(hilo);
        s->cola->cabeza = s->cola->cabeza->siguiente;
    }


}

/*
 Funcion que permite imprimir todos los atributos de un hilo
 Entrada hilo: apuntador a un hilo
*/
void Imprimehilo (pthread_t* hilo) {
    printf("", hilo->ID, hilo->coordenadas, hilo->pokemones, hilo->cantidad_pokemons); //ver tipo de datos del printf
}

/*
 Funcion que permite mostrar en pantalla un mensaje de insercion exitosa
*/
void msg_Insercion () {
    printf("\n-----> Insercion completada\n");
}

/*
 Funcion que permite mostrar en pantalla una serie de mensajes sobre error de
 insercion del hilo
 Entrada p: apuntador a un hilo
*/
void msg_ErrorInsertar (pthread_t* p) {
    Imprimehilo(p);
    printf ("\nOperacion fallida: el hilo no cumple con el formato admitido: ");
    printf ("\nPID - estado - prioridad - time - comando");
    printf ("\nPID: es un entero.");
    printf ("\nestado: es un caracter, inicialmente es 'L'.");
    printf ("\nprioridad: es un entero del 0 al 5.");
    printf ("\ntime: es un numero real.");
    printf ("\ncomando: es una cadena de caracteres.");
}

/*
 Funcion que permite mostrar en pantalla un mensaje de error de incongruencias
 de PIDs
 Entrada p: apuntador a un hilo
*/
void msg_errorInsertPID (pthread_t* p) {
    printf ("\nOperacion cancelada: el siguiente hilo tiene el mismo PID.");
    Imprimehilo(p);
    printf ("\nUtilice un PID diferente.");
}

/*
 Funcion que permite mostrar en pantalla un mensaje de error de eliminacion
*/
void msg_ErrorElim () {
    printf ("\nOperacion fallida: el hilo ha eliminar no existe.");
}

/*
 Funcion que permite mostrar en pantalla un mensaje de error de ejecucion cuando ya un hilo
 se encuentra en ejecucion
*/
void msg_Busy () {
    printf("\nOperacion cancelada: hay un hilo en ejecucion. Detenga el hilo y vuelva a intentarlo.");
}

/*
 Funcion que muestra en pantalla un mensaje que dice que el archivo se ha empezadoa a leer
 de manera exitosa
*/
void msg_readFile () {
    printf("\nInicio lectura de archivo.");
}

/*
 FUncion que muestra en pantalla cuando un archivo ha terminado de leerse
*/
void msg_endReadFile () {
    printf("\nFin lectura de archivo.");
}

/*
 Funcion que muestra en panntalla un mensaje con un posible error de lectura de archivo.
*/
void msg_ErrorOpenFile () {
    printf("\nHa ocurrido un error al cargar el archivo.\n");
    exit (0);
}
