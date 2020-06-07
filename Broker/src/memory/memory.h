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

void* memory_alloc(int size, void* memory, memory_algorithm memory_algorithm, selection_algorithm partition_algorithm, selection_algorithm victim_algorithm);

#endif /* MEMORY_H_ */
