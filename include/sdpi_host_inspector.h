#ifndef _SDPI_HOST_INSPECTOR_H
#define _SDPI_HOST_INSPECTOR_H

//construct inspector base on httpx inspector
#include "sdpi_httpx_struct.h"
#include "sdpi_host_struct.h"
#include "sdpi_flow_parser.h"

int
sdpi_host_inspect_context_init(
	sdpi_inspect_context_t *context
	);

void
sdpi_host_inspect_context_free(
	sdpi_inspect_context_t *context
	);

void
sdpi_host_inspector(
	sdpi_inspect_context_t *context,
	sdpi_flow_struct_t	*flow_struct,
	sdpi_inspect_result_t *result
	);

#endif