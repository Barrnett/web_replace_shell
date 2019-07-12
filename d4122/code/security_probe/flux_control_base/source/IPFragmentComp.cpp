//////////////////////////////////////////////////////////////////////////
//IPFragmentComp.cpp

#include "stdafx.h"
#include "IPFragmentComp.h"
#include "tcpip_mgr_common_init.h"
#include "cpf/IPv4FragmentTool.h"
#include "cpf/proto_struct_def.h"
#include "cpf/crc_tools.h"

CIPFragmentComp::CIPFragmentComp(void)
{
	m_last_checktime = 0;
}

CIPFragmentComp::~CIPFragmentComp(void)
{
}

void CIPFragmentComp::init(int max_item)
{
	m_ipfrag_finder.Create(max_item,65536,true,NULL);

}

void CIPFragmentComp::close()
{
	int index = m_ipfrag_finder.GetHeadBlock();

	while( index != -1 )
	{
		IP_FRAGMENT_ITEM * pItem = m_ipfrag_finder.GetNextBlockFromHead(index);

		if( pItem->buffer )
		{
			CPF::MyMemFree(pItem->buffer);
			pItem->buffer = NULL;
		}
	}

	m_ipfrag_finder.Destroy();

	return;

}


BOOL CIPFragmentComp::HandleFragment(PACKET_INFO_EX& packet_info)
{
	ACE_ASSERT( !CIPv4FragmentTool::fragment_is_not(packet_info.tcpipContext.dwIPFragment) );

	const IPv4PacketHead* pIPv4 = (const IPv4PacketHead*)packet_info.tcpipContext.pIPHeader;

	IP_FRAGMENT_KEY key;
	key.srcip = packet_info.tcpipContext.conn_addr.client.dwIP;
	key.dstip = packet_info.tcpipContext.conn_addr.server.dwIP;
	key.id =  pIPv4->Id;

	
	IP_FRAGMENT_ITEM* pItem = m_ipfrag_finder.FindOnly(key);

	if( !pItem )
	{
		pItem = m_ipfrag_finder.AllocBlockByKey(key);

		if( pItem )
		{
			pItem->id = key.id;
			pItem->srcip = key.srcip;
			pItem->dstip = key.dstip;

			pItem->total_trans_pkt_len = 0;

			pItem->buffer = CPF::MyMemAlloc(MAX_BUFFER_LEN);

			pItem->ip_offset = (int)(packet_info.tcpipContext.pIPHeader-packet_info.tcpipContext.pMACHeader);
			pItem->trans_offset = (int)(packet_info.tcpipContext.pTransHeader-packet_info.tcpipContext.pMACHeader);

			if( packet_info.tcpipContext.pPPPOEHeader )
			{
				pItem->pppoe_offset = (int)(packet_info.tcpipContext.pPPPOEHeader-packet_info.tcpipContext.pMACHeader);
			}
			else
			{
				pItem->pppoe_offset = 0;
			}

			memcpy(pItem->buffer,packet_info.packet->pHeaderinfo,sizeof(PACKET_HEADER));
			memcpy(pItem->buffer+sizeof(PACKET_HEADER),packet_info.tcpipContext.pMACHeader,pItem->trans_offset);
		}
	}
	else
	{

	}

	if( pItem )
	{
		pItem->last_time = packet_info.tsCurPkt.GetSEC();

		BOOL bOK = HandleFragment_Item(pItem,packet_info);

		if( bOK )
		{
			m_ipfrag_finder.FreeBlock(pItem);
		}

		return bOK;
	}
	else
	{
		return false;
	}
	
}

