#include "sdpi_port_inspector.h"
#include "sdpi_port_config.h"

static sdpi_avl_tree_t *
sdpi_init_tport_struct(
	)
{
	int 			 i;
	int 			 list_size;
	int 			 port;
	sdpi_avl_tree_t *root = NULL;
	sdpi_avl_node_t *node = NULL;

	if(!(root = sdpi_avl_tree_init()))
	{
		fprintf(stderr, "%s: %s\n", __FUNCTION__, "sdpi_avl_tree_init failed!");
		return NULL;
	}

	list_size = sizeof(sdpi_tport_config_list) / sizeof(sdpi_port_config_t);

	for(i = 0; i < list_size; ++i)
	{
		port = sdpi_tport_config_list[i].port;

		node = sdpi_avl_tree_insert(root, port);

		node->private_data = (void *)&(sdpi_tport_config_list[i].result);
	}

	return root;
}

static sdpi_avl_tree_t *
sdpi_init_uport_struct(
	)
{
	int 			 i;
	int 			 list_size;
	int 			 port;
	sdpi_avl_tree_t *root = NULL;
	sdpi_avl_node_t *node = NULL;

	if(!(root = sdpi_avl_tree_init()))
	{
		fprintf(stderr, "%s: %s\n", __FUNCTION__, "sdpi_avl_tree_init failed!");
		return NULL;
	}

	list_size = sizeof(sdpi_uport_config_list) / sizeof(sdpi_port_config_t);

	for(i = 0; i < list_size; ++i)
	{
		port = sdpi_uport_config_list[i].port;

		node = sdpi_avl_tree_insert(root, port);

		node->private_data = (void *)&(sdpi_uport_config_list[i].result);
	}

	return root;
}

int
sdpi_port_inspect_context_init(
	sdpi_inspect_context_t *context
	)
{
	sdpi_port_struct_t	*sdpi_port_struct = NULL;	

	if(!(sdpi_port_struct = malloc(sizeof(sdpi_port_struct_t))))
	{
		fprintf(stderr, "fatal error: %s: %s\n", __FUNCTION__, "malloc failed!");
		return -1;
	}

	sdpi_port_struct->sdpi_tport_struct = sdpi_init_tport_struct();
	sdpi_port_struct->sdpi_uport_struct = sdpi_init_uport_struct();

	if(!sdpi_port_struct->sdpi_tport_struct || !sdpi_port_struct->sdpi_uport_struct)
	{
		return -1;
	}

	context->inspect_struct = sdpi_port_struct;
	return 0;
}

void
sdpi_port_inspect_context_free(
	sdpi_inspect_context_t *context
	)
{
	sdpi_port_struct_t *sdpi_port_struct = (sdpi_port_struct_t *)context->inspect_struct;

	sdpi_avl_tree_free(sdpi_port_struct->sdpi_tport_struct);
	sdpi_avl_tree_free(sdpi_port_struct->sdpi_uport_struct);

	free(sdpi_port_struct);
}

void
sdpi_port_inspector(
	sdpi_inspect_context_t *context,
	sdpi_flow_struct_t	*flow_struct,
	sdpi_inspect_result_t *result	
	)
{
	sdpi_port_struct_t	*sdpi_port_struct = (sdpi_port_struct_t *)context->inspect_struct;
	sdpi_flow_base_t 	*flow_base = &flow_struct->flow_base;
	sdpi_avl_tree_t		*sdpi_tport_root = sdpi_port_struct->sdpi_tport_struct;
	sdpi_avl_tree_t		*sdpi_uport_root = sdpi_port_struct->sdpi_uport_struct;
	sdpi_avl_tree_t		*sdpi_port_root = NULL;
	sdpi_avl_node_t		*lower_node = NULL;
	sdpi_avl_node_t		*upper_node = NULL;

	if(flow_base->protocol == SDPI_TCPPROTO_VALUE)
	{
		sdpi_port_root = sdpi_tport_root;
	}
	else if(flow_base->protocol == SDPI_UDPPROTO_VALUE)
	{
		sdpi_port_root = sdpi_uport_root;
	}
	else if(flow_base->protocol == SDPI_ICMPPROTO_VALUE)
	{
		result->method = SDPI_INSPECTED_BY_PROTOCOL;
		result->type = SDPI_APP_NETWORK_ICMP;
		result->category = SDPI_APP_CATEGORY_NETWORK;
		snprintf(result->description, sizeof(result->description), "%s", "icmp");
		return;
	}
	else
	{
		*result = sdpi_unknown_port_result;
		return;
	}

	lower_node = sdpi_avl_tree_lookup(sdpi_port_root, flow_base->lower_port);
	upper_node = sdpi_avl_tree_lookup(sdpi_port_root, flow_base->upper_port);

	if(!lower_node && !upper_node)
	{
		*result = sdpi_unknown_port_result;
		return;
	}

	if(upper_node)
	{
		*result = *(sdpi_inspect_result_t *)upper_node->private_data;
		printf("Port inspected: %s\n", result->description);
	}
	else
	{
		*result = *(sdpi_inspect_result_t *)lower_node->private_data;
		printf("Port inspected: %s\n", result->description);
	}
}
