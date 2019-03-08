#include "sdpi_flow_manager.h"

int
sdpi_create_flow_table(
	sdpi_flow_manager_t *flow_manager,
	unsigned int table_size
	)
{
	unsigned int 		 i;
	sdpi_flow_table_t 	*flow_table = NULL;
	sdpi_flow_table_t 	*flow_node = NULL;

	if(!(flow_table = malloc(table_size * sizeof(sdpi_flow_table_t))))
	{
		return -1;
	}

	flow_manager->flow_table = flow_table;
	flow_manager->table_size = table_size;

	for(i = 0; i < table_size; ++i)
	{
		flow_node = &flow_table[i];

		create_lock(flow_node->table_lock);

		flow_node->flow_number = 0;

		sdpi_flow_entry_init(&flow_node->head_entry);
	}

	return 0;
}

void
sdpi_delete_flow_table(
	sdpi_flow_manager_t *flow_manager
	)
{
	unsigned int 		i;
	sdpi_flow_entry_t 	*next_entry = NULL;
	sdpi_flow_table_t 	*flow_node = NULL;
	sdpi_flow_member_t 	*flow_member = NULL;

	for(i = 0; i < flow_manager->table_size; ++i)
	{
		flow_node = &flow_manager->flow_table[i];

		acquire_lock(flow_node->table_lock);

		while(flow_node->flow_number > 0)
		{
			next_entry = sdpi_flow_entry_next(&flow_node->head_entry);

			flow_member = container_of(next_entry, sdpi_flow_member_t, table_entry);

			sdpi_remove_flow_member(flow_manager, flow_member);

			sdpi_free_flow_member(flow_manager, flow_member);
		}

		release_lock(flow_node->table_lock);
		destroy_lock(flow_node->table_lock);
	}
}

int
sdpi_alloc_flow_memory(
	sdpi_flow_manager_t *flow_manager,
	unsigned int block_size
	)
{
	unsigned int 		i;
	sdpi_flow_memory_t 	*flow_memory = NULL;
	sdpi_flow_member_t 	*flow_member = NULL;

	if(!(flow_memory = malloc(sizeof(sdpi_flow_memory_t))))
	{
		return -1;
	}

	flow_memory->total_size = block_size;
	flow_memory->free_size = block_size;
	flow_memory->busy_size = 0;

	create_lock(flow_memory->free_lock);

	create_lock(flow_memory->busy_lock);

	sdpi_flow_entry_init(&flow_memory->free_entry);

	sdpi_flow_entry_init(&flow_memory->busy_entry);

	for(i = 0; i < block_size; ++i)
	{
		if(!(flow_member = malloc(sizeof(sdpi_flow_member_t))))
		{
			return -1;
		}

		sdpi_flow_entry_init(&flow_member->block_entry);

		sdpi_flow_entry_insert(&flow_memory->free_entry, &flow_member->block_entry);
	}

	flow_manager->flow_memory = flow_memory;
	flow_manager->block_size = block_size;

	return 0;
}

void
sdpi_free_flow_memory(
	sdpi_flow_manager_t *flow_manager
	)
{
	int 				block_size = flow_manager->block_size;
	sdpi_flow_memory_t *flow_memory = flow_manager->flow_memory;
	sdpi_flow_member_t *flow_member = NULL;
	sdpi_flow_entry_t  *next_entry = NULL;

	acquire_lock(flow_memory->free_lock);

	while(flow_memory->free_size > 0)
	{
		next_entry = sdpi_flow_entry_next(&flow_memory->free_entry);

		sdpi_flow_entry_remove(next_entry);

		flow_member = container_of(next_entry, sdpi_flow_member_t, block_entry);

		flow_memory->free_size--;

		free(flow_member);
	}

	release_lock(flow_memory->free_lock);

	destroy_lock(flow_memory->free_lock);
	destroy_lock(flow_memory->busy_lock);
}

