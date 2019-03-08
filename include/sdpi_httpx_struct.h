#ifndef _SDPI_HTTPX_INSPECTOR_H
#define _SDPI_HTTPX_INSPECTOR_H
#include <stdio.h>
#include <stddef.h>
#include "sdpi_flow_struct.h"

#define SDPI_MAX_HTTP_METHOD_LENGTH		16 
#define SDPI_MAX_HOST_NAME_LENGTH 		64
#define SDPI_SSL_HANDSHAKE_TYPE			22
#define SDPI_SSL_CLIENTHELLO_TYPE		1
#define SDPI_SSL_SERVERNAME_TYPE		0

#define sdpi_is_http_port(port)  ((port) == 80 || ((port) == 8080) || ((port) == 8000))
#define sdpi_is_https_port(port) ((port) == 443)

#pragma pack(push, 1)
typedef struct _sdpi_ssl_struct
{
	unsigned char	content_type;
	unsigned short 	tls_version;
	unsigned short 	length;
	unsigned char	body[1];
}sdpi_ssl_struct_t;

typedef struct _sdpi_ssl_client_hello
{
	unsigned char	handshake_type;
	unsigned char	length[3];
	unsigned short 	version;
	unsigned char	random[32];
}sdpi_ssl_client_hello_t;

typedef struct _sdpi_ssl_session_id
{
	unsigned char	session_id_length;
	unsigned char	session_id[1];
}sdpi_ssl_session_id_t;

typedef struct _sdpi_ssl_cipher_suites
{
	unsigned short	cipher_suites_length;
	unsigned short	cipher_suites[1];
}sdpi_ssl_cipher_suites_t;

//always no method
typedef struct _sdpi_ssl_compression_method
{
	unsigned char	method_length;
	unsigned char	methods[1];
}sdpi_ssl_compression_method_t;

typedef struct _sdpi_ssl_first_extension
{
	unsigned short	extension_total_length;
	unsigned short	extension_type;
	unsigned short	length;
	unsigned char 	extension_body[1];
}sdpi_ssl_first_extension_t;

typedef struct _sdpi_ssl_server_name
{
	unsigned short	total_length;
	unsigned char 	name_type;
	unsigned short	name_length;
	unsigned char 	server_name[1];
}sdpi_ssl_server_name_t;
#pragma pack(pop)

int sdpi_inspect_hostname_in_http(
	sdpi_flow_struct_t *flow_struct,
	char host_name[],
	unsigned int size
	);

int sdpi_inspect_hostname_in_https(
	sdpi_flow_struct_t *flow_struct,
	char host_name[],
	unsigned int size
	);

int sdpi_inspect_hostname(
	sdpi_flow_struct_t *flow_struct,
	char host_name[],
	unsigned int size
	);

#endif 
