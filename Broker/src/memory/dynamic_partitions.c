/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Dynamic Partitions Algorithm
 ============================================================================
 */

#include "dynamic_partitions.h"

void dp_init()
{
	SEARCH_FAILURE_COUNTER = 0;

	t_partition* initial_partition = malloc(sizeof(*initial_partition));
	initial_partition->id_data = 0;
	initial_partition->data = NULL;
	initial_partition->free = 1;
	initial_partition->base = 0;
	initial_partition->size = MEMORY_SIZE;

	list_add(FREE_PARTITIONS, (void*) initial_partition);
	list_add(ALL_PARTITIONS, (void*) initial_partition);
}

void* dp_alloc(int size)
{
	t_partition* partition = find_free_partition(size);

	int index_of_victim = -1;
	while (partition == NULL || partition->size < size) {
		index_of_victim = -1;
		compact_memory();
		partition = choose_victim_partition();
		if (partition != NULL) {
			partition->free = 1;
			partition->data = NULL;

			uint32_t* id_to_delete = malloc(sizeof(*id_to_delete));
			*id_to_delete = partition->id_data;
			pthread_mutex_lock(&mutex_deleted_messages_ids);
			list_add(deleted_messages_ids, (void*) id_to_delete);
			pthread_mutex_unlock(&mutex_deleted_messages_ids);

			log_deleted_partition(partition->base);

			index_of_victim = list_add(FREE_PARTITIONS, (void*) partition);
		}
	}

	if (partition->size > size && partition->size - size >= MIN_PARTITION_SIZE) {
		adjust_partition_size(partition, size);
	}

	partition->free = 0;
	list_add(OCCUPIED_PARTITIONS, (void*) partition);
	if (index_of_victim >= 0)
		list_remove(FREE_PARTITIONS, get_index_of_partition_by_base(FREE_PARTITIONS, partition->base));

	SEARCH_FAILURE_COUNTER = 0;

	return (void*) partition;
}

void adjust_partition_size(t_partition* partition, int size)
{
	t_partition* new_partition = malloc(sizeof(*new_partition));
	new_partition->data = NULL;
	new_partition->id_data = 0;
	new_partition->free = 1;

	if (size <= MIN_PARTITION_SIZE) {
		new_partition->size = partition->size - MIN_PARTITION_SIZE;
		new_partition->base = partition->base + MIN_PARTITION_SIZE;
		partition->size = MIN_PARTITION_SIZE;
	} else {
		new_partition->size = partition->size - size;
		new_partition->base = partition->base + size;
		partition->size = size;
	}

	list_add(FREE_PARTITIONS, (void*) new_partition);
	int index_of_partition = get_index_of_partition_by_base(ALL_PARTITIONS, partition->base);
	list_add_in_index(ALL_PARTITIONS, index_of_partition + 1, (void*) new_partition);
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
		return fifo_find_victim_partition();
	} else if (VICTIM_SELECTION_ALGORITHM == LRU) {
		return lru_find_victim_partition();
	}
	return NULL;
}

void compact_occupied_list(int* previous_occupied_base, int* previous_occupied_size)
{
	int occupied_list_size = list_size(OCCUPIED_PARTITIONS);
	if (occupied_list_size > 0) {

		t_partition* occupied_partition = list_get(OCCUPIED_PARTITIONS, 0);
		occupied_partition->base = 0;

		int previous_base = occupied_partition->base;
		int previous_size = occupied_partition->size;

		int index_occupied = 1;
		while(index_occupied < occupied_list_size) {
			occupied_partition = list_get(OCCUPIED_PARTITIONS, index_occupied);
			occupied_partition->base = previous_base + previous_size;
			previous_base = occupied_partition->base;
			previous_size = occupied_partition->size;
			index_occupied++;
		}
		*previous_occupied_base = previous_base;
		*previous_occupied_size = previous_size;
	}
}

