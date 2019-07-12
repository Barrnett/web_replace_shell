//////////////////////////////////////////////////////////////////////////
//IPv4FragmentTool.cpp

#include "cpf/IPv4FragmentTool.h"
#include "cpf/proto_struct_def.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonPPPOEDecode.h"
#include "cpf/crc_tools.h"

//////////////////////////////////////////////////////////////////////////
//CIPv4FragmentTool
//////////////////////////////////////////////////////////////////////////

#define IP_FRAGMENT(pIPHeader) ((*((ACE_UINT16*)(pIPHeader+6)))&0xFF7F)

CIPv4FragmentTool::CIPv4FragmentTool(void)
{
	m_item_nums = 0;
}

CIPv4FragmentTool::~CIPv4FragmentTool(void)
{
}

int CIPv4FragmentTool::DoFragment_By_MaxIPLen(const BYTE * mac_data_header,const BYTE * ip_header_data,int ip_total_len,int max_ip_fragment_len)
{
	m_item_nums = 0;

	if( ip_total_len <= max_ip_fragment_len )
	{
		ACE_ASSERT(FALSE);
		
		memcpy( m_items[0].data,mac_data_header,(int)(ip_header_data-mac_data_header)+ip_total_len);

		m_items[0].data_len = (int)(ip_header_data-mac_data_header)+ip_total_len;

		m_item_nums = 1;

		return 1;
	}

	const BYTE * pppoe_header_data = CCommonPPPOEDecode::GetPPPOEHeaderPos(mac_data_header);

	const int mac_header_len = (int)(ip_header_data-mac_data_header);

	const int dwIPHeaderLength = (int)((ip_header_data[0] & 0x0F) << 2);

	int max_ip_data_fragment_len = max_ip_fragment_len-dwIPHeaderLength;

	//将数据长度变成8的整数倍,必须变成8的整数倍，因为分片的单位就是8个字节
	max_ip_data_fragment_len >>= 3;
	max_ip_data_fragment_len <<= 3;

	int remainder_ip_data_len = ip_total_len-dwIPHeaderLength;

	ACE_UINT16 old_fragment_flag = IP_FRAGMENT(ip_header_data);//get the fragment-sig

	//分片偏移
	unsigned int ho_frag_offeset = (unsigned int)CCommonIPv4Decode::IPv4_DECODE_INFO::Fragment_Offset(old_fragment_flag);

	const BYTE * ip_data = (ip_header_data + dwIPHeaderLength);

	while( remainder_ip_data_len > 0 )
	{
		BYTE * pthis_seg_data = m_items[m_item_nums].data;

		//mac头和ip头
		memcpy(pthis_seg_data,mac_data_header,mac_header_len+dwIPHeaderLength);
		pthis_seg_data += mac_header_len+dwIPHeaderLength;

		//数据
		int this_seg_ip_data_len = mymin(max_ip_data_fragment_len,remainder_ip_data_len);

		if( pppoe_header_data )
		{
			pppoe_header * new_pppoe_header = (pppoe_header *)(m_items[m_item_nums].data + (LONG_PTR)(pppoe_header_data - mac_data_header));

			new_pppoe_header->payload_len = ACE_HTONS(2+dwIPHeaderLength+this_seg_ip_data_len);
		}
	
		memcpy(pthis_seg_data,ip_data,this_seg_ip_data_len);
		pthis_seg_data += this_seg_ip_data_len;
		ip_data += this_seg_ip_data_len;

		IPv4PacketHead* pThisIPv4Header = (IPv4PacketHead*)(m_items[m_item_nums].data+mac_header_len);

		ACE_UINT16 fragment_flag = (pThisIPv4Header->FlgOff&0XFF7F);

		//设置分片位
		fragment_flag &= (~0x40);

		if( remainder_ip_data_len > max_ip_data_fragment_len )
		{
			//设置more位
			fragment_flag |= 0x20;

			//取消more位不需要我们做,

		}

		{//分片偏移

			fragment_flag &= ~0XFF1F;

			ACE_UINT16 net_order_frag_offeset = ACE_HTONS(ho_frag_offeset/8);

			fragment_flag |= net_order_frag_offeset;
		}
	
		pThisIPv4Header->FlgOff = (fragment_flag&0XFF7F);

		pThisIPv4Header->TtlLen = ACE_HTONS(this_seg_ip_data_len+dwIPHeaderLength);

		remainder_ip_data_len -= this_seg_ip_data_len;

		pThisIPv4Header->ChkSum = 0;
		pThisIPv4Header->ChkSum = CPF::ComputeCheckSum_NetOrder((const BYTE *)pThisIPv4Header,dwIPHeaderLength);

		m_items[m_item_nums].data_len = (int)(pthis_seg_data-m_items[m_item_nums].data);

		++m_item_nums;

		ho_frag_offeset += this_seg_ip_data_len;
	}

	return m_item_nums;
	
}


