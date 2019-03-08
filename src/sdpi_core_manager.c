#include "sdpi_core_manager.h"

int
sdpi_load_core_manager(
	sdpi_core_manager_t *core
	)
{
	if(-1 == sdpi_init_flow_manager(&core->flow_manager))
	{
		fprintf(stderr, "%s: %s\n", __FUNCTION__, "sdpi_init_flow_manager failed!");
		return -1;
	}

	sdpi_init_inspect_manager(&core->inspect_manager);

	core->inspect_packet	 =  sdpi_inspect_flow_frame;
	core->inspect_struct	 =  sdpi_inspect_flow_struct;
	core->register_inspector =  sdpi_register_inspector;

	return 0;
}

void
sdpi_unload_core_manager(
	sdpi_core_manager_t *core
	)
{
	sdpi_free_flow_manager(&core->flow_manager);
	sdpi_free_inspect_manager(&core->inspect_manager);
}

//if return value = -1, this registering procedure must fail
//and the inspector routine wouldn't be called.

int
sdpi_register_inspector(
	sdpi_core_manager_t *core,
	sdpi_core_context_t *context
	)
{
	sdpi_inspect_manager_t *im = &core->inspect_manager;

	return im->context_register(im, context);
}

int
sdpi_inspect_flow_frame(
	sdpi_core_manager_t *core,
	sdpi_core_result_t *core_result,
	char *frame,
	unsigned int frame_size
	)
{
	sdpi_flow_member_t		*flow_member = NULL;
	sdpi_flow_struct_t		*flow_struct = NULL;
	sdpi_flow_manager_t		*flow_manager = &core->flow_manager;
	sdpi_inspect_manager_t	*inspect_manager = &core->inspect_manager;
	sdpi_inspect_result_t	*flow_detail = NULL;

	flow_member = flow_manager->process_frame(flow_manager, frame, frame_size);

	if(!flow_member)
	{
		//if the returned member is null, it indicates that the no member available now
		return -1;
	}

	flow_struct = &flow_member->flow_struct;

	flow_detail = (sdpi_inspect_result_t *)flow_member->flow_detail;

	inspect_manager->context_inspect(inspect_manager, flow_struct, flow_detail);

	if(flow_detail->type != SDPI_APP_UNKNOWN)
	{
		flow_member->is_inspected = 1;
	}

	core_result->flow_result = flow_detail;
	core_result->flow_struct = flow_struct;

	return 0;
}

int
sdpi_inspect_flow_struct(
	sdpi_core_manager_t *core,
	sdpi_core_result_t *core_result,
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	)
{
	sdpi_flow_member_t		*flow_member = NULL;
	sdpi_flow_struct_t		*flow_struct = NULL;
	sdpi_flow_manager_t		*flow_manager = &core->flow_manager;
	sdpi_inspect_manager_t	*inspect_manager = &core->inspect_manager;
	sdpi_inspect_result_t	*flow_detail = NULL;

	flow_member = flow_manager->process_struct(
		flow_manager, flow_base, direction, payload, payload_size);

	if(!flow_member)
	{
		//if the returned member is null, it indicates that the no member available now
		return -1;
	}

	flow_struct = &flow_member->flow_struct;

	flow_detail = (sdpi_inspect_result_t *)flow_member->flow_detail;

	inspect_manager->context_inspect(inspect_manager, flow_struct, flow_detail);

	if(flow_detail->type != SDPI_APP_UNKNOWN)
	{
		flow_member->is_inspected = 1;
	}

	core_result->flow_result = flow_detail;
	core_result->flow_struct = flow_struct;

	return 0;
}