#ifndef _SDPI_NUM_PATRICIA_H
#define _SDPI_NUM_PATRICIA_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SDPI_IP_MAX_SIZE	20

#define SDPI_BIT_VALUE(addr, bits) ((addr) & ((0xffffffff >> (32 - bits)) << (32 - bits)))

#define SDPI_BIT_SET(addr, bits) ((addr) & (0x80000000 >> bits))

#define SDPI_MIN_VALUE(x, y) (((x) <= (y)) ? (x) : (y)) 

typedef struct _sdpi_ip_mask
{
	int 			ip_addr;
	unsigned int 	bits;
}sdpi_ip_mask_t;

typedef struct _sdpi_num_patricia_node
{
	struct _sdpi_num_patricia_node 	*parent;
	struct _sdpi_num_patricia_node 	*left;
	struct _sdpi_num_patricia_node 	*right;
	sdpi_ip_mask_t 					*prefix;
	unsigned int 			 		 bits;
	void  							*private_data;
}sdpi_num_patricia_node_t;

typedef struct _sdpi_num_patricia_tree
{
	unsigned int nodes;
	sdpi_num_patricia_node_t *root;
}sdpi_num_patricia_tree_t;

typedef sdpi_num_patricia_tree_t sdpi_ip_struct_t;

sdpi_num_patricia_tree_t *
sdpi_num_patricia_init();

void 
sdpi_num_patricia_free_nodes(
	sdpi_num_patricia_tree_t *tree,
	sdpi_num_patricia_node_t *node
	);

void
sdpi_num_patricia_free(
	sdpi_num_patricia_tree_t *tree
	);

sdpi_ip_mask_t *
sdpi_num_patricia_prefix_new(
	int ip_addr,
	unsigned int bits
	);

void
sdpi_num_patricia_prefix_delete(
	sdpi_ip_mask_t *prefix
	);

sdpi_num_patricia_node_t *
sdpi_num_patricia_node_new(
	sdpi_ip_mask_t *init_prefix
	);

void
sdpi_num_patricia_node_delete(
	sdpi_num_patricia_node_t *node
	);

//locate_addr: 与目标地址最接近的有效地址
//locate_bits: 与目标bits
sdpi_num_patricia_node_t *
sdpi_num_patricia_node_locate(
	sdpi_num_patricia_tree_t *tree,
	sdpi_ip_mask_t *lookup_prefix,
	unsigned int *locate_addr,
	unsigned int *locate_bits
	);

sdpi_num_patricia_node_t *
sdpi_num_patricia_node_lookup(
	sdpi_num_patricia_tree_t *tree,
	sdpi_ip_mask_t *lookup_prefix
	);

sdpi_num_patricia_node_t *
sdpi_num_patricia_node_lookup_ex(
	sdpi_num_patricia_tree_t *tree,
	sdpi_ip_mask_t *lookup_prefix
	);

sdpi_num_patricia_node_t *
sdpi_num_patricia_node_insert(
	sdpi_num_patricia_tree_t *tree,
	sdpi_ip_mask_t *insert_prefix
	);

void 
sdpi_num_patricia_traverse(
	sdpi_num_patricia_node_t *root
	);

void
sdpi_num_patricia_node_remove(
	sdpi_num_patricia_tree_t *tree,
	sdpi_num_patricia_node_t *node
	);

int 
sdpi_ip_to_number(
	char *str_ip_addr
	);

char *
sdpi_number_to_ip(
	int num_ip_addr,
	char str_ip_addr[],
	unsigned int size
	);

#endif
