//////////////////////////////////////////////////////////////////////////
//ConfigFacade.cpp

#include "stdafx.h"
#include "ConfigFacade.h"
#include "cpf/path_tools.h"
#include "PacketIO/PacketIO.h"
#include "PacketIO/WriteDataStream.h"
#include "IPFragmentComp.h"

CConfigFacade * g_pCConfigFacade = NULL;

void CConfigFacade::Initialize()
{
	ACE_ASSERT(!g_pCConfigFacade);

	if( !g_pCConfigFacade )
	{
		g_pCConfigFacade = new CConfigFacade;

		g_pCConfigFacade->init();

	}

	return;
}

void CConfigFacade::UnInitialize()
{
	if( g_pCConfigFacade )
	{
		g_pCConfigFacade->close();
		delete g_pCConfigFacade;
		g_pCConfigFacade = NULL;
	}

	return;
}

CConfigFacade::CConfigFacade(void)
{
}

CConfigFacade::~CConfigFacade(void)
{
}

void CConfigFacade::close()
{
	if( m_pIPFragmentComp )
	{
		m_pIPFragmentComp->close();
		delete m_pIPFragmentComp;
		m_pIPFragmentComp = NULL;
	}

	return;
}


void CConfigFacade::init()
{
	m_pIPFragmentComp = new CIPFragmentComp;

	m_pIPFragmentComp->init(1000);

	return;
}



int CConfigFacade::SplitTCPPacket_By_MacMaxFrame(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext,
												 IWriteDataStream * pWriteDataStream,int output_card_index,int MAX_PKT_LEN)
{
	int npkt = m_tcp_fragment_tool.DoFragment_By_MacMaxFrame(
		tcpipContext.pMACHeader,
		tcpipContext.pIPHeader,
		tcpipContext.pTransHeader,
		tcpipContext.pAppHeader,
		tcpipContext.dwMaclen-4,
		MAX_PKT_LEN);

	for(int i = 0; i < npkt; ++i)
	{
		const BYTE * fragment_data = NULL;

		int fragment_len = m_tcp_fragment_tool.GetFragment(i,fragment_data);

		pWriteDataStream->WritePacket(PACKET_OK, (const char*)fragment_data, fragment_len+4,(1<<output_card_index),1);
	}

	return npkt;
}

int CConfigFacade::SplitTCPPacket_By_FragmentNum(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext,
														IWriteDataStream * pWriteDataStream,int output_card_index,
														int http_fragment_num,int http_re_order,
														int do_ip_fragment,int ip_framgent_num,int ip_re_order)
{
	int npkt = m_tcp_fragment_tool.DoFragment_By_FragmentNum(
		tcpipContext.pMACHeader,
		tcpipContext.pIPHeader,
		tcpipContext.pTransHeader,
		tcpipContext.pAppHeader,
		tcpipContext.dwMaclen-4,
		http_fragment_num);

	for(int i = 0; i < npkt; ++i)
	{
		int index_tcp = i;

		if( http_re_order )
		{
			index_tcp = (npkt-i-1);
		}

		const BYTE * fragment_data = NULL;

		int fragment_len = m_tcp_fragment_tool.GetFragment(index_tcp,fragment_data);

		if( do_ip_fragment && ip_framgent_num >= 2 )
		{
			int ip_out_fragment_nums = m_ip_frament_tool.DoFragment_By_FragmentNum(fragment_data,
				fragment_data + (tcpipContext.pIPHeader-tcpipContext.pMACHeader),				
				fragment_len- (int)(tcpipContext.pIPHeader-tcpipContext.pMACHeader),
				ip_framgent_num);

			const BYTE * ip_data = NULL;

			for(int j = 0; j < ip_out_fragment_nums; ++j )
			{
				int index_ip = j;

				if( ip_re_order )
				{
					index_ip = (ip_out_fragment_nums-j-1);
				}

				int len = m_ip_frament_tool.GetFragment(index_ip,ip_data);

				pWriteDataStream->WritePacket(PACKET_OK, (const char*)ip_data, len+4,(1<<output_card_index),1);
			}			
		}
		else
		{
			pWriteDataStream->WritePacket(PACKET_OK, (const char*)fragment_data, fragment_len+4,(1<<output_card_index),1);
		}
	}

	return npkt;
}

int CConfigFacade::SplitIPPacket_By_MaxIPLen(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext,
													IWriteDataStream * pWriteDataStream,int output_card_index,int max_ip_pkt_len)
{
	const BYTE* pdata = tcpipContext.pMACHeader;

	int count = m_ip_frament_tool.DoFragment_By_MaxIPLen(pdata, tcpipContext.pIPHeader,tcpipContext.dwIPPacketLen,max_ip_pkt_len);

	for(int i = 0; i < count; ++i)
	{
		const BYTE * fragment_data = NULL;
		int fragment_len = m_ip_frament_tool.GetFragment(i,fragment_data);

		pWriteDataStream->WritePacket(PACKET_OK, (const char*)fragment_data, fragment_len+4,(1<<output_card_index),1);
	}

	return count;
}

int CConfigFacade::SplitIPPacket_By_FragmentNum(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext,
													   IWriteDataStream * pWriteDataStream,int output_card_index,int fragment_num,int ip_re_order)
{
	const BYTE* pdata = tcpipContext.pMACHeader;

	int count = m_ip_frament_tool.DoFragment_By_FragmentNum(pdata, tcpipContext.pIPHeader,tcpipContext.dwIPPacketLen,fragment_num);

	for(int i = 0; i < count; ++i)
	{
		int index_ip = i;

		if( ip_re_order )
		{
			index_ip = (count-i-1);
		}

		const BYTE * fragment_data = NULL;
		int fragment_len = m_ip_frament_tool.GetFragment(index_ip,fragment_data);

		pWriteDataStream->WritePacket(PACKET_OK, (const char*)fragment_data, fragment_len+4,(1<<output_card_index),1);
	}

	return count;
}


BOOL CConfigFacade::HandleFragment(PACKET_INFO_EX& packet_info)
{
	if( m_pIPFragmentComp )
	{
		return m_pIPFragmentComp->HandleFragment(packet_info);
	}

	return false;
}

void CConfigFacade::CheckIPFragmentTimetout(ACE_UINT32 cur_time)
{
	m_pIPFragmentComp->CheckTimetout(cur_time);
}
