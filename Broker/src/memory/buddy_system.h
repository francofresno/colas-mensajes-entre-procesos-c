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

void buddy_init(int size);
void* buddy_alloc(int size, void* memory, t_memory_algorithm memory_algorithm, t_selection_algorithm victim_algorithm);

#endif /* BUDDY_SYSTEM_H_ */
