/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Dynamic Partitions Algorithm
 ============================================================================
 */

#include "dynamic_partitions.h"

void dp_init(int size)
{
	FREE_PARTITIONS = list_create();
	OCCUPIED_PARTITIONS = list_create();

	t_partition initial_partition;
	initial_partition.size = size;

	list_add(FREE_PARTITIONS, (void*) &initial_partition);
}

void* dp_alloc(int size, int min_partition_size, int comp_frequency, void* memory, t_memory_algorithm memory_algorithm, t_selection_algorithm partition_algorithm, t_selection_algorithm victim_algorithm)
{
	t_partition* partition = NULL;

	while (partition == NULL) {
		if (partition_algorithm == FIRST_FIT) {
			partition = first_fit_find_free_partition(size);
		} else if (partition_algorithm == BEST_FIT) {
			partition = best_fit_find_free_partition(size);
		}


	}


	// TODO compectar antes de seleccionar victima chequeando frecuencia de compactacion y volver a buscar particion

	if (partition == NULL) {
		if (victim_algorithm == FIFO) {
			partition = fifo_find_victim_partition(size);
		} else if (victim_algorithm == LRU) {
			partition = lru_find_victim_partition(size);
		}
	}

	// TODO hacer algo con la particion

	return NULL;
}

t_partition* first_fit_find_free_partition(int size)
{
	t_link_element* element = FREE_PARTITIONS->head;

	if (element == NULL)
		return NULL;

	t_partition* partition = (t_partition*) element->data;

	int index = 0;
	while (element != NULL) {
		if (partition->size >= size)
			return (t_partition*) list_remove(FREE_PARTITIONS, index);

		element = element->next;
		partition = element == NULL ? NULL : element->data;
		index++;
	}

	return NULL;
}

t_partition* best_fit_find_free_partition(int size)
{
	t_link_element* element = FREE_PARTITIONS->head;

	if (element == NULL)
		return NULL;

	t_partition* partition = (t_partition*) element->data;
	t_partition* best_choise = partition;

	int index = 0;
	int best_index = 0;
	while (element != NULL) {
		if (partition->size == size)
			return (t_partition*) list_remove(FREE_PARTITIONS, index);

		if (partition->size > size && partition->size < best_choise->size) {
			best_index = index;
			best_choise = partition;
		}

		element = element->next;
		partition = element == NULL ? NULL : element->data;
		index++;
	}

	if (best_choise->size < size)
		return NULL;

	return (t_partition*) list_remove(FREE_PARTITIONS, best_index);
}

t_partition* fifo_find_victim_partition(int size)
{
	if (OCCUPIED_PARTITIONS->head == NULL)
		return NULL;

	return (t_partition*) list_remove(OCCUPIED_PARTITIONS, 0);
}

t_partition* lru_find_victim_partition(int size)
{
	//TODO en que me baso para decir que fue el lru?
	return NULL;
}
