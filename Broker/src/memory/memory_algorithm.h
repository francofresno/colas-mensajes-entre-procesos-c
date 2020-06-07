/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Memory Algorithm Common Logic
 ============================================================================
 */

#ifndef MEMORY_MEMORY_ALGORITHM_H_
#define MEMORY_MEMORY_ALGORITHM_H_

// Enums memorias
typedef enum {
	DYNAMIC_PARTITIONS,
	BUDDY_SYSTEM,
} memory_algorithm;

typedef enum {
	FIRST_FIT,
	BEST_FIT,
	FIFO,
	LRU,
	NONE
} selection_algorithm;

#endif /* MEMORY_MEMORY_ALGORITHM_H_ */
