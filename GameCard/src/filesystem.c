#include "filesystem.h"

char* directoriosACrear[] = {"Metadata", "Files", "Blocks"};
int BLOCKS;
int BLOCK_SIZE;


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

/******************************************
 **Funciones de verificación de archivos***
 ******************************************/

void verificarDirectorio(char* path)
{
	DIR* directorioActual = opendir(path);

	if(!directorioActual)
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
		BLOCKS = config_get_int_value(config, "BLOCKS");
		BLOCK_SIZE = config_get_int_value(config, "BLOCK_SIZE");
		verificarBitmap(pathDirectorio, config);
		config_destroy(config);
	}
	else if(strcmp(path, "Blocks") == 0){
		verificarBloques(pathDirectorio);
	}
}

void verificarBitmap(char* pathActual, t_config* config)
{
	string_append(&pathActual, "/Bitmap.bin");
	char* bitarrayFlujo;

	FILE* archivoActual = fopen(pathActual, "rb");
	if(!archivoActual)
		archivoActual = fopen(pathActual, "wb");
	fclose(archivoActual);

	truncate(pathActual, BLOCKS/8);
	int fileDescriptor = open(pathActual, O_RDWR);
	bitarrayFlujo = mmap(NULL, BLOCKS/8, PROT_WRITE | PROT_READ, MAP_SHARED, fileDescriptor, 0);
	bitarray = bitarray_create_with_mode(bitarrayFlujo, BLOCKS/8, LSB_FIRST);
//	msync((void*) bitarrayFlujo, BLOCKS/8, MS_SYNC);
	close(fileDescriptor);
}

void verificarBloques(char* pathDirectorio)
{
	FILE* archivoActual;
	for(int i = 1; i <= BLOCKS; i++)
	{
		char* numeroBloque = malloc(7);
		char* pathActual = strdup(pathDirectorio);
		sprintf(numeroBloque, "/%d.bin", i);
		string_append(&pathActual, numeroBloque);
		archivoActual = fopen(pathActual, "rb");
		if(!archivoActual)
		{
			archivoActual = fopen(pathActual, "wb");
			truncate(pathActual, BLOCK_SIZE);
		}
		fclose(archivoActual);
		free(numeroBloque);
		free(pathActual);
	}
}

char* verificarPokemon(t_nombrePokemon nombrePokemon)
{
	char* pathActual = obtenerRutaTotal("/Files/");
	string_append(&pathActual, nombrePokemon.nombre);
	FILE* archivoActual = fopen(pathActual, "rb");
	if(!archivoActual)
		archivoActual = fopen(pathActual, "wb");
	fclose(archivoActual);
	return pathActual;
}

/******************************************
 **Funciones de modificación de archivos***
 ******************************************/

void modificarMetadataPokemon(char* pathDirectorio)
{
	char* pathMetadata = pathDirectorio;
	string_append(&pathMetadata, "/Metadata.bin");

	FILE* archivoActual = fopen(pathMetadata, "rb");
	if(!archivoActual)
	{
		archivoActual = fopen(pathMetadata, "wb");
		t_config* config = config_create(pathMetadata);
		config_set_value(config, "DIRECTORY", "N");
		asignarBloque();
		//		config_set_value(config, "SIZE", size);
		//		config_set_value(config, "BLOCKS", bloques);
	}
}
char** asignarBloque()
{
	for(int i = 0; i < BLOCKS; i++)
	{
		if(!bitarray_test_bit(bitarray, i))
		{
			bitarray_set_bit(bitarray, i);

		}
	}
}



