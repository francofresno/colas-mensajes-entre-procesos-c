/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Dynamic Partitions Algorithm Header
 ============================================================================
 */

#ifndef DYNAMIC_PARTITIONS_H_
#define DYNAMIC_PARTITIONS_H_

#include "memory_algorithm.h"
#include<commons/collections/list.h>

typedef struct
{
	void* data;
	int size;
} t_partition;

t_list* FREE_PARTITIONS;
t_list* OCCUPIED_PARTITIONS;

void dp_init(int size);
void* dp_alloc(int size, int min_partition_size, int comp_frequency, void* memory, t_memory_algorithm memory_algorithm, t_selection_algorithm partition_algorithm, t_selection_algorithm victim_algorithm);
t_partition* first_fit_find_free_partition(int size);
t_partition* best_fit_find_free_partition(int size);
t_partition* fifo_find_victim_partition(int size);
t_partition* lru_find_victim_partition(int size);

#endif /* DYNAMIC_PARTITIONS_H_ */
