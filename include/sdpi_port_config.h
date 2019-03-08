﻿#ifndef _SDPI_PORT_CONFIG_H
#define	_SDPI_PORT_CONFIG_H
#include "sdpi_flow_parser.h"
#include "sdpi_port_struct.h"

typedef struct _sdpi_port_config
{
	unsigned short 			port;
	sdpi_inspect_result_t 	result;
}sdpi_port_config_t;

static sdpi_inspect_result_t
sdpi_unknown_port_result = \
	{SDPI_INSPECTED_BY_PORT, SDPI_APP_UNKNOWN, SDPI_APP_CATEGORY_UNKNOWN, "port unknown"};

static const sdpi_port_config_t 
sdpi_uport_config_list[] = \
{
	{ 137,		{ SDPI_INSPECTED_BY_PORT, SDPI_APP_OS_WINDOWS, SDPI_APP_CATEGORY_OS, "NetBIOS" } },
	{ 138,		{ SDPI_INSPECTED_BY_PORT, SDPI_APP_OS_WINDOWS, SDPI_APP_CATEGORY_OS, "NetBIOS" } },
	{ 161,		{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_UDP, SDPI_APP_CATEGORY_NETWORK, "SNMP" } },
	{ 162,		{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_UDP, SDPI_APP_CATEGORY_NETWORK, "SNMP" } },	
	{ 1900,		{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_UDP, SDPI_APP_CATEGORY_NETWORK, "SSDP" } },
	{ 5355,		{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_UDP, SDPI_APP_CATEGORY_NETWORK, "LLMNR" } },
	{ 5938,		{ SDPI_INSPECTED_BY_PORT, SDPI_APP_REMOTE_ACCESS_TEAMVIEW, SDPI_APP_CATEGORY_REMOTE_ACCESS, "TeamView" } },
	{ 12345,	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_P2P_XUNLEI, SDPI_APP_CATEGORY_P2P, "XunLei" } },  //本地地址优先
} ;

static const sdpi_port_config_t 
sdpi_tport_config_list[] = \
{
	{ 20,   	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_DOWNLOAD_FTP, SDPI_APP_CATEGORY_DOWNLOAD, "FTP Connect"} },
	{ 21,    	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_DOWNLOAD_FTP, SDPI_APP_CATEGORY_DOWNLOAD, "FTP Download"} },
	{ 22,    	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_REMOTE_ACCESS_SSH, SDPI_APP_CATEGORY_REMOTE_ACCESS, "SSH"} },
	{ 23,    	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_REMOTE_ACCESS_TELNET, SDPI_APP_CATEGORY_REMOTE_ACCESS, "Telnet"} },
	{ 25,    	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_MAIL_UNKNOWN, SDPI_APP_CATEGORY_MAIL, "SMTP"} },
	{ 53,    	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_UDP, SDPI_APP_CATEGORY_NETWORK, "DNS" } },
	{ 69,    	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_DOWNLOAD_FTP, SDPI_APP_CATEGORY_DOWNLOAD, "TFTP" } },
	{ 80,    	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_TCP, SDPI_APP_CATEGORY_NETWORK, "HTTP" } },
	{ 110,   	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_MAIL_UNKNOWN, SDPI_APP_CATEGORY_MAIL, "POP3"} },
	{ 135,   	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_OS_WINDOWS, SDPI_APP_CATEGORY_OS, "Microsoft RPC" } },
	{ 139,   	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_OS_WINDOWS, SDPI_APP_CATEGORY_OS, "SMB" } },
	{ 443,   	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_TCP, SDPI_APP_CATEGORY_NETWORK, "HTTPS" } },
	{ 445,   	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_OS_WINDOWS, SDPI_APP_CATEGORY_OS, "SMB" } },
	{ 1433,  	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_DATABASE_SQLSERVER, SDPI_APP_CATEGORY_DATABASE, "SQLServer"} },
	{ 3306,  	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_DATABASE_MYSQL, SDPI_APP_CATEGORY_DATABASE, "MySQL"} },
	{ 3389,  	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_REMOTE_ACCESS_RDP, SDPI_APP_CATEGORY_REMOTE_ACCESS, "Remote Desktop"} },
	{ 5938,  	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_REMOTE_ACCESS_TEAMVIEW, SDPI_APP_CATEGORY_REMOTE_ACCESS, "TeamView" } },
	{ 8000,  	{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_TCP, SDPI_APP_CATEGORY_NETWORK, "HTTP" } },
	{ 8080,		{ SDPI_INSPECTED_BY_PORT, SDPI_APP_NETWORK_TCP, SDPI_APP_CATEGORY_NETWORK, "HTTP" } },
};

#endif