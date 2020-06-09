/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Memory Header
 ============================================================================
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdlib.h>

#include "dynamic_partitions.h"
#include "buddy_system.h"

void* MEMORY;
int MEMORY_SIZE;
int MIN_PARTITION_SIZE;
int COMPACTION_FREQUENCY;
t_memory_algorithm MEMORY_ALGORITHM;
t_selection_algorithm PARTITION_SELECTION_ALGORITHM;
t_selection_algorithm VICTIM_SELECTION_ALGORITHM;

void load_memory(int size, int min_partition_size, int frequency, t_memory_algorithm memory_alg, t_selection_algorithm victim_alg, t_selection_algorithm partition_alg);
void* memory_alloc();

#endif /* MEMORY_H_ */
