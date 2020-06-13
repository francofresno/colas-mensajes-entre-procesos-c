/*
 ============================================================================
 Name        : Broker
 Author      : Fran and Co
 Description : Memory
 ============================================================================
 */

#include <string.h>

#include "memory.h"

pthread_mutex_t mutex_memory = PTHREAD_MUTEX_INITIALIZER;

void load_memory(int size, int min_partition_size, int frequency, t_memory_algorithm memory_alg, t_selection_algorithm victim_alg, t_selection_algorithm partition_alg)
{
	MEMORY = malloc(size);
	MEMORY_SIZE = size;
	MIN_PARTITION_SIZE = min_partition_size;
	COMPACTION_FREQUENCY = frequency;
	MEMORY_ALGORITHM = memory_alg;
	VICTIM_SELECTION_ALGORITHM = victim_alg;
	PARTITION_SELECTION_ALGORITHM = partition_alg;

	lru_list = list_create();
	deleted_messages_ids = list_create();
	ALL_PARTITIONS = list_create();

	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		buddy_init();
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		dp_init();
	}
}

void* memory_alloc(int size)
{
	if (size > MEMORY_SIZE)
		return NULL;

	void* allocated = NULL;
	pthread_mutex_lock(&mutex_memory);
	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		allocated = buddy_alloc(size);
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		allocated = dp_alloc(size);
	}

	if (allocated != NULL && VICTIM_SELECTION_ALGORITHM == LRU) {
		pthread_mutex_lock(&mutex_lru_list);
		add_to_lru(allocated);
		pthread_mutex_unlock(&mutex_lru_list);
	}
	pthread_mutex_unlock(&mutex_memory);

	return allocated;
}

void* memory_copy(t_copy_args* args)
{
	void* data = NULL;
	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		//TODO
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		t_partition* partition = args->alloc;
		partition->id_data = args->id;

		data = memcpy(MEMORY + partition->base, args->data, args->data_size); // TODO funciona bien pero valgrind dice invalid read of size X
		partition->data = data;

		log_new_message_in_memory(partition->id_data, partition->base);
	}

	return data;
}

void memory_dump()
{
	//TODO
	log_dump();
}

void* get_partition_by_id(t_list* partitions, uint32_t id_partition)
{
	int index = -1;

	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		index = get_index_of_buddy(partitions, id_partition);
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		index = get_index_of_partition(partitions, id_partition);
	}

	return index >= 0 ? list_get(partitions, index) : NULL;
}

void add_to_lru(void* partition)
{
	list_add(lru_list, partition);
}

void ids_message_destroyer(void* message)
{
	uint32_t* message_enqueue = (uint32_t*) message;
	free(message_enqueue);
}

t_list* get_victim_messages_ids(int* element_count)
{
	*element_count = list_size(deleted_messages_ids);
	return deleted_messages_ids;
}

void notify_all_victim_messages_deleted()
{
	list_clean_and_destroy_elements(deleted_messages_ids, ids_message_destroyer);
}

void notify_message_used(uint32_t id_message)
{
	if (VICTIM_SELECTION_ALGORITHM == LRU) {
		pthread_mutex_lock(&mutex_lru_list);
		int index = -1;

		if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
			index = get_index_of_buddy(lru_list, id_message);
		} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
			index = get_index_of_partition(lru_list, id_message);
		}

		if (index >= 0) {
			void* partition = list_remove(lru_list, index);
			add_to_lru(partition);
		}
		pthread_mutex_unlock(&mutex_lru_list);
	}
}