void compact_free_list(int previous_base, int previous_size, int free_list_size)
{
	int size_compacted_partition = 0;
	t_partition* compacted_partition = malloc(sizeof(*compacted_partition));
	compacted_partition->data = NULL;
	compacted_partition->id_data = 0;
	compacted_partition->free = 1;
	compacted_partition->base = previous_base + previous_size;

	for (int i=0; i < free_list_size; i++) {
		t_partition* free_partition = list_get(FREE_PARTITIONS, i);
		size_compacted_partition += free_partition->size;
		list_remove(FREE_PARTITIONS, i);
		free(free_partition);
	}

	compacted_partition->size = size_compacted_partition;
	list_add(FREE_PARTITIONS, (void*) compacted_partition);
}

void sort_all_partitions_by_base()
{
	bool sort_by_base(void* partition1, void* partition2) {
		t_partition* partition = (t_partition*) partition1;
		t_partition* other_partition = (t_partition*) partition2;
		return partition->base > other_partition->base;
	}

	list_sort(ALL_PARTITIONS, sort_by_base);
}

void sort_memory_by_base(t_list* all_duplicated)
{
	int backup_size = 0;
	void get_backup_size(void* element) {
		t_partition* partition = (t_partition*) element;
		backup_size += partition->size;
	}
	list_iterate(OCCUPIED_PARTITIONS, get_backup_size);

	void* backup_memory = malloc(backup_size);
	void get_backup_copy(void* element) {
		t_partition* partition = (t_partition*) element;
		void* data = memcpy(backup_memory + partition->base, partition->data, partition->size);
		int index = get_index_of_partition_by_base(OCCUPIED_PARTITIONS, partition->base);
		t_partition* occ_partition = list_get(OCCUPIED_PARTITIONS, index);
		occ_partition->data = data;
	}
	list_iterate(OCCUPIED_PARTITIONS, get_backup_copy);

	memcpy(MEMORY, backup_memory, backup_size);
	free(backup_memory);
}

void compact_memory()
{
	SEARCH_FAILURE_COUNTER++;
	if (SEARCH_FAILURE_COUNTER == COMPACTION_FREQUENCY) {
		int free_list_size = list_size(FREE_PARTITIONS);
		if (free_list_size > 1) {
			int previous_base = 0;
			int previous_size = 0;
			compact_occupied_list(&previous_base, &previous_size);
			compact_free_list(previous_base, previous_size, free_list_size);

//			t_list* all_duplicated = list_duplicate(ALL_PARTITIONS); TODO
			sort_all_partitions_by_base();
//			sort_memory_by_base(all_duplicated);
//			list_destroy(all_duplicated);
		}
		log_compactation();
		SEARCH_FAILURE_COUNTER = 0;
	}
}

int get_index_of_partition_by_base(t_list* partitions, uint32_t base_partition)
{
	if (partitions->head == NULL)
		return -1;

	t_link_element *element = partitions->head;
	t_partition* partition = (t_partition*) (partitions->head->data);

	int index = 0;
	while(element != NULL) {
		if (partition->base == base_partition)
			return index;

		element = element->next;
		partition = element == NULL ? NULL : element->data;
		index++;
	}

	return -1;
}

void* find_data_partition_by_id(uint32_t id)
{
	t_partition* partition = find_partition_by_id(id);
	return partition != NULL ? partition->data : NULL;
}

t_partition* find_partition_by_id(uint32_t id)
{
	t_link_element* element = OCCUPIED_PARTITIONS->head;

	if (element == NULL)
		return NULL;

	t_partition* partition = (t_partition*) (element->data);

	while(element != NULL && partition->id_data != id) {
		element = element->next;
		partition = element == NULL ? NULL : element->data;
	}

	return partition;
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
	return (t_partition*) get_first(OCCUPIED_PARTITIONS);
}

t_partition* lru_find_victim_partition()
{
	return (t_partition*) get_first(lru_list);
}
