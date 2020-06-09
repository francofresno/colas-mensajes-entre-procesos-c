/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Memory Algorithm Common Logic
 ============================================================================
 */

#ifndef MEMORY_MEMORY_CONSTANTS_H_
#define MEMORY_MEMORY_CONSTANTS_H_

#include <stddef.h>
#include <pthread.h>

// Enums memorias
typedef enum {
	DYNAMIC_PARTITIONS,
	BUDDY_SYSTEM,
} t_memory_algorithm;

typedef enum {
	FIRST_FIT,
	BEST_FIT,
	FIFO,
	LRU,
	NONE
} t_selection_algorithm;

// Particion
typedef struct
{
	uint32_t id_data;
	void* data;
	int base;
	int size;
} t_partition;

// Constantes
void* MEMORY;
int MEMORY_SIZE;
int MIN_PARTITION_SIZE;
int COMPACTION_FREQUENCY;
t_memory_algorithm MEMORY_ALGORITHM;
t_selection_algorithm PARTITION_SELECTION_ALGORITHM;
t_selection_algorithm VICTIM_SELECTION_ALGORITHM;

t_list* lru_list;

// Mutex
pthread_mutex_t mutex_lru_list = PTHREAD_MUTEX_INITIALIZER;

#endif /* MEMORY_MEMORY_CONSTANTS_H_ */
