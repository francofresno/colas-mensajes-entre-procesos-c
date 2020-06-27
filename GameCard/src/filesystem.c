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
		char* pathDirectorioACrear = obtenerRutaTotal(directoriosACrear[i]);
		verificarDirectorio(pathDirectorioACrear);
		verificarMetadata(pathDirectorioACrear);
		free(pathDirectorioACrear);
	}
}

/****************************************************
 **Funciones de verificación de archivos iniciales***
 ****************************************************/

void verificarMetadata(char* pathDirectorio)
{
	char* pathMetadata = string_from_format("%s/Metadata.bin", pathDirectorio);
	FILE* archivoMetadata = fopen(pathMetadata, "rb");

	if(!archivoMetadata)
	{
		archivoMetadata = fopen(pathMetadata, "wb");
		if(string_ends_with(pathDirectorio, "Metadata"))
			fprintf(archivoMetadata, "BLOCK_SIZE=64\nBLOCKS=5192\nMAGIC_NUMBER=TALL_GRASS");
		else
			fprintf(archivoMetadata, "DIRECTORY=Y");
	}
	fclose(archivoMetadata);

	if(string_ends_with(pathDirectorio, "Metadata"))
	{
		t_config* configMetadata = config_create(pathMetadata);
		BLOCKS = config_get_int_value(configMetadata, "BLOCKS");
		BLOCK_SIZE = config_get_int_value(configMetadata, "BLOCK_SIZE");
		config_destroy(configMetadata);
		verificarBitmap(pathDirectorio);
	}
	else if(string_ends_with(pathDirectorio, "Blocks")){
		verificarBloquesIniciales(pathDirectorio);
	}
	free(pathMetadata);
}

void verificarBitmap(char* pathDirectorio)
{
	char* pathBitmap = string_from_format("%s/Bitmap.bin", pathDirectorio);

	FILE* archivoActual = fopen(pathBitmap, "rb");
	if(!archivoActual)
		archivoActual = fopen(pathBitmap, "wb");
	fclose(archivoActual);

	truncate(pathBitmap, BLOCKS/8);
	int fileDescriptor = open(pathBitmap, O_RDWR);
	char* bitarrayFlujo = mmap(NULL, BLOCKS/8, PROT_WRITE | PROT_READ, MAP_SHARED, fileDescriptor, 0);
	bitarray = bitarray_create_with_mode(bitarrayFlujo, BLOCKS/8, LSB_FIRST);
	close(fileDescriptor);

	free(pathBitmap);
}

void verificarBloquesIniciales(char* pathDirectorio)
{
	FILE* archivoActual;
	for(int i = 1; i <= BLOCKS; i++)
	{
		char* pathBloqueActual = string_from_format("%s/%d.bin", pathDirectorio, i);

		archivoActual = fopen(pathBloqueActual, "rb");
		if(!archivoActual)
		{
			archivoActual = fopen(pathBloqueActual, "wb");
			truncate(pathBloqueActual, BLOCK_SIZE);
		}
		fclose(archivoActual);

		free(pathBloqueActual);
	}
}

/****************************************************
 **Funciones de verificación de archivos generales***
 ****************************************************/

void verificarDirectorio(char* pathDirectorio)
{
	DIR* directorioActual = opendir(pathDirectorio);

	if(!directorioActual)
			mkdir(pathDirectorio, 0777);

	closedir(directorioActual);
}

char* verificarPokemon(t_nombrePokemon nombrePokemon)
{
	char* pathDirectorioFiles = obtenerRutaTotal("Files");
	char* pathDirectorioPokemon = string_from_format("%s/%s", pathDirectorioFiles, nombrePokemon.nombre);
	verificarDirectorio(pathDirectorioPokemon);

	char* pathMetadataPokemon = string_from_format("%s/Metadata.bin", pathDirectorioPokemon);
	verificarMetadataPokemon(pathMetadataPokemon);

	free(pathDirectorioPokemon);
	free(pathDirectorioFiles);
	return pathMetadataPokemon;
}

void verificarMetadataPokemon(char* pathMetadata)
{
	FILE* archivoActual = fopen(pathMetadata, "rb");
	if(!archivoActual)
	{
		archivoActual = fopen(pathMetadata, "wb");
		fprintf(archivoActual, "DIRECTORY=N\nBLOCKS=[]\nSIZE=0\nOPEN=N");
	}
	fclose(archivoActual);
}

char* verificarBloque(t_config* configMetadata)
{
	char** bloquesAsignados = config_get_array_value(configMetadata, "BLOCKS");
	int sizeBloquesPokemon = config_get_int_value(configMetadata, "SIZE");

	int i = 0;
	while(bloquesAsignados[i])
	{
		i++;
	}

	if(sePuedeEscribirElUltimoBloque(sizeBloquesPokemon))
	{
		char* rutaBloque = obtenerRutaBloque(atoi(bloquesAsignados[i-1]));
		free(bloquesAsignados);
		return rutaBloque;
	}
	else
	{
		free(bloquesAsignados);
		return asignarBloque(configMetadata);
	}
}

/*******************************************
 *************Funciones boolean*************
 ******************************************/

bool sePuedeEscribirElUltimoBloque(int sizeBloquesPokemon)
{
	return sizeBloquesPokemon%BLOCK_SIZE != 0;
}
/*******************************************
 *******Funciones de índole general*********
 ******************************************/

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

