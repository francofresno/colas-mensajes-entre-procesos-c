/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Buddy System Algorithm Header
 ============================================================================
 */

#ifndef BUDDY_SYSTEM_H_
#define BUDDY_SYSTEM_H_

#include "memory_constants.h"

void* buddy_alloc(int size);
void buddy_init();

#endif /* BUDDY_SYSTEM_H_ */
