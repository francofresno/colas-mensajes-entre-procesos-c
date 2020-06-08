/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Memory
 ============================================================================
 */

#include "memory.h"

void load_memory(int size, t_memory_algorithm memory_alg, t_selection_algorithm victim_alg, t_selection_algorithm partition_alg)
{
	MEMORY = malloc(size);
	MEMORY_SIZE = size;
	MEMORY_ALGORITHM = memory_alg;
	PARTITION_SELECTION_ALGORITHM = victim_alg;
	VICTIM_SELECTION_ALGORITHM = partition_alg;

	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		buddy_init(MEMORY_SIZE);
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		dp_init(MEMORY_SIZE);
	}
}

void* memory_alloc()
{
	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		return buddy_alloc(MEMORY_SIZE, MEMORY, MEMORY_ALGORITHM, VICTIM_SELECTION_ALGORITHM);
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		return dp_alloc(MEMORY_SIZE, MEMORY, MEMORY_ALGORITHM, PARTITION_SELECTION_ALGORITHM, VICTIM_SELECTION_ALGORITHM);
	}

	return NULL;
}
