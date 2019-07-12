//////////////////////////////////////////////////////////////////////////
//DataSource_Param_Info.cpp


#include "PacketIO/DataSource_Param_Info.h"
#include "cpf/ConstructPacket.h"
#include "cpf/crc_tools.h"

Playback_Param_Info& CDataSource_Param_Info::GetCommonPlaybackParam()
{
	if( m_drvType == DRIVER_TYPE_FILE_ZCCPT )
	{
		return m_cpt_playback_param.m_playback_info;
	}
//#ifdef OS_WINDOWS
	else if( m_drvType == DRIVER_TYPE_FILE_ETHEREAL )
	{
		return m_ethreal_playback_param.m_playback_info;
	}
	else
	{
		return m_ethreal_playback_param.m_playback_info;
	}
//#else
//	ACE_ASSERT(FALSE);
	return m_cpt_playback_param.m_playback_info;
//#endif
}



void CDataSource_Param_Info::ConstrouctPacket(Mem_RecvParam_Info& mem_info,
											Mem_EditData_Info& mem_editdata_info)
{
	int offset = 0;

	mem_info.clear();

	if( mem_info.m_datatype!=1)
	{
		return;
	}

	if( mem_info.m_maxlen < 64 )
	{
		mem_info.m_maxlen = 64;
	}

	const int data_len = mem_info.m_maxlen-4;

	mem_info.m_nModelDataLen = mem_info.m_maxlen;
	mem_info.m_pModelData = new BYTE[mem_info.m_nModelDataLen];

	if( mem_editdata_info.vlan_id != -1 )
	{
		CConstructPacket::MAC_Contruct_Header(
			mem_info.m_pModelData,-1,
			mem_editdata_info.dst_mac,
			mem_editdata_info.src_mac,
			ACE_HTONS(0X8100));

		offset += 14;

		*((WORD *)(mem_info.m_pModelData+offset)) = ACE_HTONS((WORD)mem_editdata_info.vlan_id);

		offset += 2;

		*((WORD *)(mem_info.m_pModelData+offset)) = ACE_HTONS(0x0800);

		offset += 2;
	}
	else
	{
		offset = CConstructPacket::MAC_Contruct_IPType_Header(
			mem_info.m_pModelData,-1,
			mem_editdata_info.dst_mac,
			mem_editdata_info.src_mac);
	}

	unsigned char * pIPHeader = mem_info.m_pModelData+offset;
	BYTE trans_data[65536] = {0};

	memset(trans_data,0x00,sizeof(trans_data));

	if( mem_editdata_info.trans_type == 0 )
	{
		const unsigned char template_tcp_head[] =
		{0X15,0XEA,0X13,0X73,0X65,0X01,0X01,0X7F,0X59,0X8A,
		0XB0,0XDF,0X50,0X10,0XB6,0X1C,0X8E,0X7E,0X00,0X00};

		memcpy(trans_data,template_tcp_head,20);

		*(ACE_UINT16 *)(trans_data+16) = 0;

		*(ACE_UINT16 *)trans_data = ACE_HTONS(mem_editdata_info.hostorder_src_port);
		*(ACE_UINT16 *)(trans_data+2) = ACE_HTONS(mem_editdata_info.hostorder_dst_port);

		ACE_UINT16 wchecksum = CPF::CalculateTCPCheckSum(
			ACE_HTONL(mem_editdata_info.hostorder_src_ip),
			ACE_HTONL(mem_editdata_info.hostorder_dst_ip),
			trans_data,data_len-offset-20);

		*(ACE_UINT16 *)(trans_data+16) = ACE_HTONS(wchecksum);

		CConstructPacket::IP_Construct_TCPPacket(pIPHeader,
			ACE_HTONL(mem_editdata_info.hostorder_src_ip),
			ACE_HTONL(mem_editdata_info.hostorder_dst_ip),
			trans_data,data_len-offset-20,
			mem_editdata_info.ttl,mem_editdata_info.tos);

		
	}
	else if( mem_editdata_info.trans_type == 1 )
	{


#pragma pack(1)
		struct UDPPacketHead {
			unsigned short SourPort;
			unsigned short DestPort;
			unsigned short Len;
			unsigned short ChkSum;
		};
#pragma pack()

		UDPPacketHead * pUdpHeader = (UDPPacketHead *)trans_data;

		pUdpHeader->SourPort = ACE_HTONS(mem_editdata_info.hostorder_src_port);
		pUdpHeader->DestPort = ACE_HTONS(mem_editdata_info.hostorder_dst_port);
		pUdpHeader->Len = ACE_HTONS(data_len-offset-20);
		pUdpHeader->ChkSum = 0x00;

		ACE_UINT16 wchecksum = CPF::CalculateUDPCheckSum(
			ACE_HTONL(mem_editdata_info.hostorder_src_ip),
			ACE_HTONL(mem_editdata_info.hostorder_dst_ip),
			trans_data,data_len-offset-20);

		pUdpHeader->ChkSum = ACE_HTONS(wchecksum);

		CConstructPacket::IP_Construct_UDPPPacket(pIPHeader,
			ACE_HTONL(mem_editdata_info.hostorder_src_ip),
			ACE_HTONL(mem_editdata_info.hostorder_dst_ip),
			trans_data,data_len-offset-20,
			mem_editdata_info.ttl,mem_editdata_info.tos);
	}
	else if( mem_editdata_info.trans_type == 2 )
	{
		CConstructPacket::ICMP_Construct_EchoPacket(trans_data,0xac87,0x7834,
			NULL,data_len-offset-20-8);

		CConstructPacket::IP_Construct_ICMPPacket(pIPHeader,
			ACE_HTONL(mem_editdata_info.hostorder_src_ip),
			ACE_HTONL(mem_editdata_info.hostorder_dst_ip),
			trans_data,data_len-offset-20);
	}
	else if( mem_editdata_info.trans_type == 3 )
	{
		CConstructPacket::ICMP_Construct_EchoReplyPacket(trans_data,0xac87,0x7834,
			NULL,data_len-offset-offset-8);

		CConstructPacket::IP_Construct_ICMPPacket(pIPHeader,
			ACE_HTONL(mem_editdata_info.hostorder_src_ip),
			ACE_HTONL(mem_editdata_info.hostorder_dst_ip),
			trans_data,data_len-offset-20,
			mem_editdata_info.ttl,mem_editdata_info.tos);
	}


	return;

}