int CIPv4FragmentTool::DoFragment_By_FragmentNum(const BYTE * mac_data_header,const BYTE * ip_header_data,int ip_total_len,int fragment_num)
{
	const BYTE * pppoe_header_data = CCommonPPPOEDecode::GetPPPOEHeaderPos(mac_data_header);

	m_item_nums = 0;

	const int mac_header_len = (int)(ip_header_data-mac_data_header);

	const int dwIPHeaderLength = (int)((ip_header_data[0] & 0x0F) << 2);

	int total_ip_data_len = ip_total_len-dwIPHeaderLength;

	int remainder_ip_data_len = total_ip_data_len;

	ACE_UINT16 old_fragment_flag = IP_FRAGMENT(ip_header_data);//get the fragment-sig

	//分片偏移
	unsigned int ho_frag_offeset = (unsigned int)CCommonIPv4Decode::IPv4_DECODE_INFO::Fragment_Offset(old_fragment_flag);

	const BYTE * ip_data = (ip_header_data + dwIPHeaderLength);

	{
		if( fragment_num <= 0 ) 
		{
			fragment_num = 1;
		}

		int max_fragment_nums = 0;//最多可以分几片

		if( total_ip_data_len % 8 == 0 )
			max_fragment_nums = total_ip_data_len/8;
		else
			max_fragment_nums =  total_ip_data_len/8+1;

		if( fragment_num > max_fragment_nums )
		{
			fragment_num = max_fragment_nums;
		}
	}

	for(int i = 0; i < fragment_num; ++i)
	{
		BYTE * pthis_seg_data = m_items[m_item_nums].data;

		int this_seg_ip_data_len = 0;

		if( i != fragment_num -1 )
		{
			this_seg_ip_data_len = total_ip_data_len/fragment_num;

			//将数据长度变成8的整数倍,必须变成8的整数倍，因为分片的单位就是8个字节
			this_seg_ip_data_len >>= 3;
			this_seg_ip_data_len <<= 3;
		}
		else
		{
			this_seg_ip_data_len = remainder_ip_data_len;
		}

		//mac头和ip头
		memcpy(pthis_seg_data,mac_data_header,mac_header_len+dwIPHeaderLength);
		pthis_seg_data += mac_header_len+dwIPHeaderLength;

		if( pppoe_header_data )
		{
			pppoe_header * new_pppoe_header = (pppoe_header *)(m_items[m_item_nums].data + (LONG_PTR)(pppoe_header_data - mac_data_header));

			new_pppoe_header->payload_len = ACE_HTONS(2+dwIPHeaderLength+this_seg_ip_data_len);
		}

		memcpy(pthis_seg_data,ip_data,this_seg_ip_data_len);
		pthis_seg_data += this_seg_ip_data_len;
		ip_data += this_seg_ip_data_len;

		IPv4PacketHead* pThisIPv4Header = (IPv4PacketHead*)(m_items[m_item_nums].data+mac_header_len);

		ACE_UINT16 fragment_flag = (pThisIPv4Header->FlgOff&0XFF7F);

		//设置分片位
		fragment_flag &= (~0x40);

		if( remainder_ip_data_len > this_seg_ip_data_len )
		{
			//设置more位
			fragment_flag |= 0x20;

			//取消more位不需要我们做,跟着以前的包的标志，因为以前的包也可能是分片的最后一包也可能不是最后一包。

		}

		{//分片偏移

			fragment_flag &= ~0XFF1F;

			ACE_UINT16 net_order_frag_offeset = ACE_HTONS(ho_frag_offeset/8);

			fragment_flag |= net_order_frag_offeset;
		}

		pThisIPv4Header->FlgOff = (fragment_flag&0XFF7F);

		pThisIPv4Header->TtlLen = ACE_HTONS(this_seg_ip_data_len+dwIPHeaderLength);

		remainder_ip_data_len -= this_seg_ip_data_len;

		pThisIPv4Header->ChkSum = 0;
		pThisIPv4Header->ChkSum = CPF::ComputeCheckSum_NetOrder((const BYTE *)pThisIPv4Header,dwIPHeaderLength);

		m_items[m_item_nums].data_len = (int)(pthis_seg_data-m_items[m_item_nums].data);

		++m_item_nums;

		ho_frag_offeset += this_seg_ip_data_len;
	}

	return m_item_nums;
}