static void
sdpi_delegate_flow_manager(
	sdpi_flow_manager_t *flow_manager
	)
{
	sdpi_flow_member_operations_t *flow_member_ops = &flow_manager->flow_member_ops;
	sdpi_flow_struct_operations_t *flow_struct_ops = &flow_manager->flow_struct_ops;

	flow_member_ops->alloc_member	=	sdpi_alloc_flow_member;
	flow_member_ops->find_member	=	sdpi_find_flow_member;
	flow_member_ops->free_member	=	sdpi_free_flow_member;
	flow_member_ops->insert_member	=	sdpi_insert_flow_member;
	flow_member_ops->remove_member	=	sdpi_remove_flow_member;
	flow_member_ops->incref_members =	sdpi_incref_flow_members;
	flow_member_ops->rstref_member  =	sdpi_rstref_flow_member;
	flow_member_ops->print_members	=	sdpi_traverse_flow_members;

	flow_struct_ops->fill_base_1	=	sdpi_make_flow_base_by_frame;
	flow_struct_ops->fill_base_2	=	sdpi_make_flow_base_by_fields;
	flow_struct_ops->fill_struct_1	=	sdpi_make_flow_struct_by_frame;
	flow_struct_ops->fill_struct_2	=	sdpi_make_flow_struct_by_fields;

	flow_manager->create_table		=	sdpi_create_flow_table;
	flow_manager->delete_table		=	sdpi_delete_flow_table;

	flow_manager->alloc_space		=	sdpi_alloc_flow_memory;
	flow_manager->free_space		=	sdpi_free_flow_memory;

	flow_manager->process_frame		=	sdpi_process_flow_frame;
	flow_manager->process_struct	=	sdpi_process_flow_struct;
}

int
sdpi_init_flow_manager(
	sdpi_flow_manager_t *flow_manager
	)
{
	sdpi_delegate_flow_manager(flow_manager);

	if(-1 == flow_manager->create_table(flow_manager, SDPI_FLOW_TABLE_SIZE))
	{
		fprintf(stderr, "%s: %s\n", __FUNCTION__, "sdpi_init_flow_table failed!");
		return -1;
	}

	if(-1 == flow_manager->alloc_space(flow_manager, SDPI_FLOW_BLOCK_SIZE))
	{
		fprintf(stderr, "%s: %s\n", __FUNCTION__, "sdpi_init_flow_memory failed!");
		return -1;
	}

	return 0;
}

void
sdpi_free_flow_manager(
	sdpi_flow_manager_t *flow_manager
	)
{
	//MUST free flow_table first, because this routine would exchange the flow_member 
	//which flow_table obtained from busy_list into free_list, and then free flow_member 
	//in free_list
	
	flow_manager->delete_table(flow_manager);
	flow_manager->free_space(flow_manager);
}

sdpi_flow_member_t *
sdpi_alloc_flow_member(
	sdpi_flow_manager_t *flow_manager
	)
{
	sdpi_flow_memory_t 	*flow_memory = flow_manager->flow_memory;
	sdpi_flow_member_t 	*flow_member = NULL;
	sdpi_flow_entry_t 	*next_entry = NULL;
	int 		 		 clear_size = 0;

	acquire_lock(flow_memory->free_lock);

	next_entry = sdpi_flow_entry_next(&flow_memory->free_entry);

	if(next_entry == &flow_memory->free_entry)
	{
		fprintf(stderr, "%s: %s\n", __FUNCTION__, "no memory available!");
		return NULL;
	}

	sdpi_flow_entry_remove(next_entry);

	flow_memory->free_size--;

	release_lock(flow_memory->free_lock);

	acquire_lock(flow_memory->busy_lock);

	sdpi_flow_entry_insert(&flow_memory->busy_entry, next_entry);

	flow_memory->busy_size++;

	release_lock(flow_memory->busy_lock);

	flow_member = container_of(next_entry, sdpi_flow_member_t, block_entry);

	//don't clear table_entry and block_entry!
	clear_size = sizeof(sdpi_flow_member_t) - 2 * sizeof(sdpi_flow_entry_t);

	memset(flow_member, 0, clear_size);

	return flow_member;
}

