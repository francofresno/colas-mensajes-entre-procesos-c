#include "filesystem.h"

char* directoriosACrear[] = {"Metadata", "Files", "Blocks"};
int BLOCKS;
int BLOCK_SIZE;


t_bitarray* bitarray;

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
	verificarDirectorio(pathActual);
	return pathActual;
}

void verificarMetadataPokemon(char* pathMetadata)
{
	FILE* archivoActual = fopen(pathMetadata, "rb");
	if(!archivoActual)
	{
		archivoActual = fopen(pathMetadata, "wb");
		t_config* config = config_create(pathMetadata);
		config_set_value(config, "DIRECTORY", "N");
		config_set_value(config, "BLOCKS", "[]");
		config_set_value(config, "SIZE", "0");
		config_set_value(config, "OPEN", "N");
		config_save(config);
		config_destroy(config);
	}
}

char* verificarBloque(char* pathMetadata)
{
	t_config* config = config_create(pathMetadata);

//	config_set_value(config, "OPEN", "Y");
//	config_save(config);

	char** bloquesAsignados = config_get_array_value(config, "BLOCKS");

	int sizeArchivos = config_get_int_value(config, "SIZE");

//	config_set_value(config, "OPEN", "N");
//	config_save(config);

	int i = 0;
	while(bloquesAsignados[i])
	{
		i++;
	}

	if(sePuedeEscribirElUltimoBloque(sizeArchivos))
		return obtenerRutaBloque(i+1);
	else
		return asignarBloque(config);
}
/******************************************
 **Funciones de modificación de archivos***
 ******************************************/

char* asignarBloque(t_config* config)
{
	char* path = NULL;
	for(int i = 0; path == NULL && i < BLOCKS; i++)
	{
		if(!bitarray_test_bit(bitarray, i))
		{
			bitarray_set_bit(bitarray, i);

//			config_set_value(config, "OPEN", "Y");
//			config_save(config);

			char** bloquesAsignados = config_get_array_value(config, "BLOCKS");

//			config_set_value(config, "OPEN", "N");
//			config_save(config);

			char* bloquesAsignadosComoArray = armarArrayDeBloques(bloquesAsignados, i+1);

			path = obtenerRutaBloque(i+1);

			config_set_value(config, "BLOCKS", bloquesAsignadosComoArray);
			config_save(config);

			free(bloquesAsignadosComoArray);
		}
	}
	return path;
}

void probarAsignacion()
{
	t_newPokemon_msg estructuraNew;
	estructuraNew.nombre_pokemon.nombre = "Pikachu";
	estructuraNew.nombre_pokemon.nombre_lenght = strlen(estructuraNew.nombre_pokemon.nombre);
	estructuraNew.coordenadas.posX = 1;
	estructuraNew.coordenadas.posY = 2;
	estructuraNew.cantidad_pokemons = 2;

	char* pathDirectorioPokemon = verificarPokemon(estructuraNew.nombre_pokemon);
	string_append(&pathDirectorioPokemon, "/Metadata.bin");
	verificarMetadataPokemon(pathDirectorioPokemon);
	verificarBloque(pathDirectorioPokemon);
}

char* armarArrayDeBloques(char** arrayOriginal, int numeroBloque)
{
	int j = 0;
	char* bloquesAsignadosComoArray = string_new();
	string_append(&bloquesAsignadosComoArray, "[");
	while(arrayOriginal[j])
	{
		string_append_with_format(&bloquesAsignadosComoArray, "%s,", arrayOriginal[j]);
		j++;
	}
	string_append_with_format(&bloquesAsignadosComoArray, "%d]", numeroBloque);
	return bloquesAsignadosComoArray;
}
/*******************************************
 ******Funciones de obtención de rutas******
 ******************************************/
char* obtenerRutaTotal(char* path)
{
	char* pathTotal = string_new();
	string_append_with_format(&pathTotal, "%s/%s", PUNTO_MONTAJE, path);
	return pathTotal;
}

char* obtenerRutaBloque(int numeroBloque)
{
	char* path = obtenerRutaTotal("Blocks");
	char* bloqueAsignado = malloc(7);
	sprintf(bloqueAsignado, "/%d.bin", numeroBloque);
	string_append(&path, bloqueAsignado);
	free(bloqueAsignado);
	return path;
}

/*******************************************
 *************Funciones boolean*************
 ******************************************/

bool sePuedeEscribirElUltimoBloque(int sizeArchivos)
{
	return sizeArchivos%64 != 0;
}




