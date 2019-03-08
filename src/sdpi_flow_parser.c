#include "sdpi_flow_parser.h"

int
sdpi_register_inspect_context(
	sdpi_inspect_manager_t *inspect_manager,
	sdpi_inspect_context_t *context
	)
{
	sdpi_inspect_member_t	*member = NULL;

	if(!(member = malloc(sizeof(sdpi_inspect_member_t))))
	{
		fprintf(stderr, "%s: malloc failed!", __FUNCTION__);
		return -1;
	}

	//init routine called here MUST NOT BE NULL，you must define your own init routine 
	//even if you do nothing except return 0;
	if(context->context_init(context) == 0)
	{
		member->context = *context;
		sdpi_inspect_entry_insert(&inspect_manager->leader, &member->entry);
		return 0;
	}

	free(member);
	return -1;
}

void 
sdpi_unregister_inspect_context(
	sdpi_inspect_context_t *context
	)
{
	sdpi_inspect_member_t *member = NULL;

	if(context)
	{
		member = container_of(context, sdpi_inspect_member_t, context);

		//free routine could be NULL
		if(context->context_free)
		{
			context->context_free(context);
		}

		sdpi_inspect_entry_remove(&member->entry);

		free(member);
	}
}

void
sdpi_call_inspectors(
	sdpi_inspect_manager_t *inspect_manager,
	sdpi_flow_struct_t *flow_struct,
	sdpi_inspect_result_t *result
	)
{
	sdpi_inspect_entry_t	*head = &inspect_manager->leader;
	sdpi_inspect_entry_t	*curr_entry = NULL;
	sdpi_inspect_member_t	*curr_node = NULL;
	sdpi_inspect_context_t	*context = NULL;

	for(curr_entry = head->next; curr_entry != head; curr_entry = curr_entry->next)
	{
		curr_node = container_of(curr_entry, sdpi_inspect_member_t, entry);

		context = &curr_node->context;

		context->inspect_routine(context, flow_struct, result);

		if(result->type != SDPI_APP_UNKNOWN)  break;
	}
}

void
sdpi_init_inspect_manager(
	sdpi_inspect_manager_t *inspect_manager
	)
{
	sdpi_inspect_entry_init(&inspect_manager->leader);

	inspect_manager->context_register = sdpi_register_inspect_context;

	inspect_manager->context_unregister = sdpi_unregister_inspect_context;

	inspect_manager->context_inspect = sdpi_call_inspectors;
}

void
sdpi_free_inspect_manager(
	sdpi_inspect_manager_t *inspect_manager
	)
{
	sdpi_inspect_member_t	*member = NULL;
	sdpi_inspect_context_t	*context = NULL;
	sdpi_inspect_entry_t	*entry = NULL;
	sdpi_inspect_entry_t	*head = &inspect_manager->leader;
	
	while(!sdpi_inspect_entry_empty(head))
	{
		entry = sdpi_inspect_entry_next(head);

		member = container_of(entry, sdpi_inspect_member_t, entry);

		context = &member->context;

		//unregister contains 3 steps: 
		//1. context sucide(call free routine; 
		//2. pick off from the manager list; 
		//3. free member node
		inspect_manager->context_unregister(context);
	}
}

