#ifndef _SDPI_IP_INSPECTOR_H
#define _SDPI_IP_INSPECTOR_H

#include "sdpi_flow_parser.h"
#include "sdpi_ip_struct.h"

int
sdpi_ip_inspect_context_init(
	sdpi_inspect_context_t *context
	);

void
sdpi_ip_inspect_context_free(
	sdpi_inspect_context_t *context
	);

void
sdpi_ip_inspector(
	sdpi_inspect_context_t *context,
	sdpi_flow_struct_t	*flow_struct,
	sdpi_inspect_result_t *result
	);
#endif


