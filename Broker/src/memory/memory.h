/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Memory Header
 ============================================================================
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "dynamic_partitions.h"
#include "buddy_system.h"

typedef struct
{
	void* data;
	void* alloc;
	uint32_t id;
} t_copy_args;

// Mutex
extern pthread_mutex_t mutex_memory;

void load_memory(int size, int min_partition_size, int frequency, t_memory_algorithm memory_alg, t_selection_algorithm victim_alg, t_selection_algorithm partition_alg);
void* memory_alloc(int size);
void add_to_lru(t_partition* partition);
void notify_message_used(uint32_t id_message);


#endif /* MEMORY_H_ */
