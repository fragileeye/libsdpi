#include "sdpi_flow_struct.h"

int
sdpi_make_flow_base_by_frame(
	sdpi_flow_base_t *flow_base,
	char *frame,
	unsigned int frame_size
	)
{
	unsigned char 		protocol;
	unsigned int 		src_addr;
	unsigned int  		dst_addr;
	unsigned short 		src_port;
	unsigned short 		dst_port;

	if(!sdpi_is_valid_ip_packet(frame))
	{
		return -1;
	}

	protocol = sdpi_get_protocol(frame);
	src_addr = sdpi_get_src_addr(frame);
	dst_addr = sdpi_get_dst_addr(frame);

	if(sdpi_is_icmp_packet(frame))
	{
		src_port = dst_port = 0;
	}
	else if(sdpi_is_tcp_packet(frame))
	{
		src_port = sdpi_get_src_port(frame);
		dst_port = sdpi_get_dst_port(frame);
	}
	else if(sdpi_is_udp_packet(frame))
	{
		src_port = sdpi_get_src_port(frame);
		dst_port = sdpi_get_dst_port(frame);
	}
	else
	{
		return -1;
	}

	flow_base->protocol = protocol;
	
	if(src_addr < dst_addr)
	{
		flow_base->lower_addr = src_addr;
		flow_base->lower_port = src_port;
		flow_base->upper_addr = dst_addr;
		flow_base->upper_port = dst_port;
	}
	else
	{
		flow_base->lower_addr = dst_addr;
		flow_base->lower_port = dst_port;
		flow_base->upper_addr = src_addr;
		flow_base->upper_port = src_port;
	}

	return 0;
}

int
sdpi_make_flow_struct_by_frame(
	sdpi_flow_struct_t *flow_struct,
	char *frame,
	unsigned int frame_size
	)
{
	sdpi_flow_base_t		*flow_base = &flow_struct->flow_base;
	sdpi_flow_direction_t 	direction = SDPI_DIRECTION_INIT;
	unsigned char			protocol;
	unsigned int 			src_addr;
	unsigned int  			dst_addr;
	unsigned short			src_port;
	unsigned short			dst_port;
	unsigned short 			payload_size;
	char 					*payload;
	
	if(!sdpi_is_valid_ip_packet(frame)) 
	{
		return -1;
	}

	src_addr = sdpi_get_src_addr(frame);
	dst_addr = sdpi_get_dst_addr(frame);
	direction = sdpi_make_direction(src_addr, dst_addr);

	if(sdpi_is_icmp_packet(frame))
	{
		payload = sdpi_get_icmp_payload(frame);
		payload_size = frame_size - SDPI_ICMP_PAYLOAD_OFFSET;
	}
	else if(sdpi_is_tcp_packet(frame))
	{
		payload = sdpi_get_tcp_payload(frame);
		payload_size = frame_size - SDPI_TCP_PAYLOAD_OFFSET;
	}
	else if(sdpi_is_udp_packet(frame))
	{
		payload = sdpi_get_udp_payload(frame);
		payload_size = frame_size - SDPI_UDP_PAYLOAD_OFFSET;
	}
	else
	{
		return -1;
	}

	if(payload_size > SDPI_FLOW_PAYLOAD_SIZE)
	{
		payload_size = SDPI_FLOW_PAYLOAD_SIZE;
	}

	if(flow_struct->direction == SDPI_DIRECTION_INIT)
	{
		protocol = sdpi_get_protocol(frame);
		src_port = sdpi_get_src_port(frame);
		dst_port = sdpi_get_dst_port(frame);

		sdpi_make_flow_base_by_fields(flow_base, protocol, 
			src_addr, dst_addr, src_port, dst_port);

		flow_struct->direction = direction;

		if(direction == SDPI_DIRECTION_L2U)
		{
			flow_struct->lower_size = payload_size;
			memcpy(flow_struct->lower_payload, payload, payload_size);
		}
		else //direction == SDPI_DIRECTION_U2L
		{
			flow_struct->upper_size = payload_size;
			memcpy(flow_struct->upper_payload, payload, payload_size);
		}
	}
	else
	{
		flow_struct->direction |= direction;

		if(direction == SDPI_DIRECTION_L2U && flow_struct->lower_size == 0)
		{
			flow_struct->lower_size = payload_size;
			memcpy(flow_struct->lower_payload, payload, payload_size);
		}
		else if(direction == SDPI_DIRECTION_U2L && flow_struct->upper_size == 0)
		{
			flow_struct->upper_size = payload_size;
			memcpy(flow_struct->upper_payload, payload, payload_size);
		}
	}

	return 0;
}

void
sdpi_make_flow_base_by_fields(
	sdpi_flow_base_t *flow_base,
	unsigned int protocol,
	unsigned int src_addr,
	unsigned int dst_addr,
	unsigned short src_port,
	unsigned short dst_port
	)
{
	flow_base->protocol = protocol;

	if(src_addr < dst_addr)
	{
		flow_base->lower_addr = src_addr;
		flow_base->lower_port = src_port;
		flow_base->upper_addr = dst_addr;
		flow_base->upper_port = dst_port;
	}
	else
	{
		flow_base->lower_addr = dst_addr;
		flow_base->lower_port = dst_port;
		flow_base->upper_addr = src_addr;
		flow_base->upper_port = src_port;
	}
}

//根据收到的信息字段生成flow_struct
int
sdpi_make_flow_struct_by_fields(
	sdpi_flow_struct_t *flow_struct,
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	)
{
	if(flow_base->protocol != SDPI_ICMPPROTO_VALUE && 
	   flow_base->protocol != SDPI_TCPPROTO_VALUE && 
	   flow_base->protocol != SDPI_UDPPROTO_VALUE )
	{
		return -1;
	}

	if(payload_size > SDPI_FLOW_PAYLOAD_SIZE)
	{
		payload_size = SDPI_FLOW_PAYLOAD_SIZE;
	}

	if(flow_struct->direction == SDPI_DIRECTION_INIT)
	{
		flow_struct->flow_base = *flow_base;
		flow_struct->direction = direction;

		if(direction == SDPI_DIRECTION_L2U)
		{
			flow_struct->lower_size = payload_size;
			memcpy(flow_struct->lower_payload, payload, payload_size);
		}
		else //direction == SDPI_DIRECTION_U2L
		{
			flow_struct->upper_size = payload_size;
			memcpy(flow_struct->upper_payload, payload, payload_size);
		}
	}
	else
	{
		flow_struct->direction |= direction;

		if(direction == SDPI_DIRECTION_L2U && flow_struct->lower_size == 0)
		{
			flow_struct->lower_size = payload_size;
			memcpy(flow_struct->lower_payload, payload, payload_size);
		}
		else if(direction == SDPI_DIRECTION_U2L && flow_struct->upper_size == 0)
		{
			flow_struct->upper_size = payload_size;
			memcpy(flow_struct->upper_payload, payload, payload_size);
		}
	}

	return 0;
}