//find member by hash
sdpi_flow_member_t *
sdpi_find_flow_member(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_base_t *flow_base
	)
{
	sdpi_flow_table_t 	*flow_node = NULL;
	sdpi_flow_member_t 	*flow_member = NULL;
	sdpi_flow_struct_t	*flow_struct = NULL;
	sdpi_flow_base_t	*this_base = NULL;
	sdpi_flow_entry_t 	*next_entry = NULL;
	unsigned int 		 hash_idx = 0;
	unsigned int		 cmp_value = 0;

	hash_idx = (flow_base->upper_addr + flow_base->protocol + 
				flow_base->lower_addr) % SDPI_FLOW_TABLE_SIZE;

	flow_node = &flow_manager->flow_table[hash_idx];

	acquire_lock(flow_node->table_lock);

	next_entry = sdpi_flow_entry_next(&flow_node->head_entry);

	release_lock(flow_node->table_lock);

	while(next_entry != &flow_node->head_entry)
	{
		flow_member = container_of(next_entry, sdpi_flow_member_t, table_entry);

		flow_struct = &flow_member->flow_struct;

		this_base = &flow_struct->flow_base;

		//3 tuples hash, also called session hash
		cmp_value = this_base->protocol == flow_base->protocol &&
					this_base->lower_addr == flow_base->lower_addr && 
					this_base->upper_addr == flow_base->upper_addr;

		if(!cmp_value)
		{
			return flow_member;
		}

		next_entry = sdpi_flow_entry_next(next_entry);
	}

	return NULL;
}

//insert member into table
void
sdpi_insert_flow_member(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	)
{
	int 				 hash_idx = 0;
	sdpi_flow_struct_t	*flow_struct = &member->flow_struct;
	sdpi_flow_base_t	*flow_base = &flow_struct->flow_base;
	sdpi_flow_table_t 	*flow_table = flow_manager->flow_table;
	sdpi_flow_table_t 	*flow_node = NULL;

	hash_idx = (flow_base->lower_addr + flow_base->protocol + 
				flow_base->upper_addr) % SDPI_FLOW_TABLE_SIZE;

	member->hash_idx = hash_idx;

	flow_node = &flow_table[hash_idx];

	acquire_lock(flow_node->table_lock);

	sdpi_flow_entry_insert(&flow_node->head_entry, &member->table_entry);

	flow_node->flow_number++;

	release_lock(flow_node->table_lock);
}

//remove member from table
void
sdpi_remove_flow_member(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	)
{
	int 				 hash_idx = member->hash_idx;
	sdpi_flow_table_t	*flow_table = flow_manager->flow_table;
	sdpi_flow_table_t 	*flow_node = &flow_table[hash_idx];

	member->hash_idx = 0;

	acquire_lock(flow_node->table_lock);

	sdpi_flow_entry_remove(&member->table_entry);

	flow_node->flow_number--;

	release_lock(flow_node->table_lock);
}

//free member into memory
void
sdpi_free_flow_member(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	)
{
	sdpi_flow_memory_t 	*flow_memory = flow_manager->flow_memory;

	acquire_lock(flow_memory->busy_lock);

	sdpi_flow_entry_remove(&member->block_entry);

	flow_memory->busy_size--;

	release_lock(flow_memory->busy_lock);

	acquire_lock(flow_memory->free_lock);

	sdpi_flow_entry_insert(&flow_memory->free_entry, &member->block_entry);

	flow_memory->free_size++;

	release_lock(flow_memory->free_lock);
}

void
sdpi_incref_flow_members(
	sdpi_flow_manager_t *flow_manager
	)
{
	sdpi_flow_memory_t				*flow_memory = NULL;
	sdpi_flow_entry_t 				*next_entry = NULL;
	sdpi_flow_member_t 				*flow_member = NULL;
	sdpi_flow_member_operations_t	*member_ops = &flow_manager->flow_member_ops;

	flow_memory = flow_manager->flow_memory;

	//NOTE: no need lock the entire traversing handler, due to the new entry would
	//be inserted after head_entry which can't be visited this time, so just lock when 
	//fetch the first next_entry after head_entry.

	acquire_lock(flow_memory->busy_lock);

	next_entry = sdpi_flow_entry_next(&flow_memory->busy_entry);

	release_lock(flow_memory->busy_lock);

	while(next_entry != &flow_memory->busy_entry)
	{
		flow_member = container_of(next_entry, sdpi_flow_member_t, block_entry);
		
		//notice: must get next entry here, or we could got a pointer error 
		//when free member from busy list, 

		next_entry = sdpi_flow_entry_next(next_entry);

		if(++flow_member->idle_time >= SDPI_FLOW_IDLE_LIMIT)
		{
			//remove from hash table
			member_ops->remove_member(flow_manager, flow_member);

			//free from busy list and back to free list
			member_ops->free_member(flow_manager, flow_member);
		}
	}
}

