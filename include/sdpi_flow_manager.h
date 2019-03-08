#ifndef _SDPI_FLOW_MANAGER_H
#define _SDPI_FLOW_MANAGER_H

#include <stdio.h>
#include <stddef.h>
#include "sdpi_flow_struct.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <pthread.h>
#endif 

#define SDPI_FLOW_TABLE_SIZE		131
#define SDPI_FLOW_BLOCK_SIZE		10000
#define SDPI_FLOW_IDLE_LIMIT		1000
#define SDPI_FLOW_INFORMATION_SIZE	100  //larger than sizeof(SDPI_inspect_RESULT)

#define sdpi_flow_entry_init(head) \
	((head)->prev = (head)->next = head)

#define sdpi_flow_entry_empty(head) \
	((head)->next == (head))

#define sdpi_flow_entry_next(entry) \
	((entry)->next)

#define sdpi_flow_entry_insert(head, entry) do{ \
	(entry)->next = (head)->next; \
	(entry)->prev = (head); \
	(head)->next->prev = (entry); \
	(head)->next = (entry); \
}while(0)

#define sdpi_flow_entry_remove(entry) do{ \
	(entry)->next->prev = (entry)->prev; \
	(entry)->prev->next = (entry)->next; \
	(entry)->prev = (entry)->next = (entry); \
}while(0)

#ifndef container_of
#define container_of(ptr, type, member) \
	((type *)((char *)ptr - offsetof(type, member)))
#endif

#if !defined FLOW_MANAGER_LOCK
typedef void * sdpi_lock_t;
#define create_lock(lock)	NULL
#define acquire_lock(lock)	(lock)
#define release_lock(lock)	(lock)
#define destroy_lock(lock)	NULL

#elif defined(_WIN32) || defined(_WIN64)
typedef CRITICAL_SECTION  sdpi_lock_t;
#define create_lock(lock)	InitializeCriticalSection(&lock)
#define acquire_lock(lock)	EnterCriticalSection(&lock)
#define release_lock(lock)	LeaveCriticalSection(&lock)
#define destroy_lock(lock)	DeleteCriticalSection(&lock)
#else
typedef pthread_mutex_t sdpi_lock_t;
#define create_lock(lock)	pthread_mutex_init(&(lock), NULL)
#define acquire_lock(lock)	pthread_mutex_lock(&(lock))
#define release_lock(lock)	pthread_mutex_unlock(&(lock))
#define destroy_lock(lock)	pthread_mutex_destroy(&(lock))
#endif 

typedef struct _sdpi_flow_member sdpi_flow_member_t;

typedef struct _sdpi_flow_manager sdpi_flow_manager_t;

typedef int (*pfn_flow_table_create)(
	sdpi_flow_manager_t *flow_manager,
	unsigned int table_size 
	);

typedef void (*pfn_flow_table_delete)(
	sdpi_flow_manager_t *flow_manager
	);

typedef int (*pfn_flow_memory_alloc)(
	sdpi_flow_manager_t *flow_manager,
	unsigned int block_size
	);

typedef void (*pfn_flow_memory_free)(
	sdpi_flow_manager_t *flow_manager
	);

typedef sdpi_flow_member_t * (*pfn_flow_member_alloc)(
	sdpi_flow_manager_t *flow_manager
	);

typedef void (*pfn_flow_member_free)(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	);

typedef sdpi_flow_member_t * (*pfn_flow_member_find)(
	sdpi_flow_manager_t *flow_manager, 
	sdpi_flow_base_t *flow_base
	);

typedef void (*pfn_flow_member_insert)(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	);

typedef void (*pfn_flow_member_remove)(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	);

typedef void (*pfn_flow_members_incref)(
	sdpi_flow_manager_t *flow_manager
	);

typedef void (*pfn_flow_member_rstref)(
	sdpi_flow_member_t *member
	);

typedef void (*pfn_flow_members_traverse)(
	sdpi_flow_manager_t *flow_manager
	);

typedef int (*pfn_flow_base_make_by_frame)(
	sdpi_flow_base_t *flow_base,
	char *frame,
	unsigned int frame_size
	);

typedef int (*pfn_flow_struct_make_by_frame)(
	sdpi_flow_struct_t *flow_struct,
	char *frame,
	unsigned int frame_size
	);

typedef void (*pfn_flow_base_make_by_fields)(
	sdpi_flow_base_t *flow_base,
	unsigned int protocol,
	unsigned int src_addr,
	unsigned int dst_addr,
	unsigned short src_port,
	unsigned short dst_port
	);

typedef int (*pfn_flow_struct_make_by_fields)(
	sdpi_flow_struct_t *flow_struct,
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	);

typedef sdpi_flow_member_t* (*pfn_flow_frame_process)(
	sdpi_flow_manager_t *flow_manager, 
	char *frame,
	unsigned int frame_size
	);

typedef sdpi_flow_member_t* (*pfn_flow_struct_process)(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	);

typedef struct _sdpi_flow_entry
{
	struct _sdpi_flow_entry *prev;
	struct _sdpi_flow_entry *next;
}sdpi_flow_entry_t;

typedef char sdpi_flow_information_t;

