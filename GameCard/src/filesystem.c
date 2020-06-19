#include "filesystem.h"

char* directoriosACrear[] = {"Metadata", "Files", "Blocks"};

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
	string_append(&pathActual, "/Metadata.bin");
	FILE* archivoActual = fopen(pathActual, "wb");

	if(strcmp(path, "Metadata") == 0)
	{
		t_config* config = config_create(pathActual);
		config_set_value(config, "BLOCKS", "5192");
		config_set_value(config, "BLOCK_SIZE", "64");
		config_set_value(config, "MAGIC_NUMBER", "TALL_GRASS");
		config_save_in_file(config, pathActual);

		char* pathMetadata = obtenerRutaTotal(path);
		verificarBitmap(pathMetadata, config);

		config_destroy(config);
	}
	else
		fprintf(archivoActual, "DIRECTORY=Y");

	fclose(archivoActual);
}

void verificarBitmap(char* pathActual, t_config* config)
{
	string_append(&pathActual, "/Bitmap.bin");

	FILE* archivoActual = fopen(pathActual, "rb");
	if(!archivoActual)
	{
		archivoActual = fopen(pathActual, "wb");
		int cantidadBloques = config_get_int_value(config, "BLOCKS");
		char* seteoInicial = string_repeat('1', cantidadBloques);
		fprintf(archivoActual, "%s", seteoInicial);
	}
	else
	{
		//cargarEnMemoria...
	}

	fclose(archivoActual);
}
