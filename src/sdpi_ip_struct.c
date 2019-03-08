#include "sdpi_ip_struct.h"

#pragma warning(disable:4996)

sdpi_num_patricia_tree_t *
sdpi_num_patricia_init()
{
	sdpi_num_patricia_tree_t * tmp_tree = malloc(sizeof(sdpi_num_patricia_tree_t));

	if(tmp_tree)
	{
		tmp_tree->nodes = 0;
		tmp_tree->root = NULL;
	}

	return tmp_tree;
}

void sdpi_num_patricia_free_nodes(
	sdpi_num_patricia_tree_t *tree,
	sdpi_num_patricia_node_t *node
	)
{
	if(!node) return;

	if(node->left)
	{
		sdpi_num_patricia_free_nodes(tree, node->left);
	}
	
	if(node->right)
	{
		sdpi_num_patricia_free_nodes(tree, node->right);
	}

	sdpi_num_patricia_node_delete(node);
}

void 
sdpi_num_patricia_free(
	sdpi_num_patricia_tree_t *tree
	)
{
	sdpi_num_patricia_free_nodes(tree, tree->root); 
	free(tree);
}

sdpi_ip_mask_t *
sdpi_num_patricia_prefix_new(
	int ip_addr,
	unsigned int bits
	)
{
	sdpi_ip_mask_t *tmp_prefix = malloc(sizeof(sdpi_ip_mask_t));

	if(tmp_prefix)
	{
		tmp_prefix->ip_addr = ip_addr;
		tmp_prefix->bits = bits;
	}

	return tmp_prefix;
}

void 
sdpi_num_patricia_prefix_delete(
	sdpi_ip_mask_t *prefix
	)
{
	free(prefix);
}

sdpi_num_patricia_node_t *
sdpi_num_patricia_node_new(
	sdpi_ip_mask_t *init_prefix
	)
{
	sdpi_num_patricia_node_t *tmp_node = malloc(
		sizeof(sdpi_num_patricia_node_t));

	if(tmp_node)
	{
		tmp_node->bits = !init_prefix ? 0 : init_prefix->bits;
		tmp_node->prefix = init_prefix;
		tmp_node->parent = tmp_node->left = tmp_node->right = NULL;
	}

	return tmp_node;
}

void
sdpi_num_patricia_node_delete(
	sdpi_num_patricia_node_t *node
	)
{
	if(node && node->prefix)
	{
		sdpi_num_patricia_prefix_delete(node->prefix);
	}

	free(node);
}

//定位与target有公共前缀的节点
sdpi_num_patricia_node_t *
sdpi_num_patricia_node_locate(
	sdpi_num_patricia_tree_t *tree,
	sdpi_ip_mask_t *lookup_prefix,
	unsigned int *locate_addr,
	unsigned int *locate_bits
	)
{
	sdpi_num_patricia_node_t 	*traverse_node = NULL;
	sdpi_num_patricia_node_t 	*previous_node = NULL;
	unsigned int 				lookup_addr = 0;
	unsigned int 			 	lookup_bits = 0;
	unsigned int				index_addr = 0;
	unsigned int				index_bits = 0;
	unsigned int				different_bit = 0;
	unsigned int 				tmp_value = 0;

	if(!tree || !tree->nodes)
	{
		return NULL;
	}

	lookup_bits = lookup_prefix->bits;
	lookup_addr = lookup_prefix->ip_addr;

	//找到target节点的可能子节点，因为只有target子节点（或本身）才与target有公共前缀
	//值得注意的是，需要跳过prefix = NULL的情况，因为prefix=NULL的节点为glue节点。
	traverse_node = tree->root;

	while(traverse_node->bits < lookup_bits || !traverse_node->prefix)
	{
		if(SDPI_BIT_SET(lookup_addr, traverse_node->bits))
		{
			if(traverse_node->right)
			{
				traverse_node = traverse_node->right;
			}
			else
			{
				break;
			}
		}
		else
		{
			if(traverse_node->left)
			{
				traverse_node = traverse_node->left;
			}
			else
			{
				break;
			}
		}
	}

	index_bits = traverse_node->prefix->bits;
	index_addr = traverse_node->prefix->ip_addr;
	tmp_value = ~(index_addr ^ lookup_addr);

	//找公共前缀的位置
	while(tmp_value & 0x80000000)
	{
		different_bit++;
		tmp_value <<= 1;
	}

	different_bit = SDPI_MIN_VALUE(different_bit, SDPI_MIN_VALUE(index_bits, lookup_bits));

	//找具有公共前缀的节点，从traverse节点回溯
	previous_node = traverse_node->parent;

	while(previous_node && previous_node->bits >= different_bit)
	{
		traverse_node = previous_node;
		previous_node = previous_node->parent;
	}

	*locate_addr = index_addr;
	*locate_bits = different_bit;

	return traverse_node;
}

