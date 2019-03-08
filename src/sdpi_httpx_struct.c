#include "sdpi_httpx_struct.h"
#pragma warning(disable:4996)

static const char 
*sdpi_accept_http_methods[] = {
	"GET", "POST"
};

int sdpi_inspect_hostname_in_http(
	sdpi_flow_struct_t *flow_struct,
	char host_name[],
	unsigned int size
	)
{
	sdpi_flow_base_t 	*flow_base = &flow_struct->flow_base;
	char 		 		*lower_payload = flow_struct->lower_payload;
	char 				*upper_payload = flow_struct->upper_payload;
	char 				*payload = NULL;
	char 				method[SDPI_MAX_HTTP_METHOD_LENGTH] = { 0 };
	unsigned short 		accept_methods = 0;
	unsigned short 		i;

	if(size < SDPI_MAX_HOST_NAME_LENGTH) return -1;

	accept_methods = sizeof(sdpi_accept_http_methods) / sizeof(char *);

	if(!payload && sscanf(lower_payload, "%15[^ ]", method) == 1)
	{
		for(i = 0; i < accept_methods; ++i)
		{
			if(!strcmp(method, sdpi_accept_http_methods[i]))
			{
				payload = lower_payload;
				break;
			}
		}
	}

	if(!payload && sscanf(upper_payload, "%15[^ ]", method) == 1)
	{
		for(i = 0; i < accept_methods; ++i)
		{
			if(!strcmp(method, sdpi_accept_http_methods[i]))
			{
				payload = upper_payload;
				break;
			}
		}
	}

	if(payload && 1 == sscanf(payload, "%*s%*[^//]//%63[^/]", host_name)) 
	{
		return 0;
	}

	return -1;
}

static int sdpi_inspect_hostname_by_ssl(
	char host_name[],
	unsigned int name_size,
	char *payload,
	unsigned int payload_size
	)
{
	unsigned int 				legal_size = 0;
	unsigned char 				*ssl_handshake_type = NULL;
	sdpi_ssl_struct_t 			*ssl_struct = NULL;
	unsigned short				total_length = 0;
	unsigned short				session_id_length = 0;
	unsigned short				cipher_suites_length = 0;
	unsigned short				name_length = 0;
	sdpi_ssl_client_hello_t		*client_hello = NULL;
	sdpi_ssl_session_id_t		*session_id = NULL;
	sdpi_ssl_cipher_suites_t	*cipher_suite = NULL;
	sdpi_ssl_first_extension_t	*extension = NULL;
	sdpi_ssl_server_name_t		*name_struct = NULL;

	if(payload_size <= sizeof(sdpi_ssl_struct_t)) 
	{
		return -1;
	}

	ssl_struct = (sdpi_ssl_struct_t *)payload;

	if(ssl_struct->content_type != SDPI_SSL_HANDSHAKE_TYPE) 
	{
		return -1;
	}

	total_length = sdpi_short_n2l(ssl_struct->length);
	
	legal_size = total_length < SDPI_FLOW_PAYLOAD_SIZE ? 
		total_length : SDPI_FLOW_PAYLOAD_SIZE;

	if(payload_size < legal_size) 
	{
		return -1;
	}

	ssl_handshake_type = (unsigned char *)ssl_struct->body;

	if(*ssl_handshake_type != SDPI_SSL_CLIENTHELLO_TYPE) 
	{
		return -1;
	}

	client_hello = (sdpi_ssl_client_hello_t *)ssl_struct->body;

	session_id = (sdpi_ssl_session_id_t *)(
		(char *)client_hello + sizeof(sdpi_ssl_client_hello_t)
		);

	session_id_length = session_id->session_id_length;

	cipher_suite = (sdpi_ssl_cipher_suites_t *)(
		(char *)session_id + 
		offsetof(sdpi_ssl_session_id_t, session_id) + 
		session_id_length
		);

	cipher_suites_length = sdpi_short_n2l(cipher_suite->cipher_suites_length);

	extension = (sdpi_ssl_first_extension_t *)(
		(char *)cipher_suite +
		offsetof(sdpi_ssl_cipher_suites_t, cipher_suites) +
		cipher_suites_length +
		sizeof(sdpi_ssl_compression_method_t)
		);

	if(extension->extension_type != SDPI_SSL_SERVERNAME_TYPE) 
	{
		return -1;
	}

	name_struct = (sdpi_ssl_server_name_t *)extension->extension_body;

	name_length = sdpi_short_n2l(name_struct->name_length);

	if(name_size <= name_length) 
	{
		return -1;
	}

	snprintf(host_name, name_size, "%s", name_struct->server_name);
	return 0;
}

int sdpi_inspect_hostname_in_https(
	sdpi_flow_struct_t *flow_struct,
	char host_name[],
	unsigned int name_size
	)
{
	unsigned short	lower_size = flow_struct->lower_size;
	unsigned short	upper_size = flow_struct->upper_size;
	char 			*lower_payload = flow_struct->lower_payload;
	char 			*upper_payload = flow_struct->upper_payload;
	int				result = -1;

	result = sdpi_inspect_hostname_by_ssl(
		host_name, name_size, lower_payload, lower_size);

	if(result == 0) 
	{ 
		return result;
	}
	
	result = sdpi_inspect_hostname_by_ssl(
		host_name, name_size, upper_payload, upper_size);
		
	return result;
}

int sdpi_inspect_hostname(
	sdpi_flow_struct_t *flow_struct,
	char host_name[],
	unsigned int size
	)
{
	sdpi_flow_base_t	*flow_base = &flow_struct->flow_base;
	unsigned short 		lower_port = flow_base->lower_port;
	unsigned short 		upper_port = flow_base->upper_port;

	if(size < SDPI_MAX_HOST_NAME_LENGTH)
	{
		return -1;
	}

	if(flow_base->protocol != SDPI_TCPPROTO_VALUE)
	{
		return -1;
	}

	if(sdpi_is_http_port(lower_port) || sdpi_is_http_port(upper_port))
	{
		return sdpi_inspect_hostname_in_http(flow_struct, host_name, size);
	}

	if(sdpi_is_https_port(lower_port) || sdpi_is_https_port(upper_port))
	{
		return sdpi_inspect_hostname_in_https(flow_struct, host_name, size);
	}

	return -1;
}
