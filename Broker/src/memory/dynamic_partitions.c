/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Dynamic Partitions Algorithm
 ============================================================================
 */

#include "dynamic_partitions.h"

//TODO SINCRO

void dp_init()
{
	FREE_PARTITIONS = list_create();
	OCCUPIED_PARTITIONS = list_create();

	t_partition initial_partition;
	initial_partition.id_data = 0;
	initial_partition.data = NULL;
	initial_partition.base = 0;
	initial_partition.size = MEMORY_SIZE;

	list_add(FREE_PARTITIONS, (void*) &initial_partition);
}

void* dp_alloc(int size)
{
	t_partition* partition = NULL;

	while (partition == NULL) {
		if (PARTITION_SELECTION_ALGORITHM == FIRST_FIT) {
			partition = first_fit_find_free_partition(size);
		} else if (PARTITION_SELECTION_ALGORITHM == BEST_FIT) {
			partition = best_fit_find_free_partition(size);
		}


	}


	// TODO compectar antes de seleccionar victima chequeando frecuencia de compactacion y volver a buscar particion

	if (partition == NULL) {
		if (VICTIM_SELECTION_ALGORITHM == FIFO) {
			partition = fifo_find_victim_partition(size);
		} else if (VICTIM_SELECTION_ALGORITHM == LRU) {
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
	if (lru_list->head == NULL)
		return NULL;

	return (t_partition*) list_remove(lru_list, 0);
}