BOOL CIPFragmentComp::HandleFragment_Item(IP_FRAGMENT_ITEM* pItem,PACKET_INFO_EX& packet_info)
{
	const IPv4PacketHead* pIPv4 = (const IPv4PacketHead*)packet_info.tcpipContext.pIPHeader;

	unsigned int begin_offset = (int)((ACE_NTOHS(packet_info.tcpipContext.dwIPFragment) & 0x1FFF)<<3);

	unsigned int next_offset = begin_offset + packet_info.tcpipContext.dwTransPacketLen;

	if( (int)next_offset > MAX_BUFFER_LEN - sizeof(PACKET_HEADER) - pItem->trans_offset )
	{
		return FALSE;
	}

	if( CIPv4FragmentTool::fragment_is_last(packet_info.tcpipContext.dwIPFragment) )
	{
		pItem->fragments[pItem->remain_fragment_nums-1].end = next_offset;
	}

	memcpy(pItem->buffer+sizeof(PACKET_HEADER)+pItem->trans_offset+begin_offset,
		packet_info.tcpipContext.pTransHeader,
		packet_info.tcpipContext.dwTransPacketLen);

	for(int i = 0; i < pItem->remain_fragment_nums; ++i)
	{
		if( begin_offset < pItem->fragments[i].begin )
		{//重复的包

		}
		else if( begin_offset == pItem->fragments[i].begin )
		{
			if( next_offset == pItem->fragments[i].end )
			{//这片来齐了，

				if( i== pItem->remain_fragment_nums-1 )
				{//是最后一片

					--pItem->remain_fragment_nums;

					pItem->total_trans_pkt_len += packet_info.tcpipContext.dwTransPacketLen;

					break;
				}
				else
				{
					pItem->delete_fragment(i);

					pItem->total_trans_pkt_len += packet_info.tcpipContext.dwTransPacketLen;

					break;
				}
			}
			else if( next_offset < pItem->fragments[i].end )
			{
				 pItem->fragments[i].begin = next_offset;

				 pItem->total_trans_pkt_len += packet_info.tcpipContext.dwTransPacketLen;

				 break;
			}
			else
			{
				ACE_ASSERT(FALSE);

				return false;
			}
		}
		else 
		{
			if( next_offset == pItem->fragments[i].end )
			{
				pItem->fragments[i].end = begin_offset;

				pItem->total_trans_pkt_len += packet_info.tcpipContext.dwTransPacketLen;

				break;
			}
			else if( next_offset < pItem->fragments[i].end )
			{//增加一个未到分片

				if( pItem->insert_fragment(i+1,next_offset,pItem->fragments[i].end) )
				{
					pItem->fragments[i].end = begin_offset;

					pItem->total_trans_pkt_len += packet_info.tcpipContext.dwTransPacketLen;

					break;
				}
				else
				{
					return false;
				}				
			}
			else
			{//检查下一个未到的分片

				continue;
			}
		}

	}

	if( pItem->remain_fragment_nums == 0 )
	{
		Time_Stamp cur_data_stamp = packet_info.packet->pHeaderinfo->stamp;

		packet_info.pDynBuffer = pItem->buffer;

		packet_info.packet->pHeaderinfo = (PACKET_HEADER *)packet_info.pDynBuffer;

		packet_info.packet->pHeaderinfo->stamp = cur_data_stamp;

		packet_info.packet->pPacket = packet_info.pDynBuffer+sizeof(PACKET_HEADER);
		packet_info.packet->nCaplen = packet_info.packet->nPktlen = pItem->trans_offset+pItem->total_trans_pkt_len + 4;//4表示mac的crc

		IPv4PacketHead* pIPv4 = (IPv4PacketHead *)(pItem->buffer + sizeof(PACKET_HEADER) + pItem->ip_offset);

		pIPv4->FlgOff = ACE_HTONS(0x4000);//没有分片

		int totel_ip_len = (pItem->trans_offset-pItem->ip_offset)+(int)pItem->total_trans_pkt_len;

		pIPv4->TtlLen = ACE_HTONS((ACE_UINT16)totel_ip_len);

		//重新计算校验和
		pIPv4->ChkSum = 0;
		pIPv4->ChkSum = CPF::ComputeCheckSum_NetOrder( (BYTE *)pIPv4, (int)(pItem->trans_offset-pItem->ip_offset ));

		if( pItem->pppoe_offset )
		{//pppoe的长度字段

			pppoe_header * new_pppoe_header = (pppoe_header *)(pItem->buffer + sizeof(PACKET_HEADER) + pItem->pppoe_offset);

			new_pppoe_header->payload_len = ACE_HTONS(2+totel_ip_len);
		}

		//传输层的校验和不需要重新计算

		/*
		BYTE * pNewTransHeader = (pItem->buffer + sizeof(PACKET_HEADER) + pItem->trans_offset);

		//重新计算校验和
		if( pIPv4->Proto == 6 )
		{//TCP
			TCPPacketHead* pTcpHead = (TCPPacketHead *)pNewTransHeader;

			pTcpHead->ChkSum = 0x0000;
			pTcpHead->ChkSum = ACE_HTONS(CPF::CalculateTCPCheckSum(pIPv4->SourIP,pIPv4->DestIP,(const BYTE *)pNewTransHeader,pItem->total_trans_pkt_len));

		}
		else if( pIPv4->Proto == 17 )
		{//udp
			UDPPacketHead* pUdpHead = (UDPPacketHead *)pNewTransHeader;

			pUdpHead->ChkSum = 0x0000;
			pUdpHead->ChkSum = ACE_HTONS(CPF::CalculateUDPCheckSum(pIPv4->SourIP,pIPv4->DestIP,(const BYTE *)pNewTransHeader,pItem->total_trans_pkt_len));
		}
		else if( pIPv4->Proto == 1 )
		{//icmp
			IcmpHeader * pIcmpHeader = (IcmpHeader *)pNewTransHeader;

			pIcmpHeader->i_cksum = 0;
			pIcmpHeader->i_cksum = ACE_HTONS(CPF::ComputeCheckSum(pNewTransHeader,pItem->total_trans_pkt_len));
		}
		else
		{//do nothing

		}
		*/

		packet_info.tcpipContext.reset();

		CCommonMacTCPIPDecode::MacDecodeToTransport(
			(BYTE *)packet_info.packet->pPacket,packet_info.packet->nPktlen,
			packet_info.tcpipContext,false,false);

		return true;
	}

	return false;
}

void CIPFragmentComp::CheckTimetout(ACE_UINT32 cur_time)
{
	if( m_last_checktime > cur_time )
	{
		m_last_checktime = cur_time;

		return;
	}
	else if( cur_time - m_last_checktime < 5*60 )
	{
		return;
	}
	else
	{
		m_last_checktime = cur_time;
	}

	int index = m_ipfrag_finder.GetHeadBlock();

	while( index != -1 )
	{
		IP_FRAGMENT_ITEM * pItem = m_ipfrag_finder.GetNextBlockFromHead(index);

		if( pItem->last_time > cur_time )
		{
			pItem->last_time = cur_time;
			continue;
		}

		if( cur_time - pItem->last_time > 3*60 )
		{
			if( pItem->buffer )
			{
				CPF::MyMemFree(pItem->buffer);
				pItem->buffer = NULL;
			}

			m_ipfrag_finder.FreeBlock(pItem);
		}		
	}

	return;

}