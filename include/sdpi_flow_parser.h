#ifndef _SDPI_FLOW_PARSER
#define _SDPI_FLOW_PARSER
#include <stdio.h>
#include "sdpi_flow_struct.h"
#include "sdpi_app_classify.h"

/*
	1、register: must implements pfn_inspect_context_init, pfn_inspect_context_free, pfn_inspect_routine
	2、register based on linklist
*/

#define SDPI_FLOW_DESCRIPTION_SIZE 	64

#define sdpi_inspect_entry_init(head) \
	((head)->prev = (head)->next = head)

#define sdpi_inspect_entry_empty(head) \
	((head)->next == head)

#define sdpi_inspect_entry_next(entry) \
	((entry)->next)

#define sdpi_inspect_entry_insert(head, entry) do{ \
		(entry)->next = (head)->next; \
		(entry)->prev = (head); \
		(head)->next->prev = (entry); \
		(head)->next = (entry); \
	}while(0)

#define sdpi_inspect_entry_remove(entry) do{ \
		(entry)->next->prev = (entry)->prev; \
		(entry)->prev->next = (entry)->next; \
		(entry)->prev = (entry)->next = (entry); \
	}while(0)

#ifndef container_of
#define container_of(ptr, type, member) \
	((type *)((char *)ptr - offsetof(type, member)))
#endif

typedef enum _sdpi_inspect_method
{
	SDPI_INSPECTED_BY_PROTOCOL,
	SDPI_INSPECTED_BY_PORT = 1,
	SDPI_INSPECTED_BY_IP = 2,
	SDPI_INSPECTED_BY_PAYLOAD = 4,
	SDPI_INSPECTED_BY_ALL = 7,
}sdpi_inspect_method_t;

typedef struct _sdpi_inspect_result
{
	sdpi_inspect_method_t 	method;
	sdpi_app_type_t			type;
	sdpi_app_category_t		category;
	char 					description[SDPI_FLOW_DESCRIPTION_SIZE];
}sdpi_inspect_result_t;

typedef struct _sdpi_inspect_entry 
{
	struct _sdpi_inspect_entry *prev;
	struct _sdpi_inspect_entry *next;
}sdpi_inspect_entry_t;

typedef struct _sdpi_inspect_context sdpi_inspect_context_t;

typedef int (*pfn_inspect_context_init)(
	sdpi_inspect_context_t *context
	);

typedef void (*pfn_inspect_context_free)(
	sdpi_inspect_context_t *context
	);

typedef void (*pfn_inspect_routine)(
	sdpi_inspect_context_t *context,	
	sdpi_flow_struct_t *flow_struct,
	sdpi_inspect_result_t *result
	);

typedef struct _sdpi_inspect_context
{
	//the structure used to store customed information, if you want to 
	//know more, refer to the implemention of port/ip/host inspection 
	void						*inspect_struct; 

	//init routine: construct the specific structure 'inspect_struct'
	pfn_inspect_context_init	context_init;

	//free_routine: destruct ...
	pfn_inspect_context_free	context_free;

	//inspect_routine: do specific inspection
	pfn_inspect_routine			inspect_routine;

}sdpi_inspect_context_t;

typedef struct _sdpi_inspect_manager sdpi_inspect_manager_t;

typedef int (*pfn_inspect_context_register)(
	sdpi_inspect_manager_t *inspect_manager, 
	sdpi_inspect_context_t *context
	);

typedef void (*pfn_inspect_context_unregister)(
	sdpi_inspect_context_t *context
	);

typedef void (*pfn_inspect_context_inspect)(
	sdpi_inspect_manager_t *inspect_manager,
	sdpi_flow_struct_t	  *flow_struct,
	sdpi_inspect_result_t  *result
	);

typedef struct _sdpi_inspect_member
{
	sdpi_inspect_entry_t	entry;
	sdpi_inspect_context_t  context;
}sdpi_inspect_member_t;

typedef struct _sdpi_inspect_manager
{
	sdpi_inspect_entry_t			leader;
	unsigned int					is_fired;					
	pfn_inspect_context_register	context_register;
	pfn_inspect_context_unregister	context_unregister;
	pfn_inspect_context_inspect		context_inspect;
}sdpi_inspect_manager_t;

void 
sdpi_init_inspect_manager(
	sdpi_inspect_manager_t *inspect_manager
	);

void 
sdpi_free_inspect_manager(
	sdpi_inspect_manager_t *inspect_manager
	);

int 
sdpi_register_inspect_context(
	sdpi_inspect_manager_t *inspect_manager,
	sdpi_inspect_context_t *context
	);

void 
sdpi_unregister_inspect_context(
	sdpi_inspect_context_t *context
	);

void
sdpi_call_inspectors(
	sdpi_inspect_manager_t *inspect_manager,
	sdpi_flow_struct_t *flow_struct,
	sdpi_inspect_result_t *result
	);

#endif