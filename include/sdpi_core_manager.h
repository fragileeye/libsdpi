#ifndef _SDPI_core_manager_H
#define _SDPI_core_manager_H
#include "sdpi_flow_manager.h"
#include "sdpi_flow_parser.h"

typedef struct _sdpi_core_result	 sdpi_core_result_t;
typedef struct _sdpi_core_manager	 sdpi_core_manager_t;
typedef struct _sdpi_inspect_context sdpi_core_context_t;

typedef int (*pfn_inspector_register)(
	sdpi_core_manager_t *core,
	sdpi_core_context_t *context
	);

typedef int (*pfn_flow_frame_inspector)(
	sdpi_core_manager_t *core,
	sdpi_core_result_t *core_result, //OUT
	char *frame,
	unsigned int frame_size
	);

typedef int (*pfn_flow_struct_inspector)(
	sdpi_core_manager_t *core,
	sdpi_core_result_t *core_result, //OUT
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	);

typedef struct _sdpi_core_result
{
	//reference the flow_struct of flow_member
	sdpi_flow_struct_t			*flow_struct;

	//reference the flow_detail of flow_member
	sdpi_inspect_result_t		*flow_result;

}sdpi_core_result_t;

typedef struct _sdpi_core_manager
{
	//flow manager: refer to sdpi_flow_manager.h
	sdpi_flow_manager_t			flow_manager;

	//flow parser: refer to sdpi_flow_parser.h
	sdpi_inspect_manager_t		inspect_manager;
	
	//register context
	pfn_inspector_register		register_inspector;

	//encapsulate sdpi_process_flow_frame, refer to sdpi_flow_manager.h
	pfn_flow_frame_inspector	inspect_packet;

	//encapsulate sdpi_process_flow_struct, refter to sdpi_flow_manager.h
	pfn_flow_struct_inspector	inspect_struct;

}sdpi_core_manager_t;


int 
sdpi_load_core_manager(
	sdpi_core_manager_t *core
	);

void 
sdpi_unload_core_manager(
	sdpi_core_manager_t *core
	);

int 
sdpi_register_inspector(
	sdpi_core_manager_t *core,
	sdpi_core_context_t *context
	);

int
sdpi_inspect_flow_frame(
	sdpi_core_manager_t *core,
	sdpi_core_result_t *core_result, //OUT
	char *frame,
	unsigned int frame_size
	);

int 
sdpi_inspect_flow_struct(
	sdpi_core_manager_t *core,
	sdpi_core_result_t *core_result, //OUT
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	);

#endif