#include "rutinas.h"
#include "interfaz.h"

/*
 Funcione que permite insertar un hilo
 Entrada s: es un apuntador a la estructura de colas del planificador
 Entrada t: es un apuntador a la estructura de datos perteneciente al hilo a insertar
 */
void Insertarhilo(EstructuraColas* s, pthread_t* t){

    if(s->enEjecucion == NULL) {

        if (t->estado == 'L') {
            pthread_t* t2 = NULL;

                t2 = consultarID(s->cola, t->ID);
                if (!t2) {
                    queue_push (t2 , void *)
                    msg_Insercion ();
                    } else
                        msg_errorInsertPID (t2);
                    break;



                default:
                    msg_ErrorInsertar(t);
                    break;
            }
        }

        else
            msg_ErrorInsertar(t);
    }

    else
        msg_Busy ();
}

/*


/*
 Funcion que permite elminar el primer hilo de la estructura de colas ejecutado
 Entrada s: es un apuntador a la estructura de colas del planificador
 */
void ElimHiloEjecutando(EstructuraColas *s) {
    queue_pop(s->enEjecucion);
    s->enEjecucion = NULL;
}

/*
 Funcion que retorna el proximo hilo a planificar
 Entrada s: es un apuntador a la estructura de colas del planificador
 Salida: identidicar al proximo hilo
 */
phread_t *Proxhilo(EstructuraColas *s) {
    if (s->cola->primero){
        s->enEjecucion = s->cola;
        CambiarEstado(s, s->enEjecucion->ultimo->hilo, 'L');
        return DesplazarNodo(s->cola);
    }

    else{
        s->enEjecucion = NULL;
        return NULL;
    }

}

/*
 Funcion que asigna un nuevo estado a un hilo
 Entrada s: es un apuntador a la estructura de colas del planificador
 Enrada p: es un apuntador a la estructura de datos perteneciente al hilo a modficar su estado
 Entrada newestado: es el estado que se asgina al hilo p
 */
void CambiarEstado (EstructuraColas *s, pthread_t* hilo, Estado newestado) {
    hilo->estado = newestado;
}

/*
 Funcion que detiene la ejecucion del hilo actual
 Entrada s: apuntador a la estructura de colas del planificador
*/
void DetenerEjecucion (EstructuraColas* s) {
    s->enEjecucion->ultimo->hilo->estado = 'L';
    s->enEjecucion = NULL;
}
