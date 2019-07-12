//////////////////////////////////////////////////////////////////////////
//CommonDHCPDecode.cpp

#include "cpf/CommonDHCPDecode.h"

CCommonDHCPDecode::CCommonDHCPDecode(void)
{
}

CCommonDHCPDecode::~CCommonDHCPDecode(void)
{
}

void CCommonDHCPDecode::DHCPProtocolDecode(const BYTE * pPacket, DWORD dwPacketLen, DHCP_DECODE_INFO& dhcp_info)
{
	dhcp_info.reset();

	if( dwPacketLen <= sizeof(DHCP_DECODE_INFO_HEADER) )
	{
		dhcp_info.nErrorType = 1;

		return;
	}

	memcpy(&dhcp_info.dhcp_header,pPacket,sizeof(DHCP_DECODE_INFO_HEADER));

	const BYTE * option = pPacket + sizeof(DHCP_DECODE_INFO_HEADER);

	while( 1 )
	{
		if( option - pPacket > (int)dwPacketLen )
		{
			break;
		}

		int type = (int)(*option);

		if( type == 0xff )
		{
			break;
		}

		++option;

		int len = (int)(*option);

		++option;

		switch(type)
		{
		case 53://dhcp msg type
			if( len != 1 )
			{
				dhcp_info.nErrorType = 1;
				return;
			}
			dhcp_info.dhcp_msg_type = (int)(*option);
			break;

		case 1://subnet
			if( len != 4 )
			{
				dhcp_info.nErrorType = 1;
				return;
			}

			dhcp_info.subnet_mask = *(ACE_UINT32 *)option;
			break;

		case 3://router
			if( len != 4 )
			{
				dhcp_info.nErrorType = 1;
				return;
			}

			dhcp_info.subnet_mask = *(ACE_UINT32 *)option;
			break;

		case 50://request_ip
			if( len != 4 )
			{
				dhcp_info.nErrorType = 1;
				return;
			}

			dhcp_info.request_ip = *(ACE_UINT32 *)option;
			break;

		case 54://Option: (t=54,l=4) Server Identifier = 192.168.0.1
			if( len == 4 )
			{
				dhcp_info.server_addr = *(ACE_UINT32 *)option;
			}
			else
			{
				dhcp_info.server_addr = 0;
			}
			break;

		default:
			break;
		}

		option += len;
	}

	return;
}
