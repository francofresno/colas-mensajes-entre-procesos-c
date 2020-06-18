#include "filesystem.h"

char* obtenerRutaTotal(char* path, char* path2)
{
	char* pathTotal = string_new();
	string_append_with_format(&pathTotal, "%s/%s/%s", PUNTO_MONTAJE, path, path2);
	return pathTotal;
}
