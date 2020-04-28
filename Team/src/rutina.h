#ifndef rutinas_h
#define rutinas_h

#include <stdlib.h>


void Insertarhilo(EstructuraColas* s, pthread_t* hilo);

void Elimhilo(EstructuraColas *s, long pid, short prio);

void ElimHiloEjecutando(EstructuraColas *s);

pthread_t *Proxhilo(EstructuraColas *s);

void CambiarEstado(EstructuraColas *s, pthread_t* p, Estado newestado);

void DetenerEjecucion (EstructuraColas *s);

#endif //rutinas_h
