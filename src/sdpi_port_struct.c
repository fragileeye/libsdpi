#include "sdpi_port_struct.h"

sdpi_avl_tree_t *sdpi_avl_tree_init()
{
	sdpi_avl_tree_t *tree = malloc(sizeof(sdpi_avl_tree_t));

	if(tree)
	{
		tree->root = NULL;
		tree->nodes = 0;
	}

	return tree;
}

void sdpi_avl_tree_free_nodes(
	sdpi_avl_node_t *root
	)
{
	if(root)
	{
		if(root->left)
		{
			sdpi_avl_tree_free_nodes(root->left);
		}

		if(root->right)
		{
			sdpi_avl_tree_free_nodes(root->right);
		}

		free(root);
	}
}

void sdpi_avl_tree_free(
	sdpi_avl_tree_t *tree
	)
{
	if(tree)
	{
		sdpi_avl_tree_free_nodes(tree->root);
		free(tree);
	}
}

sdpi_avl_node_t *sdpi_avl_node_new(
	unsigned short port 
	)
{
	sdpi_avl_node_t *node = malloc(sizeof(sdpi_avl_node_t));

	if(node)
	{
		node->port = port;
		node->height = 1; //本层身高为1
		node->left = node->right = node->parent = NULL;
	}

	return node;
}

void sdpi_avl_node_delete(
	sdpi_avl_node_t *node
	)
{
	free(node);
}

sdpi_avl_node_t *
sdpi_avl_tree_lookup(
	sdpi_avl_tree_t *tree,
	unsigned short port
	)
{
	sdpi_avl_node_t *traverse_node = tree->root;

	while(traverse_node)
	{
		if(traverse_node->port == port)
		{
			break;
		}
		else if(traverse_node->port < port)
		{
			traverse_node = traverse_node->right;
		}
		else
		{
			traverse_node = traverse_node->left;
		}
	}

	return traverse_node;
}

void sdpi_avl_tree_rotate_left(
	sdpi_avl_tree_t *tree,
	sdpi_avl_node_t *node
	)
{
	sdpi_avl_node_t *node_parent = node->parent;
	sdpi_avl_node_t *node_right = node->right;

	//右子树的左支为node节点的右支
	if(node_right->left)
	{
		node_right->left->parent = node;
	}

	node->right = node_right->left;

	//右子树为node的新父亲
	node->parent = node_right;
	node_right->left = node;

	node_right->parent = node_parent;
	//说明当前节点为根节点，需要调整，这里是个坑，一定要注意
	//因为若根节点旋转，则转跑求了，需要将根节点重新指向平衡节点。
	if(!node_parent)
	{
		tree->root = node_right;
	}
	else if(node == node_parent->left)
	{
		node_parent->left = node_right;
	}
	else
	{
		node_parent->right = node_right;
	}
}

void sdpi_avl_tree_rotate_right(
	sdpi_avl_tree_t *tree,
	sdpi_avl_node_t *node
	)
{
	sdpi_avl_node_t *node_parent = node->parent;
	sdpi_avl_node_t *node_left = node->left;

	//step1:将node左支的右孩子给node的左孩子
	if(node_left->right)
	{
		node_left->right->parent = node;
	}
	node->left = node_left->right;

	//step2: 左支为新父亲
	node->parent = node_left;
	node_left->right = node;

	//step3: 将新的父亲的父节点指向原父亲节点
	node_left->parent = node_parent;
	if(!node_parent)
	{
		tree->root = node_left;
	}
	else if(node == node_parent->left)
	{
		node_parent->left = node_left;
	}
	else
	{
		node_parent->right = node_left;
	}
}

void sdpi_avl_tree_adjust_height(
	sdpi_avl_tree_t *tree,
	sdpi_avl_node_t *node
	)
{
	sdpi_avl_node_t *node_parent = NULL;

	while(node && node->parent)
	{
		node_parent = node->parent;
		node_parent->height = node->height + 1;

		node = node_parent;
		node_parent = node->parent;
	}
}