//////////////////////////////////////////////////////////////////////////
//CTCPFragmentTool
//////////////////////////////////////////////////////////////////////////


CTCPFragmentTool::CTCPFragmentTool(void)
{
	m_item_nums = 0;
}

CTCPFragmentTool::~CTCPFragmentTool(void)
{
}

int CTCPFragmentTool::DoFragment_By_MacMaxFrame(const BYTE * mac_data_header,const BYTE * ip_header_data,const BYTE * trans_header,
			   const BYTE * app_header,int total_mac_len,int max_mac_fragment_len)
{
	m_item_nums = 0;

	if( max_mac_fragment_len < 60 )
	{
		ACE_ASSERT(FALSE);
		return 0;
	}

	if( total_mac_len <= max_mac_fragment_len )
	{
		BYTE * dest_buffer = m_items[m_item_nums].data;

		memcpy(dest_buffer,mac_data_header,total_mac_len);

		m_item_nums = 1;

		return 1;
	}
	
	const BYTE * pppoe_header_data = CCommonPPPOEDecode::GetPPPOEHeaderPos(mac_data_header);

	unsigned int last_seq_no = (unsigned int)ACE_NTOHL(((TCPPacketHead*)trans_header)->SeqNo);

	//ip和tcp的头长度
	unsigned int nIPAndTcpHeaderLen  = (unsigned int)(app_header-ip_header_data);

	const unsigned int nMacAndIPAndTCPHeaderLen = (unsigned int)(app_header-mac_data_header);
	const unsigned int nTcpHeaderLen = (unsigned int)(app_header -trans_header);

	int total_app_len = (int)(total_mac_len-nMacAndIPAndTCPHeaderLen);

	int max_app_len = (int)(max_mac_fragment_len-nMacAndIPAndTCPHeaderLen);//分片包，最大的app长度

	int remainder_app_len = total_app_len;//剩余的app包

	unsigned short old_TtlLen = ((IPv4PacketHead *)ip_header_data)->TtlLen;//网络字节序
	unsigned short old_ip_check_sum = ((IPv4PacketHead *)ip_header_data)->ChkSum;//网络字节序

	const BYTE * src_app_buffer = app_header;

	while( remainder_app_len > 0 )
	{
		BYTE * const dest_buffer = m_items[m_item_nums].data;

		//拷贝mac，ip，和传输层的头部
		memcpy(dest_buffer,mac_data_header,nMacAndIPAndTCPHeaderLen);

		TCPPacketHead* pTCP = (TCPPacketHead*)(dest_buffer + (int)(trans_header-mac_data_header));
		IPv4PacketHead* pIPv4 = (IPv4PacketHead*)(dest_buffer + (int)(ip_header_data-mac_data_header));

		BYTE * dest_app_buffer = dest_buffer + (int)(nMacAndIPAndTCPHeaderLen);

		pTCP->SeqNo = ACE_HTONL(last_seq_no);

		unsigned int this_app_len = 0;

		if( remainder_app_len >= max_app_len )
			this_app_len = max_app_len;
		else
			this_app_len = remainder_app_len;

		memcpy(dest_app_buffer,src_app_buffer,this_app_len);

		last_seq_no += this_app_len;
		src_app_buffer += this_app_len;

		pTCP->ChkSum = 0;
		pTCP->ChkSum = CPF::Calculate_trans_CheckSum_NetOrder(pIPv4->SourIP,pIPv4->DestIP,pIPv4->Proto,(const BYTE *)pTCP,this_app_len+nTcpHeaderLen);

		pIPv4->ChkSum = CPF::csum_incremental_update3(old_ip_check_sum,old_TtlLen,ACE_HTONS(nIPAndTcpHeaderLen + this_app_len));

		pIPv4->TtlLen = ACE_HTONS(nIPAndTcpHeaderLen + this_app_len);

		if( pppoe_header_data )
		{
			pppoe_header * new_pppoe_header = (pppoe_header *)(dest_buffer + (LONG_PTR)(pppoe_header_data - mac_data_header));

			new_pppoe_header->payload_len = ACE_HTONS(2+nIPAndTcpHeaderLen + this_app_len);
		}

		remainder_app_len -= this_app_len;

		m_items[m_item_nums].data_len = this_app_len+nMacAndIPAndTCPHeaderLen;

		++m_item_nums;

	}

	return m_item_nums;

}


