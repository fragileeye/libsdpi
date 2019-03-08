#include "sdpi_httpx_struct.h"
#include "sdpi_host_config.h"
#include "sdpi_host_inspector.h"

sdpi_str_patricia_tree_t *
sdpi_init_host_struct()
{
	int 						i;
	int 						list_size;
	char 						*host_name = NULL;
	sdpi_str_patricia_tree_t	*tree = NULL;
	sdpi_str_patricia_node_t	*node = NULL;

	if(!(tree = sdpi_str_patricia_init()))
	{
		return NULL;
	}

	list_size = sizeof(sdpi_host_config_list) / sizeof(sdpi_host_config_t);

	for(i = 0; i < list_size; ++i)
	{
		host_name = sdpi_host_config_list[i].host_name;

		node = sdpi_str_patricia_node_insert(tree, host_name);

		node->private_data = (void *)&(sdpi_host_config_list[i].result);
	}

	return tree;
}

int
sdpi_host_inspect_context_init(
	sdpi_inspect_context_t *context
	)
{
	sdpi_host_struct_t *sdpi_host_struct = sdpi_init_host_struct();

	if(sdpi_host_struct != NULL)
	{
		context->inspect_struct = sdpi_host_struct;
		return 0;
	}

	return -1;
}

void
sdpi_host_inspect_context_free(
	sdpi_inspect_context_t *context
	)
{
	sdpi_host_struct_t *sdpi_host_struct = (sdpi_host_struct_t *)context->inspect_struct;
	sdpi_str_patricia_free(sdpi_host_struct);
}

void
sdpi_host_inspector(
	sdpi_inspect_context_t *context,
	sdpi_flow_struct_t	*flow_struct,
	sdpi_inspect_result_t *result
	)
{
	sdpi_str_patricia_tree_t	*tree = (sdpi_str_patricia_tree_t *)context->inspect_struct;
	sdpi_str_patricia_node_t	*node = NULL;
	char 						host_name[SDPI_MAX_HOST_NAME_LENGTH] = { 0 };
	char						*valid_name = host_name;

	if(sdpi_inspect_hostname(flow_struct, host_name, sizeof(host_name)) == -1)
	{
		*result = sdpi_unknown_host_result;
		return;
	}

	*result = sdpi_unknown_host_result;

	while(*valid_name)
	{
		if((node = sdpi_str_patricia_node_lookup(tree, valid_name)))
		{
			*result = *(sdpi_inspect_result_t *)node->private_data;
			printf("Host inspected: %s\n", result->description);
			break;
		}

		while(*valid_name && *valid_name++ != '.');
	}
}