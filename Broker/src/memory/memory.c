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

void load_memory(int size, int min_partition_size, int frequency, t_memory_algorithm memory_alg, t_selection_algorithm victim_alg, t_selection_algorithm partition_alg, char* dump_path)
{
	MEMORY = malloc(size);
	MEMORY_SIZE = size;
	MIN_PARTITION_SIZE = min_partition_size;
	COMPACTION_FREQUENCY = frequency;
	MEMORY_ALGORITHM = memory_alg;
	VICTIM_SELECTION_ALGORITHM = victim_alg;
	PARTITION_SELECTION_ALGORITHM = partition_alg;
	DUMP_PATH = dump_path;

	lru_list = list_create();
	deleted_messages_ids = list_create();
	FREE_PARTITIONS = list_create();
	OCCUPIED_PARTITIONS = list_create();
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
		t_buddy* buddy = args->alloc;
		buddy->id_data = args->id;
		buddy->queue = args->queue;

		data = memcpy(MEMORY + buddy->base, args->data, args->data_size);
		buddy->data = data;

		log_new_message_in_memory(buddy->id_data, buddy->base);
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		t_partition* partition = args->alloc;
		partition->id_data = args->id;
		partition->queue = args->queue;

		data = memcpy(MEMORY + partition->base, args->data, args->data_size); // TODO funciona bien pero valgrind dice invalid read of size X
		partition->data = data;

		log_new_message_in_memory(partition->id_data, partition->base);
	}

	return data;
}

void write_dump_time_info(FILE* dump_file)
{

}

void write_partitions_info(FILE* dump_file)
{
	int partition_number = 1;
	int base, limit, size, lru, id, free;
	char* queue;

	int partitions_size = list_size(ALL_PARTITIONS);
	for (int i=0; i < partitions_size; i++) {
		if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
			t_buddy* buddy = (t_buddy*) list_get(ALL_PARTITIONS, i);
			base = buddy->base;
			size = buddy->size;
			limit = base + size;
			queue = op_code_a_string(buddy->queue);
			id = buddy->id_data;
			free = buddy->free;
			lru = 0; //TODO both
		} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
			t_partition* partition = (t_partition*) list_get(ALL_PARTITIONS, i);
			base = partition->base;
			size = partition->size;
			limit = base + size;
			queue = op_code_a_string(partition->queue);
			id = partition->id_data;
			free = partition->free;
			lru = 0;
		}
		if (free)
			fprintf(dump_file,"Partición %d: %d - %d.    [X]    Size: %db    LRU:<%d>    Cola:<%s>    ID:<%d>\n", partition_number, base, limit, size, lru, queue, id);
		else
			fprintf(dump_file,"Partición %d: %d - %d.    [L]    Size: %db\n", partition_number, base, limit, size);

		partition_number++;
	}

}

void memory_dump()
{
	FILE* dump_file = fopen(DUMP_PATH, "w");

	const char* line = "-----------------------------------------------------------------------------------------------------------------------------\n";

	fprintf(dump_file,"%s", line);
	write_dump_time_info(dump_file);
	write_partitions_info(dump_file);
	fprintf(dump_file,"%s", line);

	log_dump();
	fclose(dump_file);
}

//const char* dump_time = "%d/%d/%d %d:%d:%d\n";

//time_t rawtime;
//   struct tm * timeinfo;
//
//   time ( &rawtime );
//   timeinfo = localtime ( &rawtime );
//
//   printf("\n%d/%d/%d %d:%d:%d\n",timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);



void* get_partition_by_id(t_list* partitions, uint32_t id_partition)
{
	int index = -1;

	if (MEMORY_ALGORITHM == BUDDY_SYSTEM) {
		index = get_index_of_buddy_by_base(partitions, id_partition);
	} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
		index = get_index_of_partition_by_base(partitions, id_partition);
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
			index = get_index_of_buddy_by_base(lru_list, id_message);
		} else if (MEMORY_ALGORITHM == DYNAMIC_PARTITIONS) {
			index = get_index_of_partition_by_base(lru_list, id_message);
		}

		if (index >= 0) {
			void* partition = list_remove(lru_list, index);
			add_to_lru(partition);
		}
		pthread_mutex_unlock(&mutex_lru_list);
	}
}

void end_memory()  //TODO
{

}
