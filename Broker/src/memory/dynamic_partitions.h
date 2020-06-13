/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Dynamic Partitions Algorithm Header
 ============================================================================
 */

#ifndef DYNAMIC_PARTITIONS_H_
#define DYNAMIC_PARTITIONS_H_

#include "memory_commons.h"

t_list* FREE_PARTITIONS;
t_list* OCCUPIED_PARTITIONS;

int SEARCH_FAILURE_COUNTER;

// Mutex
extern pthread_mutex_t mutex_free_list;
extern pthread_mutex_t mutex_occupied_list;

void dp_init();
void* dp_alloc(int size);
t_partition* find_free_partition(int size);
t_partition* choose_victim_partition();
void compact_memory();

int get_index_of_partition(t_list* partitions, uint32_t id_partition);

t_partition* first_fit_find_free_partition(int size);
t_partition* best_fit_find_free_partition(int size);
t_partition* fifo_find_victim_partition();
t_partition* lru_find_victim_partition();

#endif /* DYNAMIC_PARTITIONS_H_ */