sdpi_avl_node_t * 
sdpi_avl_tree_insert(
	sdpi_avl_tree_t *tree,
	unsigned short port
	)
{
	sdpi_avl_node_t *insert_node = NULL;
	sdpi_avl_node_t *traverse_node = tree->root;
	sdpi_avl_node_t *previous_node = NULL;

	//指针初始化为NULL
	insert_node = sdpi_avl_node_new(port);

	if(!insert_node) return NULL;

	while(traverse_node)
	{
		previous_node = traverse_node;

		if(traverse_node->port > port)
		{
			traverse_node = traverse_node->left;
		}
		else
		{
			traverse_node = traverse_node->right;
		}
	}

	insert_node->parent = previous_node;

	if(!previous_node)
	{
		tree->root = insert_node;
		return insert_node;
	}

	if(previous_node->port < port)
	{
		previous_node->right = insert_node;
	}
	else
	{
		previous_node->left = insert_node;
	}
	
	tree->nodes++;
	//sdpi_avl_tree_adjust_height(tree, insert_node);
	sdpi_avl_tree_update(tree, insert_node);

	return insert_node;
}

int sdpi_avl_tree_calc_balance(
	sdpi_avl_node_t *node
	)
{
	int balance = 0;

	if(!node->left && !node->right)
	{
		return balance;
	}

	if(node->left && node->right)
	{
		balance = node->left->height - node->right->height;
	}
	else if(node->left)
	{
		balance = node->left->height;
	}
	else
	{
		balance = -(int)node->right->height;
	}

	return balance;
}

//更新插入后的树结构，进行平衡调整，并且修改树高度。
void sdpi_avl_tree_update(
	sdpi_avl_tree_t *tree,
	sdpi_avl_node_t *node
	)
{
	sdpi_avl_node_t *node_parent = node->parent;
	sdpi_avl_node_t *node_son = NULL;
	int 			 balance = 0;

	//直接从node_parent开始判断，因为插入node后，node不可能不平衡，否则之前就已经不平衡了
	while(node_parent)
	{
		//插入后更新父节点的高度
		if(node_parent->height < node->height + 1)
		{
			node_parent->height = node->height + 1;
		}

		//node节点肯定是平衡的，如果插入节点node后node不平衡，则说明插入前也一定存在不平衡
		balance = sdpi_avl_tree_calc_balance(node);
		node_son = balance == 1 ? node->left : node->right;

		balance = sdpi_avl_tree_calc_balance(node_parent);

		if(balance > 1)
		{
			if(node_son == node->left)
			{
				//LL情况，此时以node_parent节点右旋即可
				sdpi_avl_tree_rotate_right(tree, node_parent);

				//node_parent右旋后与node_grandson具有同样的高度
				node_parent->height = node_son->height;
			}
			else
			{
				//LR情况，node节点左旋(变成LL情况)，node_parent节点右旋
				sdpi_avl_tree_rotate_left(tree, node);
				sdpi_avl_tree_rotate_right(tree, node_parent);

				node_parent->height = node->height = node_son->height;
				node_son->height += 1;
			}
			break;
		}
		else if(balance < -1)
		{
			if(node_son == node->right)
			{
				//RR情况
				sdpi_avl_tree_rotate_left(tree, node_parent);
				node_parent->height = node_son->height;
			}
			else
			{
				//RL情况
				sdpi_avl_tree_rotate_right(tree, node);
				sdpi_avl_tree_rotate_left(tree, node_parent);

				node_parent->height = node->height = node_son->height;
				node_son->height += 1;
			}
			break;
		}

		node_son = node;
		node = node_parent;
		node_parent = node->parent;
	}
}

//中序遍历
void sdpi_avl_tree_traverse(
	sdpi_avl_node_t *root
	)
{
	if(!root) return;

	if(root->left) sdpi_avl_tree_traverse(root->left);
		
	printf("port: %d, height: %d\n", root->port, root->height);

	if(root->right) sdpi_avl_tree_traverse(root->right);
}
