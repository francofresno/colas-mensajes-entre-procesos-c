/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Header Memory Common Logic
 ============================================================================
 */

#ifndef MEMORY_COMMONS_H_
#define MEMORY_COMMONS_H_

#include <stddef.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>


#include <commons/collections/list.h>

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
	int free;
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
t_list* deleted_messages_ids;

// Mutex
extern pthread_mutex_t mutex_lru_list;
extern pthread_mutex_t mutex_deleted_messages_ids;

#endif /* MEMORY_COMMONS_H_ */
