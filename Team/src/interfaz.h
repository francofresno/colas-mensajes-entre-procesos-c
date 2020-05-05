#ifndef interfaz_h
#define interfaz_h
#include <stdio.h>
#include "rutina.h"
#include "cola.h"


void menuInterno(EstructuraColas* s, char* salida);

void Imprime(EstructuraColas *s);

void Imprimehilo (pthread_t* hilo);

void msg_Insercion ();

void msg_ErrorInsertar (pthread_t* hilo);

void msg_errorInsertPID (pthread_t* hilo);

void msg_ErrorElim ();

void msg_Busy ();

void msg_readFile ();

void msg_endReadFile ();

void msg_ErrorOpenFile ();


#endif //interfaz_h
