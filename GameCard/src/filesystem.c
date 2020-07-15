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

	if(!existePokemon(pathDirectorio))
			mkdir(pathDirectorio, 0777);

	closedir(directorioActual);
}

char* verificarPokemon(char* nombrePokemon)
{
	char* pathDirectorioFiles = obtenerRutaTotal("Files");
	char* pathDirectorioPokemon = string_from_format("%s/%s", pathDirectorioFiles, nombrePokemon);
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

	int cantidadBloques = cantidadElementosArray(bloquesAsignados);

	if(sePuedeEscribirElUltimoBloque(sizeBloquesPokemon))
	{
		char* rutaBloque = obtenerRutaBloque(atoi(bloquesAsignados[cantidadBloques-1]));
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

bool existePokemon(char* nombrePokemon)
{
	char* pathDirectorioFiles = obtenerRutaTotal("Files");
	char* pathDirectorioPokemon = string_from_format("%s/%s", pathDirectorioFiles, nombrePokemon);

	DIR* directorioPokemon = opendir(pathDirectorioPokemon);

	bool existe = !(directorioPokemon == NULL);

	closedir(directorioPokemon);

	free(pathDirectorioFiles);
	free(pathDirectorioPokemon);
	return existe;
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

void obtenerCoordenadas(char* archivoPokemonMapeado, int* aDevolver)
{
	char** todasLasLineas = string_split(archivoPokemonMapeado, "\n");

	int j = 0;
	for(int i = 0; todasLasLineas[i]; i++)
	{
		char** lineaDividida = string_split(todasLasLineas[i], "=");
		char** coordenadasLinea = string_split(lineaDividida[0], "-");
		aDevolver[j] = atoi(coordenadasLinea[0]);
		aDevolver[j+1] = atoi(coordenadasLinea[1]);
		printf("X: %d\nY: %d\n\n", aDevolver[j], aDevolver[j+1]);
		liberarArray(lineaDividida, 2);
		liberarArray(coordenadasLinea, 2);
		j+=2;
	}

	liberarArray(todasLasLineas, cantidadElementosArray(todasLasLineas));
}

char* arreglarNombrePokemon(t_nombrePokemon nombrePokemon)
{
	char* nombreAUsar = malloc(nombrePokemon.nombre_lenght+1);
	memcpy(nombreAUsar, nombrePokemon.nombre, nombrePokemon.nombre_lenght);
	char caracterNulo = '\0';
	memcpy(nombreAUsar+nombrePokemon.nombre_lenght, &caracterNulo, 1);
	return nombreAUsar;
}

int cantidadElementosArray(char** array)
{
	int i = 0;
	while(array[i])
	{
		i++;
	}
	return i;
}

void liberarArray(char** array, int cantidadElementos)
{
	for(int i = 0; i < cantidadElementos; i++)
		free(array[i]);
	free(array);
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

	char* stringLeido = malloc(BLOCK_SIZE+1);

	int bytesLeidos = fread(stringLeido, BLOCK_SIZE, 1, archivoBloque);

	if(bytesLeidos != 1)
		exit(-1);

	int bytesEscritos = 0;
	fseek(archivoBloque, strlen(stringLeido), SEEK_SET);
	if(strlen(stringLeido)+strlen(stringAEscribir) < BLOCK_SIZE)
		bytesEscritos = fwrite(stringAEscribir, sizeof(char), strlen(stringAEscribir), archivoBloque);
	else
		bytesEscritos = fwrite(stringAEscribir, sizeof(char), BLOCK_SIZE-strlen(stringLeido), archivoBloque);

	fclose(archivoBloque);
	free(stringLeido);
	return bytesEscritos;
}

int leerArchivoPokemon(char* pathMetadataPokemon, int** aDevolver)
{
	t_config* configMetadataPokemon = abrirArchivo(pathMetadataPokemon);

	char** bloquesAsignados = config_get_array_value(configMetadataPokemon, "BLOCKS");

	char* archivoPokemon = string_new();

	for(int i = 0; bloquesAsignados[i]; i++)
	{
		char* pathBloqueActual = obtenerRutaBloque(atoi(bloquesAsignados[i]));
		char* stringLeido = leerBloque(pathBloqueActual);
		string_append(&archivoPokemon, stringLeido);

		free(pathBloqueActual);
		free(stringLeido);
	}

	char** lineasTotales = string_split(archivoPokemon, "\n");
	int cantidadLineas = cantidadElementosArray(lineasTotales);

	int *coordenadasFinales = malloc(2*sizeof(int)*cantidadLineas);

	obtenerCoordenadas(archivoPokemon, coordenadasFinales);

	*aDevolver = coordenadasFinales;

	liberarArray(lineasTotales, cantidadLineas);
	liberarArray(bloquesAsignados, cantidadElementosArray(bloquesAsignados));
	free(archivoPokemon);
	cerrarArchivo(configMetadataPokemon);

	return cantidadLineas;
}

char* leerBloque(char* pathBloque)
{
	FILE* archivoBloque = fopen(pathBloque, "rb");
	char* stringLeido = malloc(BLOCK_SIZE+1);
	fread(stringLeido, BLOCK_SIZE, 1, archivoBloque);
	fclose(archivoBloque);
	return stringLeido;
}

/**************************************************
 ******Funciones de procesamiento de mensajes******
 **************************************************/

int procesarNewPokemon(t_newPokemon_msg* estructuraNew)
{
	char* nombrePokemon = arreglarNombrePokemon(estructuraNew->nombre_pokemon);

	char* pathMetadataPokemon = verificarPokemon(nombrePokemon);
	char* stringAEscribir = string_from_format("%d-%d=%d\n", estructuraNew->coordenadas.posX, estructuraNew->coordenadas.posY, estructuraNew->cantidad_pokemons);
	escribirArchivoPokemon(stringAEscribir, pathMetadataPokemon);

	free(stringAEscribir);
	free(pathMetadataPokemon);
	free(nombrePokemon);
	return 0;
}

t_localizedPokemon_msg procesarGetPokemon(t_getPokemon_msg* estructuraGet)
{
	char* nombrePokemon = arreglarNombrePokemon(estructuraGet->nombre_pokemon);

	t_localizedPokemon_msg estructuraLocalized;

	if(!existePokemon(nombrePokemon))
	{
		estructuraLocalized.cantidad_coordenadas = 0;
		estructuraLocalized.nombre_pokemon = estructuraGet->nombre_pokemon;
		estructuraLocalized.coordenadas = NULL;
		return estructuraLocalized;
	}
	char* pathMetadataPokemon = verificarPokemon(nombrePokemon);
	int* aImprimir = NULL;
	estructuraLocalized.cantidad_coordenadas = leerArchivoPokemon(pathMetadataPokemon, &aImprimir);

	estructuraLocalized.nombre_pokemon = estructuraGet->nombre_pokemon;

	printf("CantCoor: %d\n", estructuraLocalized.cantidad_coordenadas);
	estructuraLocalized.coordenadas = malloc(sizeof(uint32_t) * estructuraLocalized.cantidad_coordenadas * 2);

	int j = 0;
	for(int i = 0; i < estructuraLocalized.cantidad_coordenadas; i++)
	{
		printf("J: %d - J+1: %d\n", aImprimir[j], aImprimir[j+1]);
		estructuraLocalized.coordenadas[i].posX = aImprimir[j];
		estructuraLocalized.coordenadas[i].posY = aImprimir[j+1];
		j+=2;
	}


	free(nombrePokemon);
	free(aImprimir);
	free(pathMetadataPokemon);
	return estructuraLocalized;
}

t_caughtPokemon_msg procesarCatchPokemon(t_catchPokemon_msg* estructuraCatch)
{
	char* nombrePokemon = arreglarNombrePokemon(estructuraCatch->nombre_pokemon);

	if(!existePokemon(nombrePokemon))
	{
		t_caughtPokemon_msg estructuraCaught;
		estructuraCaught.atrapado = 0;
		return estructuraCatch;
	}
}





