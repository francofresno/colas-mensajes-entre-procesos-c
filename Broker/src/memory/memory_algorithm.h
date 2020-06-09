/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Memory Algorithm Common Logic
 ============================================================================
 */

#ifndef MEMORY_MEMORY_ALGORITHM_H_
#define MEMORY_MEMORY_ALGORITHM_H_

#include <stddef.h>

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

#endif /* MEMORY_MEMORY_ALGORITHM_H_ */
