#include "filesystem.h"

char* directoriosACrear[] = {"Metadata", "Files", "Blocks"};
char* BLOCKS = "5192";
char* BLOCK_SIZE = "64";
char* MAGIC_NUMBER = "TALL_GRASS";
t_bitarray* bitarray;

char* obtenerRutaTotal(char* path)
{
	char* pathTotal = string_new();
	string_append_with_format(&pathTotal, "%s/%s", PUNTO_MONTAJE, path);
	return pathTotal;
}

void configuracionInicial(void)
{
	verificarDirectorio(PUNTO_MONTAJE);

	for(int i = 0; i < sizeof(directoriosACrear)/sizeof(char*); i++)
	{
		verificarDirectorio(obtenerRutaTotal(directoriosACrear[i]));
		verificarMetadata(directoriosACrear[i]);
	}
}

void verificarDirectorio(char* path)
{
	DIR* directorioActual = opendir(path);

	if(directorioActual == NULL)
			mkdir(path, 0777);

	free(directorioActual);
}

void verificarMetadata(char* path)
{
	char* pathActual = obtenerRutaTotal(path);
	char* pathDirectorio = obtenerRutaTotal(path);
	string_append(&pathActual, "/Metadata.bin");
	FILE* archivoActual = fopen(pathActual, "rb");

	if(!archivoActual)
	{
		archivoActual = fopen(pathActual, "wb");
		if(strcmp(path, "Metadata") == 0)
			fprintf(archivoActual, "BLOCK_SIZE=64\nBLOCKS=5192\nMAGIC_NUMBER=TALL_GRASS");
		else
			fprintf(archivoActual, "DIRECTORY=Y");
	}
	fclose(archivoActual);

	if(strcmp(path, "Metadata") == 0)
	{
		t_config* config = config_create(pathActual);
		verificarBitmap(pathDirectorio, config);
		config_destroy(config);
	}
	else if(strcmp(path, "Bloques") == 0){

	}
}

void verificarBitmap(char* pathActual, t_config* config)
{
	string_append(&pathActual, "/Bitmap.bin");
	int cantidadBloques = config_get_int_value(config, "BLOCKS");

	FILE* archivoActual = fopen(pathActual, "rb");
	if(!archivoActual)
	{
		archivoActual = fopen(pathActual, "wb");
		char* seteoInicial = string_repeat('0', cantidadBloques);
		fprintf(archivoActual, "%s", seteoInicial);
		fclose(archivoActual);
	}

	char* bitarrayFlujo = malloc(cantidadBloques);
	archivoActual = fopen(pathActual, "rb");
	fread(bitarrayFlujo, 1, cantidadBloques, archivoActual);
	bitarray = bitarray_create_with_mode(bitarrayFlujo, cantidadBloques/8, LSB_FIRST);
	if(!bitarray_test_bit(bitarray, 3))
		printf("Dio false");

	fclose(archivoActual);
}
//void verificarBloques(char* path)
//{
////	for(int i; i < atoi(BLOCKS))
//}
