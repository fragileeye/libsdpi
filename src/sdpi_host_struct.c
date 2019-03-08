#include "sdpi_host_struct.h"

sdpi_str_patricia_tree_t *
sdpi_str_patricia_init()
{
	sdpi_str_patricia_tree_t *tree = \
		malloc(sizeof(sdpi_str_patricia_tree_t));

	if(tree)
	{
		tree->root = NULL;
		tree->nodes = 0;
	}

	return tree;
}

void sdpi_str_patricia_free(
	sdpi_str_patricia_tree_t *tree
	)
{
	sdpi_str_patricia_node_delete(tree->root, 1);
	free(tree);
}

sdpi_str_patricia_node_t *
sdpi_str_patricia_node_new(
	char *name,
	unsigned int length
	)
{
	sdpi_str_patricia_node_t 	*node = NULL;
	sdpi_host_name_t 		 	*prefix = NULL;
	unsigned int 			  	 size = 0;

	node = calloc(sizeof(sdpi_str_patricia_node_t), 1);

	//新建节点初始化prefix
	if(name && length > 0)
	{
		size = (sizeof(sdpi_host_name_t) + length + 7) & ~7;
		prefix = calloc(size, 1);

		prefix->length = length;
		snprintf(prefix->name, length + 1, "%s", name);
		node->prefix = prefix;
	}

	return node;
}

void sdpi_str_patricia_node_delete(
	sdpi_str_patricia_node_t *root,
	int is_recurse
	)
{
	int i;

	if(root && is_recurse)
	{
		for(i = 0; i < SDPI_MAX_TABLE_SIZE; ++i)
		{
			if(root->table[i])
			{
				sdpi_str_patricia_node_delete(root->table[i], 1);
			}
		}
	}

	if(root->prefix)
	{
		free(root->prefix);
		root->prefix = NULL;
	}

	free(root);
}

sdpi_str_patricia_node_t *
sdpi_str_patricia_node_insert(
	sdpi_str_patricia_tree_t *tree,
	char *host_name
	)
{
	unsigned int 				i, k;
	unsigned int 				name_length = 0;
	unsigned int 				min_length = 0;
	unsigned int 				curr_index = 0;
	unsigned int 				diff_index = 0;
	char 						*curr_name = NULL;
	sdpi_str_patricia_node_t 	*node = NULL;
	sdpi_str_patricia_node_t 	*parent_node = NULL;
	sdpi_str_patricia_node_t 	*child_node = NULL;
	sdpi_str_patricia_node_t 	*new_node = NULL;
	sdpi_str_patricia_node_t 	*glue_node = NULL;

	//根节点为空，生成一个空的node
	if(!tree->root)
	{
		tree->root = sdpi_str_patricia_node_new(NULL, 0);
	}

	name_length = strlen(host_name);

	for(i = 0, node = tree->root; i < name_length; i = curr_index)
	{
		k = sdpi_char_index(host_name[i]);

		if(!(child_node = node->table[k]))
		{
			break;
		}

		parent_node = node;
		node = child_node;
		curr_name = node->prefix->name;
		curr_index = node->prefix->length;
	}

	//找字符开始不同的位置
	min_length = curr_index < name_length ? curr_index : name_length;

	for(i = 0; i < min_length && curr_name[i] == host_name[i]; ++i);

	diff_index = i;

	//找到已有节点，不需插入
	if(diff_index == name_length && diff_index == curr_index)
	{
		node->is_final = 1;
		return node;
	}

	//建立新节点
	new_node = sdpi_str_patricia_node_new(host_name, name_length);

	if(diff_index == curr_index)
	{
		k = sdpi_char_index(host_name[diff_index]);
		node->table[k] = new_node;
	}
	else if(diff_index == name_length)
	{
		k = sdpi_char_index(curr_name[diff_index]);
		new_node->table[k] = node;

		for(i = 0; parent_node->table[i] != node; ++i);
		parent_node->table[i] = new_node;
	}
	else
	{
		glue_node = sdpi_str_patricia_node_new(host_name, diff_index);

		k = sdpi_char_index(host_name[diff_index]);
		glue_node->table[k] = new_node;

		k = sdpi_char_index(curr_name[diff_index]);
		glue_node->table[k] = node;

		for(i = 0; parent_node->table[i] != node; i++);
		parent_node->table[i] = glue_node;
	}

	tree->nodes += (glue_node ? 2 : 1);
	new_node->is_final = 1;
	return new_node;
}

sdpi_str_patricia_node_t *
sdpi_str_patricia_node_lookup(
	sdpi_str_patricia_tree_t *tree,
	char *host
	)
{
	unsigned int 				i, k;
	unsigned int 				length = strlen(host);
	unsigned int 				curr_index = 0;
	char 					*curr_name = NULL;
	sdpi_str_patricia_node_t	*node = tree->root;
	sdpi_str_patricia_node_t	*sub_node = NULL;

	for(i = 0; i < length; i = curr_index)
	{
		k = sdpi_char_index(host[i]);

		if(!(sub_node = node->table[k]))
		{
			break;
		}

		node = sub_node;
		curr_index = node->prefix->length;
		curr_name = node->prefix->name;
	}

	if(i == length && !strcmp(curr_name, host))
	{
		return node;
	}

	return NULL;
}

void sdpi_str_patricia_traverse(
	sdpi_str_patricia_node_t *root
	)
{
	unsigned int i;

	if(root)
	{
		if(root->is_final)
		{
			fprintf(stdout, "host: %s, length: %d\n", root->prefix->name, root->prefix->length);
		}

		for(i = 0; i < SDPI_MAX_TABLE_SIZE; ++i)
		{
			if(root->table[i])
			{
				sdpi_str_patricia_traverse(root->table[i]);
			}
		}
	}
}

