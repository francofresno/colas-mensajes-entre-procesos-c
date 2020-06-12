/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Dynamic Partitions Algorithm
 ============================================================================
 */

#include "dynamic_partitions.h"

pthread_mutex_t mutex_free_list = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_occupied_list = PTHREAD_MUTEX_INITIALIZER;

//TODO SINCRO

void dp_init()
{
	FREE_PARTITIONS = list_create();
	OCCUPIED_PARTITIONS = list_create();

	SEARCH_FAILURE_COUNTER = 0;

	t_partition* initial_partition = malloc(sizeof(*initial_partition));
	initial_partition->id_data = 0;
	initial_partition->data = NULL;
	initial_partition->free = 1;
	initial_partition->base = 0;
	initial_partition->size = MEMORY_SIZE;

	list_add(FREE_PARTITIONS, (void*) initial_partition);
}

void* dp_alloc(int size)
{
	int index_victim_chosen = 0;
	t_partition* partition = find_free_partition(size);

	if (partition == NULL) {
		compact_memory();
		partition = choose_victim_partition();
		if (partition != NULL) {
			partition->free = 1;
			free(partition->data);
			partition->data = NULL;

			uint32_t* id_to_delete = malloc(sizeof(*id_to_delete));
			*id_to_delete = partition->id_data;
			pthread_mutex_lock(&mutex_deleted_messages_ids);
			list_add(deleted_messages_ids, (void*) id_to_delete);
			pthread_mutex_unlock(&mutex_deleted_messages_ids);

			index_victim_chosen = list_add(FREE_PARTITIONS, (void*) partition);
		}
	}

	if (partition == NULL || partition->size < size) {
		dp_alloc(size);
	}

	if (index_victim_chosen >= 0) {
		list_remove(FREE_PARTITIONS, index_victim_chosen);
	}

	if (partition->size > size && partition->size - size >= MIN_PARTITION_SIZE) {
		t_partition* new_partition = malloc(sizeof(*new_partition));
		new_partition->data = NULL;
		new_partition->id_data = 0;
		new_partition->free = 1;
		new_partition->size = partition->size - size;
		new_partition->base = partition->base + size + 1; // TODO chequear ese +1
		list_add(FREE_PARTITIONS, (void*) new_partition);

		partition->size = size;
	}

	partition->free = 0;
	list_add(OCCUPIED_PARTITIONS, (void*) partition);

	SEARCH_FAILURE_COUNTER = 0;

	return (void*) partition;
}

t_partition* find_free_partition(int size)
{
	if (PARTITION_SELECTION_ALGORITHM == FIRST_FIT) {
		return first_fit_find_free_partition(size);
	} else if (PARTITION_SELECTION_ALGORITHM == BEST_FIT) {
		return best_fit_find_free_partition(size);
	}
	return NULL;
}

t_partition* choose_victim_partition()
{
	if (VICTIM_SELECTION_ALGORITHM == FIFO) {
		return fifo_find_victim_partition(OCCUPIED_PARTITIONS);
	} else if (VICTIM_SELECTION_ALGORITHM == LRU) {
		return lru_find_victim_partition(OCCUPIED_PARTITIONS);
	}
	return NULL;
}

void compact_memory()
{
	SEARCH_FAILURE_COUNTER++;
	if (SEARCH_FAILURE_COUNTER == COMPACTION_FREQUENCY) {

		//TODO compacto, recordar freerear las particiones que se compactan

		SEARCH_FAILURE_COUNTER = 0;
	}
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

t_partition* fifo_find_victim_partition()
{
	if (OCCUPIED_PARTITIONS->head == NULL)
		return NULL;

	return (t_partition*) list_remove(OCCUPIED_PARTITIONS, 0);
}

t_partition* lru_find_victim_partition()
{
	if (lru_list->head == NULL)
		return NULL;

	return (t_partition*) list_remove(lru_list, 0);
}
