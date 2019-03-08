#include "sdpi_ip_inspector.h"
#include "sdpi_ip_config.h"

static 
sdpi_num_patricia_tree_t *
sdpi_init_ip_struct(
	)
{
	int 						i;
	int 						list_size;
	int 						addr;
	int 						bits;
	sdpi_ip_mask_t 				*ip_mask = NULL;
	sdpi_num_patricia_tree_t	*root = NULL;
	sdpi_num_patricia_node_t	*node = NULL;

	if(!(root = sdpi_num_patricia_init()))
	{
		return NULL;
	}

	list_size = sizeof(sdpi_ip_config_list) / sizeof(sdpi_ip_config_t);

	for(i = 0; i < list_size; ++i)
	{
		addr = sdpi_ip_to_number(sdpi_ip_config_list[i].ip_addr);

		bits = sdpi_ip_config_list[i].mask_bits;

		ip_mask = sdpi_num_patricia_prefix_new(addr, bits);

		node = sdpi_num_patricia_node_insert(root, ip_mask);

		node->private_data = (void *)&(sdpi_ip_config_list[i].result);
	}

	return root;
}

int
sdpi_ip_inspect_context_init(
	sdpi_inspect_context_t *context
	)
{
	sdpi_ip_struct_t *sdpi_ip_struct = sdpi_init_ip_struct();
	
	if(sdpi_ip_struct != NULL)
	{
		context->inspect_struct = sdpi_ip_struct;
		return 0;
	}

	return -1;
}

void
sdpi_ip_inspect_context_free(
	sdpi_inspect_context_t *context
	)
{
	sdpi_ip_struct_t *sdpi_ip_struct = (sdpi_ip_struct_t *)context->inspect_struct;
	sdpi_num_patricia_free(sdpi_ip_struct);
}

void
sdpi_ip_inspector(
	sdpi_inspect_context_t *context,
	sdpi_flow_struct_t	*flow_struct,
	sdpi_inspect_result_t *result
	)
{
	sdpi_flow_base_t 			*flow_base = &flow_struct->flow_base;
	sdpi_num_patricia_tree_t 	*tree = (sdpi_num_patricia_tree_t *)context->inspect_struct;
	sdpi_num_patricia_node_t 	*node = NULL;
	sdpi_ip_mask_t 				lower_mask;
	sdpi_ip_mask_t 				upper_mask;

	if(flow_base->protocol != SDPI_TCPPROTO_VALUE &&
	   flow_base->protocol != SDPI_UDPPROTO_VALUE)
	{
		*result = sdpi_unknown_ip_result;
		return;
	}

	//先识别小ip，这个更可能为公网IP
	lower_mask.ip_addr = flow_base->lower_addr;
	lower_mask.bits = 32;

	if((node = sdpi_num_patricia_node_lookup_ex(tree, &lower_mask)))
	{
		*result = *(sdpi_inspect_result_t *)node->private_data;
		printf("IP inspected: %s\n", result->description);
		return;
	}

	upper_mask.ip_addr = flow_base->upper_addr;
	upper_mask.bits = 32;

	if((node = sdpi_num_patricia_node_lookup_ex(tree, &upper_mask)))
	{
		*result = *(sdpi_inspect_result_t *)node->private_data;
		printf("IP inspected: %s\n", result->description);
		return;
	}

	*result = sdpi_unknown_ip_result;
}