int CTCPFragmentTool::DoFragment_By_AppMaxFrame(const BYTE * mac_data_header,const BYTE * ip_header_data,const BYTE * trans_header,
							  const BYTE * app_header,int total_mac_len,int max_app_fragment_len)
{
	int max_mac_fragment_len = max_app_fragment_len+(int)(app_header-mac_data_header);

	return DoFragment_By_MacMaxFrame(mac_data_header,ip_header_data,trans_header,app_header,total_mac_len,max_mac_fragment_len);
}

int CTCPFragmentTool::DoFragment_By_FragmentNum(const BYTE * mac_data_header,const BYTE * ip_header_data,const BYTE * trans_header,
							  const BYTE * app_header,int total_mac_len,int fragment_num)
{
	const BYTE * pppoe_header_data = CCommonPPPOEDecode::GetPPPOEHeaderPos(mac_data_header);

	m_item_nums = 0;

	unsigned int last_seq_no = (unsigned int)ACE_NTOHL(((TCPPacketHead*)trans_header)->SeqNo);


	//ip和tcp的头长度
	unsigned int nIPAndTcpHeaderLen  = (unsigned int)(app_header-ip_header_data);

	const unsigned int nMacAndIPAndTCPHeaderLen = (unsigned int)(app_header-mac_data_header);
	const unsigned int nTcpHeaderLen = (unsigned int)(app_header -trans_header);

	int total_app_len = (int)(total_mac_len-nMacAndIPAndTCPHeaderLen);

	if( fragment_num <= 0 ) 
	{
		fragment_num = 1;
	}

	if( fragment_num > total_app_len )
	{
		fragment_num = total_app_len;
	}

	int remainder_app_len = total_app_len;

	const BYTE * src_app_buffer = app_header;

	unsigned short old_TtlLen = ((IPv4PacketHead *)ip_header_data)->TtlLen;//网络字节序
	unsigned short old_ip_check_sum = ((IPv4PacketHead *)ip_header_data)->ChkSum;//网络字节序

	for(int i = 0; i < fragment_num; ++i)
	{
		unsigned int this_app_len = 0;

		if( i != fragment_num -1 )
		{
			this_app_len = total_app_len/fragment_num;
		}
		else
		{
			this_app_len = remainder_app_len;
		}

		BYTE * const dest_buffer = m_items[m_item_nums].data;

		memcpy(dest_buffer,mac_data_header,nMacAndIPAndTCPHeaderLen);

		TCPPacketHead* pTCP = (TCPPacketHead*)(dest_buffer + (int)(trans_header-mac_data_header));
		IPv4PacketHead* pIPv4 = (IPv4PacketHead*)(dest_buffer + (int)(ip_header_data-mac_data_header));

		BYTE * dest_app_buffer = dest_buffer + (int)(nMacAndIPAndTCPHeaderLen);

		pTCP->SeqNo = ACE_HTONL(last_seq_no);

		memcpy(dest_app_buffer,src_app_buffer,this_app_len);

		last_seq_no += this_app_len;
		src_app_buffer += this_app_len;

		pTCP->ChkSum = 0;
		pTCP->ChkSum = CPF::Calculate_trans_CheckSum_NetOrder(pIPv4->SourIP,pIPv4->DestIP,pIPv4->Proto,(const BYTE *)pTCP,this_app_len+nTcpHeaderLen);

		pIPv4->ChkSum = CPF::csum_incremental_update3(old_ip_check_sum,old_TtlLen,ACE_HTONS(nIPAndTcpHeaderLen + this_app_len));

		pIPv4->TtlLen = ACE_HTONS(nIPAndTcpHeaderLen + this_app_len);

		if( pppoe_header_data )
		{
			pppoe_header * new_pppoe_header = (pppoe_header *)(dest_buffer + (LONG_PTR)(pppoe_header_data - mac_data_header));

			new_pppoe_header->payload_len = ACE_HTONS(2+nIPAndTcpHeaderLen + this_app_len);
		}

		remainder_app_len -= this_app_len;

		m_items[m_item_nums].data_len = this_app_len+nMacAndIPAndTCPHeaderLen;

		++m_item_nums;

	}

	return m_item_nums;
}
