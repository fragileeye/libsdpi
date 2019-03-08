#ifndef _SDPI_PORT_SEARCHER_H
#define _SDPI_PORT_SEARCHER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct _sdpi_avl_node
{
	struct _sdpi_avl_node 	*left;
	struct _sdpi_avl_node 	*right;
	struct _sdpi_avl_node 	*parent;
	unsigned short 		   	 port;
	unsigned int 		  	 height;
	void 					*private_data;
}sdpi_avl_node_t;

typedef struct _sdpi_avl_tree
{
	sdpi_avl_node_t	*root;
	unsigned int 	 nodes;
}sdpi_avl_tree_t;

typedef struct _sdpi_port_struct
{
	sdpi_avl_tree_t	*sdpi_tport_struct;
	sdpi_avl_tree_t *sdpi_uport_struct;
}sdpi_port_struct_t;


sdpi_avl_tree_t *
sdpi_avl_tree_init(
	);

void 
sdpi_avl_tree_free_nodes(
	sdpi_avl_node_t *root
	);

void 
sdpi_avl_tree_free(
	sdpi_avl_tree_t *tree
	);

sdpi_avl_node_t *
sdpi_avl_node_new(
	unsigned short port 
	);

void 
sdpi_avl_node_delete(
	sdpi_avl_node_t *node
	);

sdpi_avl_node_t *
sdpi_avl_tree_lookup(
	sdpi_avl_tree_t *tree,
	unsigned short port
	);

void 
sdpi_avl_tree_rotate_left(
	sdpi_avl_tree_t *tree,
	sdpi_avl_node_t *node
	);

void 
sdpi_avl_tree_rotate_right(
	sdpi_avl_tree_t *tree,
	sdpi_avl_node_t *node
	);

void 
sdpi_avl_tree_adjust_height(
	sdpi_avl_tree_t *tree,
	sdpi_avl_node_t *node
	);

sdpi_avl_node_t * 
sdpi_avl_tree_insert(
	sdpi_avl_tree_t *tree,
	unsigned short port
	);

void 
sdpi_avl_tree_update(
	sdpi_avl_tree_t *tree,
	sdpi_avl_node_t *node
	);

int 
sdpi_avl_tree_calc_balance(
	sdpi_avl_node_t *node
	);

void 
sdpi_avl_tree_traverse(
	sdpi_avl_node_t *root
	);



#endif