int obtenerLugarEnBitmap()
{
	for(int i = 0; i < BLOCKS; i++)
	{
		if(!bitarray_test_bit(bitarray, i))
		{
			bitarray_set_bit(bitarray, i);
			msync((void*) bitarray->bitarray, BLOCKS/8, MS_SYNC);
			return i+1;
		}
	}
	return 0;
}

/*******************************************
 ******Funciones de obtención de rutas******
 ******************************************/

char* obtenerRutaTotal(char* path)
{
	char* pathTotal = string_from_format("%s/%s", PUNTO_MONTAJE, path);
	return pathTotal;
}

char* obtenerRutaBloque(int numeroBloque)
{
	char* pathDirectorioBlocks = obtenerRutaTotal("Blocks");
	char* pathBloque = string_from_format("%s/%d.bin", pathDirectorioBlocks, numeroBloque);
	free(pathDirectorioBlocks);
	return pathBloque;
}

/******************************************
 **Funciones de modificación de archivos***
 ******************************************/

t_config* abrirArchivo(char* pathArchivo)
{
	t_config* configArchivo = config_create(pathArchivo);
	char* estadoArchivo = config_get_string_value(configArchivo, "OPEN");
	while(strcmp(estadoArchivo, "Y") == 0)
	{
		sleep(config_get_int_value(configGeneral, "TIEMPO_DE_REINTENTO_OPERACION"));
	}
	config_set_value(configArchivo, "OPEN", "Y");
	config_save(configArchivo);
	return configArchivo;
}

void cerrarArchivo(t_config* configArchivo)
{
	config_set_value(configArchivo, "OPEN", "N");
	config_save(configArchivo);
	config_destroy(configArchivo);
}

char* asignarBloque(t_config* configPokemon)
{
	int bloqueAAsignar = obtenerLugarEnBitmap();
	if(bloqueAAsignar > 0)
	{
		char** bloquesAsignados = config_get_array_value(configPokemon, "BLOCKS");
		char* bloquesAsignadosComoArray = armarArrayDeBloques(bloquesAsignados, bloqueAAsignar);

		config_set_value(configPokemon, "BLOCKS", bloquesAsignadosComoArray);
		config_save(configPokemon);

		free(bloquesAsignadosComoArray);
		free(bloquesAsignados);
		return obtenerRutaBloque(bloqueAAsignar);
	}
	else
		return NULL;
}

void escribirArchivoPokemon(char* stringAEscribir, char* pathMetadataPokemon)
{
	t_config* configMetadataPokemon = abrirArchivo(pathMetadataPokemon);

	char* pathBloqueAEscribir = verificarBloque(configMetadataPokemon);

	int bytesEscritos = escribirBloque(pathBloqueAEscribir, stringAEscribir);

	free(pathBloqueAEscribir);

	int sizeActual = config_get_int_value(configMetadataPokemon, "SIZE");
	char* bytesTotales = string_itoa(sizeActual+bytesEscritos);
	config_set_value(configMetadataPokemon, "SIZE", bytesTotales);
	config_save(configMetadataPokemon);
	free(bytesTotales);

	if(bytesEscritos < strlen(stringAEscribir))
	{
		char* pathNuevoBloqueAEscribir = verificarBloque(configMetadataPokemon);
		char* stringResto = string_substring_from(stringAEscribir, bytesEscritos);
		bytesEscritos += escribirBloque(pathNuevoBloqueAEscribir, stringResto);
		free(stringResto);
		free(pathNuevoBloqueAEscribir);
	}
	bytesTotales = string_itoa(sizeActual+bytesEscritos);
	config_set_value(configMetadataPokemon, "SIZE", bytesTotales);

	free(bytesTotales);
	cerrarArchivo(configMetadataPokemon);
}

int escribirBloque(char* pathBloqueAEscribir, char* stringAEscribir)
{
	FILE* archivoBloque = fopen(pathBloqueAEscribir, "rb+");

	char* stringLeido = malloc(65);

	int bytesLeidos = fread(stringLeido, 64, 1, archivoBloque);

	if(bytesLeidos != 1)
		exit(-1);

	int bytesEscritos = 0;
	fseek(archivoBloque, strlen(stringLeido), SEEK_SET);
	if(strlen(stringLeido)+strlen(stringAEscribir) < 64)
		bytesEscritos = fwrite(stringAEscribir, sizeof(char), strlen(stringAEscribir), archivoBloque);
	else
		bytesEscritos = fwrite(stringAEscribir, sizeof(char), 64-strlen(stringLeido), archivoBloque);

	fclose(archivoBloque);
	free(stringLeido);
	return bytesEscritos;
}

/**************************************************
 ******Funciones de procesamiento de mensajes******
 **************************************************/

int procesarNewPokemon(t_newPokemon_msg* estructuraNew)
{
	t_nombrePokemon nombrePokemon;
	nombrePokemon.nombre_lenght = estructuraNew->nombre_pokemon.nombre_lenght;
	nombrePokemon.nombre = string_duplicate(estructuraNew->nombre_pokemon.nombre);

	char* pathMetadataPokemon = verificarPokemon(nombrePokemon);
	char* stringAEscribir = string_from_format("%d-%d=%d\n", estructuraNew->coordenadas.posX, estructuraNew->coordenadas.posY, estructuraNew->cantidad_pokemons);
	escribirArchivoPokemon(stringAEscribir, pathMetadataPokemon);

	free(stringAEscribir);
	free(pathMetadataPokemon);
	free(nombrePokemon.nombre);
	return 0;
}










