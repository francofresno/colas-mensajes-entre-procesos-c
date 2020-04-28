#include "cola.h"
#include "interfaz.h"

/*
 Funcion principal que se encarga de la ejecucion del programa
 Entrada argc: es un entero que dice el numero de elementos que conforman el
			   arreglo argv
 Entrada argv: arreglo de strings(cadena de caracteres) que contiene
 			   los archivos de entrada y salida del programa del programa
 Salida: un entero que termina el programa.
*/
int main (int argc, char** argv)
{
    EstructuraColas* s = NULL;
    s = Construye(argv[1]);
    menuInterno (s, argv[2]);

	return 0;
}
