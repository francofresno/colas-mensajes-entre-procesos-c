/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Buddy System Algorithm Header
 ============================================================================
 */

#ifndef BUDDY_SYSTEM_H_
#define BUDDY_SYSTEM_H_

#include "memory_algorithm.h"

void* buddy_alloc(int size, int min_partition_size, int comp_frequency, void* memory, t_memory_algorithm memory_algorithm, t_selection_algorithm victim_algorithm);
void buddy_init(int size);

#endif /* BUDDY_SYSTEM_H_ */
