//////////////////////////////////////////////////////////////////////////
//CommonDHCPDecode.h

#pragma once

/*
2131 Dynamic Host Configuration Protocol. R. Droms. March 1997.
(Format: TXT=113738 bytes) (Obsoletes RFC1541) (Updated by RFC3396)
(Status: DRAFT STANDARD)

2132 DHCP Options and BOOTP Vendor Extensions. S. Alexander, R. Droms.
March 1997. (Format: TXT=63670 bytes) (Obsoletes RFC1533) (Updated by
RFC3442, RFC3942) (Status: DRAFT STANDARD)


o DHCP must interoperate with the BOOTP relay agent behavior as
described by RFC 951 and by RFC 1542 [21].

9.6. DHCP Message Type

This option is used to convey the type of the DHCP message.  The code
for this option is 53, and its length is 1.  Legal values for this
option are:

Value   Message Type
-----   ------------
1     DHCPDISCOVER
2     DHCPOFFER
3     DHCPREQUEST
4     DHCPDECLINE
5     DHCPACK
6     DHCPNAK
7     DHCPRELEASE
8     DHCPINFORM

Code   Len  Type
+-----+-----+-----+
|  53 |  1  | 1-9 |
+-----+-----+-----+

*/

#include "cpf/cpf.h"

class CPF_CLASS CCommonDHCPDecode
{
public:
	enum{
		INVALID = 0,
		DHCPDISCOVER = 1,
		DHCPOFFER = 2,
		DHCPREQUEST = 3,
		DHCPDECLINE = 4,
		DHCPACK = 5,
		DHCPNAK = 6,
		DHCPRELEASE = 7,
		DHCPINFORM = 8
	};

public:
	CCommonDHCPDecode(void);
	~CCommonDHCPDecode(void);

public:

#pragma pack(push)
#pragma pack(1)
	typedef struct
	{
		BYTE			type;//1=req,2=reply
		BYTE			htype;//1=ethernet
		BYTE			hlen;
		BYTE			hops;
		ACE_UINT32		id;
		ACE_UINT16		elapse;

		ACE_UINT16		flag;
		ACE_UINT32		client_ip;
		ACE_UINT32		your_client_ip;

		ACE_UINT32		next_server_ip;
		ACE_UINT32		rleay_agent_ip;

		BYTE			client_haddr[16];
		BYTE			server_host_name[64];
		BYTE			boot_file_name[8*16];

		ACE_UINT32		magic_cookie;

	}DHCP_DECODE_INFO_HEADER;

	typedef struct tag_DHCP_DECODE_INFO
	{
		tag_DHCP_DECODE_INFO()
		{
			reset();
		}

		void reset()
		{
			memset((void *)this,0x00,sizeof(*this));
		}

		int						nErrorType;
		DHCP_DECODE_INFO_HEADER	dhcp_header;

		int						dhcp_msg_type;//0表示无效
		ACE_UINT32				request_ip;//0表示无效
		ACE_UINT32				subnet_mask;//0表示无效
		ACE_UINT32				router_addr;//0表示无效
		ACE_UINT32				server_addr;//0表示无效

		//char					option[1];

		//Option: (t=53,l=1) DHCP Message Type = DHCP ACK		35 01 05
		//Option: (t=1,l=4) Subnet Mask = 255.255.255.0			01 04 ff ff ff ff
		//Option: (t=3,l=4) Router = 192.168.0.1				03 04 c0 a8 00 01
		//Option: (t=6,l=8) Domain Name Server					06 08 ca 6a 00 14 ca 6a 2e 97
		//Option: (t=51,l=4) IP Address Lease Time = 1 day		33 04 00 01 51 80
		//Option: (t=54,l=4) Server Identifier = 192.168.0.1	36 04 c0 a8 00 01
		//End Option											ff

	}DHCP_DECODE_INFO;


#pragma pack(pop)

public:
	static void  DHCPProtocolDecode(const BYTE * pPacket, DWORD dwPacketLen, DHCP_DECODE_INFO& dhcp_info);


};