//精确查找
sdpi_num_patricia_node_t *
sdpi_num_patricia_node_lookup(
	sdpi_num_patricia_tree_t *tree,
	sdpi_ip_mask_t *lookup_prefix
	)
{
	sdpi_num_patricia_node_t	*find_node = NULL;
	sdpi_num_patricia_node_t	*traverse_node = tree->root;
	sdpi_ip_mask_t				*prefix = NULL;

	while(traverse_node->bits < lookup_prefix->bits || !traverse_node->prefix)
	{
		if(SDPI_BIT_SET(lookup_prefix->ip_addr, traverse_node->bits))
		{
			if(traverse_node->right)
			{
				traverse_node = traverse_node->right;
			}
			else
			{
				break;
			}
		}
		else
		{
			if(traverse_node->left)
			{
				traverse_node = traverse_node->left;
			}
			else
			{
				break;
			}
		}
	}

	prefix = traverse_node->prefix;

	//prefix不可能为空，因为只有glue_node节点的prefix为NULL，而退出上面循环的节点肯定不为glue_node。
	if(prefix->bits == lookup_prefix->bits && prefix->ip_addr == lookup_prefix->ip_addr)
	{
		find_node = traverse_node;
	}

	return find_node;
}

//模糊查找:针对同一子网的情况
sdpi_num_patricia_node_t *
sdpi_num_patricia_node_lookup_ex(
	sdpi_num_patricia_tree_t *tree,
	sdpi_ip_mask_t *lookup_prefix
	)
{
	sdpi_num_patricia_node_t	*traverse_node = tree->root;
	sdpi_num_patricia_node_t	*find_node = NULL;
	sdpi_ip_mask_t 				*index_prefix = NULL;
	unsigned int				 target_subnet = 0;
	unsigned int				 locate_subnet = 0;

	while(traverse_node->bits < lookup_prefix->bits || !traverse_node->prefix)
	{
		if(SDPI_BIT_SET(lookup_prefix->ip_addr, traverse_node->bits))
		{
			if(traverse_node->right)
			{
				traverse_node = traverse_node->right;
			}
			else
			{
				break;
			}
		}
		else
		{
			if(traverse_node->left)
			{
				traverse_node = traverse_node->left;
			}
			else
			{
				break;
			}
		}
	}

	index_prefix = traverse_node->prefix;

	//prefix不可能为空，因为只有glue_node节点的prefix为NULL，而退出上面循环的节点肯定不为glue_node。
	if(index_prefix->bits <= lookup_prefix->bits)
	{
		target_subnet = SDPI_BIT_VALUE(lookup_prefix->ip_addr, index_prefix->bits);
		locate_subnet = SDPI_BIT_VALUE(index_prefix->ip_addr, index_prefix->bits);

		find_node = locate_subnet == target_subnet ? traverse_node : NULL;
	}

	return find_node;
}