void
sdpi_rstref_flow_member(
	sdpi_flow_member_t *flow_member
	)
{
	flow_member->idle_time = 0;
}

//traverse flow member in busy_list.
void 
sdpi_traverse_flow_members(
	sdpi_flow_manager_t *flow_manager
	)
{
	sdpi_flow_memory_t	*flow_memory = flow_manager->flow_memory;
	sdpi_flow_member_t	*member = NULL;
	unsigned int		busy_size = 0;
	unsigned int		free_size = 0;
	sdpi_flow_entry_t	*busy_entry = NULL;

	acquire_lock(flow_memory->busy_lock);

	free_size = flow_memory->free_size;
	
	busy_size = flow_memory->busy_size;
	
	busy_entry = sdpi_flow_entry_next(&flow_memory->busy_entry);

	release_lock(flow_memory->busy_lock);

	printf("[FREE]: %9d | [BUSY: %9d]\n", free_size, busy_size);

	//NOTE: no need lock the entire traversing handler, due to the new entry would
	//be inserted after head_entry which can't be visited this time, so just lock when 
	//fetch the first next_entry after head_entry.
	/*while(busy_entry != &flow_memory->busy_entry)
	{
		member = container_of(busy_entry, sdpi_flow_member_t, block_entry);
		printf("[hash]: %5d | [idle]: %5d\n", member->hash_idx, member->idle_time);
		busy_entry = sdpi_flow_entry_next(busy_entry);
	}*/
}


sdpi_flow_member_t* sdpi_process_flow_frame(
	sdpi_flow_manager_t *flow_manager,
	char *frame,
	unsigned int frame_size
	)
{
	sdpi_flow_base_t				flow_base;
	sdpi_flow_member_t				*flow_member = NULL;
	sdpi_flow_member_operations_t	*member_ops = &flow_manager->flow_member_ops;
	sdpi_flow_struct_operations_t	*struct_ops = &flow_manager->flow_struct_ops;

	if(-1 == struct_ops->fill_base_1(&flow_base, frame, frame_size))
	{
		fprintf(stderr, "%s: %s\n", __FUNCTION__, "sdpi_make_flow_base_by_frame failed!");
		return NULL;
	}

	if(!(flow_member = member_ops->find_member(flow_manager, &flow_base)))
	{
		if(!(flow_member = member_ops->alloc_member(flow_manager)))
		{
			fprintf(stderr, "%s: %s\n", __FUNCTION__, "sdpi_alloc_flow_member failed!");
			return NULL;
		}

		struct_ops->fill_struct_1(&flow_member->flow_struct, frame, frame_size);

		member_ops->insert_member(flow_manager, flow_member);
	}
	else
	{
		struct_ops->fill_struct_1(&flow_member->flow_struct, frame, frame_size);
	}
	
	member_ops->rstref_member(flow_member);

	member_ops->incref_members(flow_manager);

	return flow_member;
}

sdpi_flow_member_t* sdpi_process_flow_struct(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	)
{
	sdpi_flow_member_t				*flow_member = NULL;
	sdpi_flow_member_operations_t	*member_ops = &flow_manager->flow_member_ops;
	sdpi_flow_struct_operations_t	*struct_ops = &flow_manager->flow_struct_ops;

	if(!(flow_member = member_ops->find_member(flow_manager, flow_base)))
	{
		if(!(flow_member = member_ops->alloc_member(flow_manager)))
		{
			fprintf(stderr, "%s: %s\n", __FUNCTION__, "sdpi_alloc_flow_member failed!");
			return NULL;
		}

		struct_ops->fill_struct_2(&flow_member->flow_struct,
					flow_base, direction, payload, payload_size);

		member_ops->insert_member(flow_manager, flow_member);
	}
	else
	{
		struct_ops->fill_struct_2(&flow_member->flow_struct,
					flow_base, direction, payload, payload_size);
	}

	member_ops->rstref_member(flow_member);

	member_ops->incref_members(flow_manager);

	return flow_member;
}