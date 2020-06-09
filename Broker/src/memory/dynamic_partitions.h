/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Dynamic Partitions Algorithm Header
 ============================================================================
 */

#ifndef DYNAMIC_PARTITIONS_H_
#define DYNAMIC_PARTITIONS_H_

#include<commons/collections/list.h>
#include "memory_constants.h"

t_list* FREE_PARTITIONS;
t_list* OCCUPIED_PARTITIONS;

// Mutex
pthread_mutex_t mutex_free_list = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_occupied_list = PTHREAD_MUTEX_INITIALIZER;

void dp_init();
void* dp_alloc(int size);
t_partition* first_fit_find_free_partition(int size);
t_partition* best_fit_find_free_partition(int size);
t_partition* fifo_find_victim_partition(int size);
t_partition* lru_find_victim_partition(int size);

#endif /* DYNAMIC_PARTITIONS_H_ */
