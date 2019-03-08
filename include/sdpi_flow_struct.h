#ifndef _SDPI_FLOW_STRUCT_H
#define _SDPI_FLOW_STRUCT_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SDPI_FLOW_PAYLOAD_SIZE 		200
#define SDPI_ETHERTYPE_OFFSET		12		//short value	
#define SDPI_ETHERTYPE_IPPROTO		0x0008	//little endian
#define SDPI_ETHERTYPE_ARPPROTO		0x0608  //little endian

#define SDPI_IPPROTO_OFFSET			23		//byte value
#define SDPI_ICMPPROTO_VALUE		1		//icmp proto
#define SDPI_TCPPROTO_VALUE			6		//tcp proto
#define SDPI_UDPPROTO_VALUE			17		//udp proto

#define SDPI_SRCADDR_OFFSET			26		//int value
#define SDPI_DSTADDR_OFFSET			30		//little endian
#define SDPI_SRCPORT_OFFSET			34		//little endian
#define SDPI_DSTPORT_OFFSET			36		//little endian

#define SDPI_ICMP_PAYLOAD_OFFSET	42		//icmpv4
#define SDPI_TCP_PAYLOAD_OFFSET		54		//normal tcp
#define SDPI_UDP_PAYLOAD_OFFSET		42		//normal udp

#define sdpi_long_n2l(x) \
	(((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x >> 8) & 0xff00) | ((x >> 24) & 0xff))

#define sdpi_short_n2l(x) \
	(((x & 0xff) << 8) | ((x >> 8) & 0xff))

#define sdpi_is_valid_ip_packet(frame)		\
	(*(short *)((char *)frame + SDPI_ETHERTYPE_OFFSET) == SDPI_ETHERTYPE_IPPROTO)

#define sdpi_get_protocol(frame) \
	(*(unsigned char *)((char *)frame + SDPI_IPPROTO_OFFSET))

#define sdpi_is_icmp_packet(frame)	\
	(sdpi_get_protocol(frame) == SDPI_ICMPPROTO_VALUE)

#define sdpi_is_tcp_packet(frame) \
	(sdpi_get_protocol(frame) == SDPI_TCPPROTO_VALUE)

#define sdpi_is_udp_packet(frame) \
	(sdpi_get_protocol(frame) == SDPI_UDPPROTO_VALUE)

#define sdpi_get_src_addr(frame) \
	sdpi_long_n2l(*(int *)((char *)frame + SDPI_SRCADDR_OFFSET))

#define sdpi_get_dst_addr(frame) \
	sdpi_long_n2l(*(int *)((char *)frame + SDPI_DSTADDR_OFFSET))

#define sdpi_get_src_port(frame) \
	sdpi_short_n2l(*(short *)((char *)frame + SDPI_SRCPORT_OFFSET))

#define sdpi_get_dst_port(frame) \
	sdpi_short_n2l(*(short *)((char *)frame + SDPI_DSTPORT_OFFSET))

#define sdpi_get_icmp_payload(frame) \
	((char *)frame + SDPI_ICMP_PAYLOAD_OFFSET)

#define sdpi_get_tcp_payload(frame) \
	((char *)frame + SDPI_TCP_PAYLOAD_OFFSET)

#define sdpi_get_udp_payload(frame) \
	((char *)frame + SDPI_UDP_PAYLOAD_OFFSET)

#define sdpi_make_direction(src_addr, dst_addr) \
	((src_addr < dst_addr) ? SDPI_DIRECTION_L2U : SDPI_DIRECTION_U2L)

typedef enum _sdpi_flow_direction
{
	SDPI_DIRECTION_INIT = 0,
	SDPI_DIRECTION_L2U = 1,
	SDPI_DIRECTION_U2L = 2,
	SDPI_DIRECTION_BOTH = 3
}sdpi_flow_direction_t;

typedef struct _sdpi_flow_base
{
	unsigned char 		  	protocol;
	unsigned int 		  	lower_addr;
	unsigned int  		  	upper_addr;
	unsigned short 		  	lower_port;
	unsigned short 		  	upper_port;
}sdpi_flow_base_t;

typedef struct _sdpi_flow_struct
{
	sdpi_flow_base_t		flow_base;
	sdpi_flow_direction_t 	direction;
	unsigned short 		  	lower_size;
	unsigned short 			upper_size;
	char 	    		  	lower_payload[SDPI_FLOW_PAYLOAD_SIZE];
	char 					upper_payload[SDPI_FLOW_PAYLOAD_SIZE];
}sdpi_flow_struct_t;

int
sdpi_make_flow_base_by_frame(
	sdpi_flow_base_t *flow_base,
	char *frame,
	unsigned int frame_size
	);

int
sdpi_make_flow_struct_by_frame(
	sdpi_flow_struct_t *flow_struct,
	char *frame,
	unsigned int frame_size
	);

void
sdpi_make_flow_base_by_fields(
	sdpi_flow_base_t *flow_base,
	unsigned int protocol,
	unsigned int src_addr,
	unsigned int dst_addr,
	unsigned short src_port,
	unsigned short dst_port
	);

int
sdpi_make_flow_struct_by_fields(
	sdpi_flow_struct_t *flow_struct,
	sdpi_flow_base_t *flow_base,
	sdpi_flow_direction_t direction,
	char *payload,
	unsigned short payload_size
	);

#endif