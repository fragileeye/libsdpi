#ifndef _SDPI_PORT_INSPECTOR_H
#define _SDPI_PORT_INSPECTOR_H

#include "sdpi_flow_parser.h"
#include "sdpi_port_struct.h"

int 
sdpi_port_inspect_context_init(
	sdpi_inspect_context_t *context
	);

void 
sdpi_port_inspect_context_free(
	sdpi_inspect_context_t *context
	);

void 
sdpi_port_inspector(
	sdpi_inspect_context_t *context,
	sdpi_flow_struct_t	*flow_struct,
	sdpi_inspect_result_t *result
	);


#endif
