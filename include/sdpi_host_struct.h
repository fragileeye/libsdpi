#ifndef _SDPI_HOST_SEARCHER_H
#define _SDPI_HOST_SEARCHER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SDPI_MAX_TABLE_SIZE 		40
#define SDPI_CHAR_VALID_SET 		"0123456789abcdefghijklmnopqrstuvwxyz-."
#define SDPI_CHAR_HYPHEN_INDEX  	36 	// 连字符'-'的索引
#define SDPI_CHAR_DOT_INDEX 		37	// 点号'.'的索引	
#define SDPI_MAX_HOSTNAME_SIZE 		100

#define sdpi_char_index(ch) ( \
	(ch >= '0' && ch <= '9') ? (ch - '0') : \
	(ch >= 'a' && ch <= 'z') ? (ch - 'a' + 10) : \
	(ch == '-') ? SDPI_CHAR_HYPHEN_INDEX : \
	(ch == '.') ? SDPI_CHAR_DOT_INDEX : -1)
/*
 * host_name 
 * 1、一般为http://www.xxx，提取字段即为xxx, 代表程序中所指的host_name
 * 2、xxx显然包括域类别，com/cn等等
 * 3、host_name包含的合法字符为[0-9,a-z,-,.],因此最大有38个合法字符，并
 * 假设合法字符顺序也为0-9, a-z, -, .
 */
typedef struct _sdpi_host_name
{
	unsigned int 	length;
	char 			name[1];
}sdpi_host_name_t;

typedef struct _sdpi_str_patricia_node
{
	struct _sdpi_str_patricia_node 	*table[SDPI_MAX_TABLE_SIZE];
	sdpi_host_name_t 			   	*prefix;
	void 							*private_data;
	unsigned int 					 is_final;
}sdpi_str_patricia_node_t;

typedef struct _sdpi_str_patricia_tree
{
	sdpi_str_patricia_node_t *root;
	unsigned int 			  nodes;
}sdpi_str_patricia_tree_t;

typedef sdpi_str_patricia_tree_t sdpi_host_struct_t;

sdpi_str_patricia_tree_t *
sdpi_str_patricia_init();

void 
sdpi_str_patricia_free(
	sdpi_str_patricia_tree_t *tree
	);

sdpi_str_patricia_node_t *
sdpi_str_patricia_node_new(
	char *name,
	unsigned int length
	);

void 
sdpi_str_patricia_node_delete(
	sdpi_str_patricia_node_t *node,
	int is_recurse
	);

sdpi_str_patricia_node_t *
sdpi_str_patricia_node_insert(
	sdpi_str_patricia_tree_t *tree,
	char *host
	);

sdpi_str_patricia_node_t *
sdpi_str_patricia_node_lookup(
	sdpi_str_patricia_tree_t *tree,
	char *host
	);

void 
sdpi_str_patricia_traverse(
	sdpi_str_patricia_node_t *root
	);

#endif