sdpi_num_patricia_node_t *
sdpi_num_patricia_node_insert(
	sdpi_num_patricia_tree_t *tree,
	sdpi_ip_mask_t *insert_prefix
	)
{
	sdpi_num_patricia_node_t 	*insert_node = NULL;
	sdpi_num_patricia_node_t 	*glue_node = NULL;
	sdpi_num_patricia_node_t 	*locate_node = NULL;
	unsigned int				 locate_addr = 0;
	unsigned int				 locate_bits = 0;
	unsigned int				 different_bit = 0;
	unsigned int 			 	 insert_addr = 0;
	unsigned int 				 insert_bits = 0;
	char						 alert_ip[SDPI_IP_MAX_SIZE];

	//非法参数
	if(!tree || !insert_prefix)
	{
		return NULL;
	}
	//先创建一个待插入节点
	if(!(insert_node = sdpi_num_patricia_node_new(insert_prefix)))
	{
		return NULL;
	}

	//空树就直接赋值头结点
	if(tree->nodes == 0)
	{
		tree->root = insert_node;
		tree->nodes += 1;
		return insert_node;
	}

	//定位不到与prefix有公共前缀的节点，则说明树是空树，这是不可能的，空树的情况前面已经处理了。
	if(!(locate_node = sdpi_num_patricia_node_locate(
			tree, insert_prefix, &locate_addr, &different_bit))) 
	{
		fprintf(stderr, "sdpi_num_patricia_node_locate failed!");
		return NULL;
	}

	locate_bits	= locate_node->bits;
	insert_bits	= insert_prefix->bits;
	insert_addr	= insert_prefix->ip_addr;

	//找到节点了，先删除创建的节点，然后返回找到的节点，不用插入了。
	if(different_bit == insert_bits && different_bit == locate_bits)
	{
		if(!locate_node->prefix)
		{
			locate_node->prefix = sdpi_num_patricia_prefix_new(insert_addr, insert_bits);
		}
		
		sdpi_number_to_ip(locate_addr, alert_ip, sizeof(alert_ip));
		fprintf(stdout, "no need to add: %s\n", alert_ip);

		sdpi_num_patricia_node_delete(insert_node);
		return locate_node;
	}

	//说明target为located节点的父节点
	if(different_bit == insert_bits)
	{
		if(SDPI_BIT_SET(locate_addr, different_bit))
		{
			insert_node->right = locate_node;
		}
		else
		{
			insert_node->left = locate_node;
		}

		insert_node->parent = locate_node->parent;

		//注意考虑located_node为根节点的情况，此时根节点的父节点为NULL
		if(!locate_node->parent)
		{
			tree->root = insert_node;
		}
		else if(locate_node->parent->right == locate_node)
		{
			locate_node->parent->right = insert_node;
		}
		else
		{
			locate_node->parent->left = insert_node;
		}

		locate_node->parent = insert_node;

		tree->nodes += 1;
		return insert_node;
	}

	if(different_bit == locate_bits)
	{
		if(SDPI_BIT_SET(insert_addr, different_bit))
		{
			locate_node->right = insert_node;
		}
		else
		{
			locate_node->left = insert_node;
		}

		insert_node->parent = locate_node;

		tree->nodes += 1;
		return insert_node;
	}

	//否则创建glue节点为target和located节点的公共父节点
	//fprintf(stdout, "%s: %s\n", __FUNCTION__, "glue node is inserting!");

	if(!(glue_node = sdpi_num_patricia_node_new(NULL)))
	{
		sdpi_num_patricia_node_delete(insert_node);
		return NULL;
	}

	glue_node->bits = different_bit;
	
	if(SDPI_BIT_SET(insert_addr, different_bit))
	{
		glue_node->right = insert_node;
		glue_node->left = locate_node;
	}
	else
	{
		glue_node->right = locate_node;
		glue_node->left = insert_node;
	}

	glue_node->parent = locate_node->parent;
	
	if(!locate_node->parent)
	{
		tree->root = glue_node;
	}
	else if(locate_node->parent->right == locate_node)
	{
		locate_node->parent->right = glue_node;
	}
	else
	{
		locate_node->parent->left = glue_node;
	}

	insert_node->parent = glue_node;
	locate_node->parent = glue_node;

	tree->nodes += 2;
	return insert_node;
}

