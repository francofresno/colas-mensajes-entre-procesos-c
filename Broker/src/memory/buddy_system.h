/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Buddy System Algorithm Header
 ============================================================================
 */

#ifndef BUDDY_SYSTEM_H_
#define BUDDY_SYSTEM_H_

#include "memory_commons.h"

void buddy_init();
void* buddy_alloc(int size);

int get_index_of_buddy(t_list* buddies, uint32_t id_partition);

#endif /* BUDDY_SYSTEM_H_ */
