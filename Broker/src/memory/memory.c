/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Memory
 ============================================================================
 */

#include "memory.h"

pthread_mutex_t mutex_memory = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_lru_list = PTHREAD_MUTEX_INITIALIZER;

void load_memory(int size, int min_partition_size, int frequency, t_memory_algorithm memory_alg, t_selection_algorithm victim_alg, t_selection_algorithm partition_alg)
{
	MEMORY = malloc(size);
	MEMORY_SIZE = size;
	MIN_PARTITION_SIZE = min_partition_size;
	COMPACTION_FREQUENCY = frequency;
	MEMORY_ALGORITHM = memory_alg;
	PARTITION_SELECTION_ALGORITHM = victim_alg;
	VICTIM_SELECTION_ALGORITHM = partition_alg;

	lru_list = list_create();

	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		buddy_init(MEMORY_SIZE);
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		dp_init(MEMORY_SIZE);
	}
}

void* memory_alloc(int size)
{
	if (size > MEMORY_SIZE)
		return NULL;

	pthread_mutex_lock(&mutex_memory);
	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		return buddy_alloc(size);
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		return dp_alloc(size);
	}
	pthread_mutex_lock(&mutex_memory);

	return NULL;
}

void* memory_copy(t_copy_args* args)
{
	t_partition* partition = args->alloc;
	partition->id_data = args->id;

	void* data = memcpy(MEMORY + partition->base, args->data, partition->size);
	partition->data = data;

	free(args->data);
	return data;
}

int get_index_of_partition(t_list* partitions, uint32_t id_partition)
{
	if (partitions->head == NULL)
		return -1;

	t_link_element *element = partitions->head;
	t_partition* partition = (t_partition*) (partitions->head->data);

	int index = 0;
	while(element != NULL) {
		if (partition->id_data == id_partition)
			return index;

		element = element->next;
		partition = element == NULL ? NULL : element->data;
		index++;
	}

	return -1;
}

t_partition* get_partition_by_id(t_list* partitions, uint32_t id_partition)
{
	int index = get_index_of_partition(partitions, id_partition);

	return index >= 0 ? (t_partition*) list_get(partitions, index) : NULL;
}

void add_to_lru(t_partition* partition)
{
	pthread_mutex_lock(&mutex_lru_list);
	list_add(lru_list, partition);
	pthread_mutex_unlock(&mutex_lru_list);
}

void notify_message_used(uint32_t id_message)
{
	pthread_mutex_lock(&mutex_lru_list);
	int index_partition = get_index_of_partition(lru_list, id_message);

	t_partition* partition = (t_partition*) list_remove(lru_list, index_partition);
	list_add(lru_list, partition);
	pthread_mutex_unlock(&mutex_lru_list);
}