void sdpi_num_patricia_traverse(
	sdpi_num_patricia_node_t *root
	)
{
	char			ip_addr[SDPI_IP_MAX_SIZE];
	sdpi_ip_mask_t	*prefix = NULL;

	if(!root) return;
	
	if(root->prefix)
	{
		prefix = root->prefix;
		sdpi_number_to_ip(prefix->ip_addr, ip_addr, sizeof(ip_addr));
		fprintf(stdout, "ip: %s, bits: %u\n", ip_addr, prefix->bits);
	}

	if(root->left)
	{
		sdpi_num_patricia_traverse(root->left);
	}

	if(root->right)
	{
		sdpi_num_patricia_traverse(root->right);
	}
}

void
sdpi_num_patricia_node_remove(
	sdpi_num_patricia_tree_t *tree,
	sdpi_num_patricia_node_t *node
	)
{
	sdpi_num_patricia_node_t *tmp_parent = NULL;
	sdpi_num_patricia_node_t *tmp_child = NULL;

	if(!tree || !tree->nodes || !node)
	{
		return;
	}

	if(node->left && node->right)
	{
		//将其变成glue节点
		if(node->prefix)
		{
			sdpi_num_patricia_prefix_delete(node->prefix);
			node->prefix = NULL;
		}

		return; //本身即为glue节点，直接返回
	}

	tmp_parent = node->parent;

	//叶子节点，值得注意的考虑当前节点的父节点是否为glue节点，如果
	//其父节点为glue节点，在删除当前节点的时候，还要删除其父节点。
	if(!node->left && !node->right)
	{
		if(!tmp_parent)
		{
			sdpi_num_patricia_node_delete(node);
			tree->root = NULL; tree->nodes = 0;
			return;
		}

		//其父节点不为glue节点
		if(tmp_parent->prefix)
		{
			if(node == tmp_parent->right)
			{
				tmp_parent->right = NULL;
			}
			else
			{
				tmp_parent->left = NULL;
			}

			sdpi_num_patricia_node_delete(node);
			tree->nodes--;
			return;
		}
		else //其父节点为glue节点的情况
		{
			tmp_child = (node == tmp_parent->right) ? \
				tmp_parent->right : tmp_parent->left;

			if(!tmp_parent->parent)
			{
				sdpi_num_patricia_node_delete(node);
				sdpi_num_patricia_node_delete(tmp_parent);
				tree->root = tmp_child; tree->nodes -= 2;
				return;
			}

			if(tmp_parent->parent->right == tmp_parent)
			{
				tmp_parent->parent->right = tmp_child;
			}
			else
			{
				tmp_parent->parent->left = tmp_child;
			}

			tmp_child->parent = tmp_parent->parent;
			sdpi_num_patricia_node_delete(node);
			sdpi_num_patricia_node_delete(tmp_parent);
			tree->nodes -= 2;
			return;
		}
	}

	//此时prefix也一定不为空，单支节点不可能为glue节点！
	tmp_child = node->left ? node->left : node->right;

	tmp_child->parent = tmp_parent;

	if(!tmp_parent)
	{
		sdpi_num_patricia_node_delete(node);
		tree->root = tmp_child; tree->nodes--;
		return;
	}

	if(node == tmp_parent->left)
	{
		tmp_parent->left = tmp_child;
	}
	else
	{
		tmp_parent->right = tmp_child;
	}

	sdpi_num_patricia_node_delete(node);
	tree->nodes--;
	return;
}

int sdpi_ip_to_number(
	char * str_ip_addr
	)
{
	int u1, u2, u3, u4;

	if(4 == sscanf(str_ip_addr, "%d.%d.%d.%d", &u1, &u2, &u3, &u4))
	{
		return (u1 << 24) | (u2 << 16) | (u3 << 8) | u4;
	}

	return 0;
}

char *sdpi_number_to_ip(
	int num_ip_addr,
	char str_ip_addr[],
	unsigned int size
	)
{
	if(str_ip_addr)
	{
		snprintf(str_ip_addr, size, "%u.%u.%u.%u", 
			(num_ip_addr >> 24) & 0xff, (num_ip_addr >> 16) & 0xff,
			(num_ip_addr >> 8) & 0xff, num_ip_addr & 0xff 
			);	
	}

	return str_ip_addr;
}