typedef struct _sdpi_flow_member
{
	//index of flow in hash table
	unsigned int 			hash_idx;

	//flow struct(including 5 tuple elements and payload)
	sdpi_flow_struct_t 		flow_struct;

	//flag to indicate if the flow has been inspected, useful
	//for pushing the flow info into db/memcache
	unsigned int 			is_inspected;

	//count to indicate whether the flow is active, if not, the 
	//flow would be delete from flow_manager
	unsigned int 			idle_time;

	//list of hash table, which is linked to head_entry
	sdpi_flow_entry_t 		table_entry;

	//list of memory block, which is linked to free_entry or busy_entry
	sdpi_flow_entry_t 		block_entry;

	//flow member information
	sdpi_flow_information_t	flow_detail[SDPI_FLOW_INFORMATION_SIZE];
}sdpi_flow_member_t;

typedef struct _sdpi_flow_table
{
	//table lock for flow_member's operation, such as insert/remove/...
	sdpi_lock_t 			table_lock;

	sdpi_flow_entry_t 		head_entry;

	unsigned int 			flow_number;

}sdpi_flow_table_t;

typedef struct _sdpi_flow_memory
{
	unsigned int 			total_size;

	sdpi_lock_t 			free_lock;

	sdpi_flow_entry_t 		free_entry;

	unsigned int 			free_size;

	sdpi_lock_t 			busy_lock;

	sdpi_flow_entry_t 		busy_entry;

	unsigned int 			busy_size;

}sdpi_flow_memory_t;

typedef struct _sdpi_flow_member_operations
{
	//new member from flow_memory, actually from free_block_list
	pfn_flow_member_alloc 		alloc_member;

	pfn_flow_member_free 		free_member;

	//insert member into flow_table, and record this member in busy_block_list
	pfn_flow_member_insert 		insert_member;

	//remove member from flow_table, and put it back into free_block_list  
	pfn_flow_member_remove 		remove_member;

	//find member in flow_table
	pfn_flow_member_find 		find_member;

	//increment the reference after timer triggered, and if 
	//idle_time is over MAX_IDLE_LIMIT, remove this member 
	//from flow_table and put it back into free_block_list

	pfn_flow_members_incref 	incref_members;

	//reset the reference to zero, if the member re-flaged, 
	//it happens while the flow keeps active.

	pfn_flow_member_rstref 		rstref_member;

	//just for test, traverse all members to check sth...
	pfn_flow_members_traverse 	print_members;

}sdpi_flow_member_operations_t;

typedef struct _sdpi_flow_struct_operations
{
	//flow structs operations...
	//construct flow base by packet frame
	pfn_flow_base_make_by_frame		fill_base_1;

	//construct flow base by fields 
	pfn_flow_base_make_by_fields 	fill_base_2; 	

	//construct flow struct by packet frame
	pfn_flow_struct_make_by_frame	fill_struct_1;

	//construct flow struct by fields
	pfn_flow_struct_make_by_fields 	fill_struct_2;

}sdpi_flow_struct_operations_t;

typedef struct _sdpi_flow_manager
{
	//hash table
	sdpi_flow_table_t 			   *flow_table;
	
	unsigned int					table_size;

	pfn_flow_table_create 			create_table;

	pfn_flow_table_delete 			delete_table;

	//double link list
	sdpi_flow_memory_t 			   *flow_memory;
	
	unsigned int					block_size;

	pfn_flow_memory_alloc			alloc_space;

	pfn_flow_memory_free 			free_space;

	sdpi_flow_member_operations_t 	flow_member_ops;
	
	sdpi_flow_struct_operations_t 	flow_struct_ops;

	//process flow frame, and return a flow member object
	pfn_flow_frame_process 			process_frame;

	//process flow fields, and return a flow member object
	pfn_flow_struct_process 		process_struct;
	
}sdpi_flow_manager_t;

//construction and deconstruction.
int
sdpi_create_flow_table(
	sdpi_flow_manager_t *flow_manager,
	unsigned int table_size
	);

void
sdpi_delete_flow_table(
	sdpi_flow_manager_t *flow_manager
	);

int
sdpi_alloc_flow_memory(
	sdpi_flow_manager_t *flow_manager,
	unsigned int block_size
	);

void
sdpi_free_flow_memory(
	sdpi_flow_manager_t *flow_manager
	);

int
sdpi_init_flow_manager(
	sdpi_flow_manager_t *flow_manager
	);

void
sdpi_free_flow_manager(
	sdpi_flow_manager_t *flow_manager
	);

//member handler
//alloc member from memory
sdpi_flow_member_t *
sdpi_alloc_flow_member(
	sdpi_flow_manager_t *flow_manager
	);

//find member by hash
sdpi_flow_member_t *
sdpi_find_flow_member(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_base_t *flow_base
	);

//insert member into table
void
sdpi_insert_flow_member(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	);

//remove member from table
void
sdpi_remove_flow_member(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	);

//free member into memory
void
sdpi_free_flow_member(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_member_t *member
	);

//increase reference idle_time when traverse 
void
sdpi_incref_flow_members(
	sdpi_flow_manager_t *flow_manager
	);

void
sdpi_rstref_flow_member(
	sdpi_flow_member_t *flow_member
);

void
sdpi_traverse_flow_members(
	sdpi_flow_manager_t *flow_manager
	);

void 
sdpi_delegate_flow_manager(
	sdpi_flow_manager_t *flow_manager
	);

sdpi_flow_member_t * sdpi_process_flow_frame(
	sdpi_flow_manager_t *flow_manager,
	char *frame,
	unsigned int frame_size
	);

sdpi_flow_member_t * sdpi_process_flow_struct(
	sdpi_flow_manager_t *flow_manager,
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	);

#endif