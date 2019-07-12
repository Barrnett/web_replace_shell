//////////////////////////////////////////////////////////////////////////
//ConfigFacade.h

#pragma once

#include "flux_control_base_export.h"
#include "cpf/IPv4FragmentTool.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "tcpip_mgr_common_init.h"

class IWriteDataStream;
class CIPFragmentComp;


class FLUX_CONTROL_BASE_CLASS CConfigFacade
{
public:
	static void Initialize();
	static void UnInitialize();

public:
	CConfigFacade(void);
	~CConfigFacade(void);

private:
	void init();
	void close();

public:
	int CConfigFacade::SplitTCPPacket_By_MacMaxFrame(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext,
		IWriteDataStream * pWriteDataStream,int output_card_index,int MAX_PKT_LEN);

	int CConfigFacade::SplitTCPPacket_By_FragmentNum(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext,
		IWriteDataStream * pWriteDataStream,int output_card_index,
		int http_fragment_num,int http_re_order,
		int do_ip_fragment,int ip_framgent_num,int ip_re_order);

	int SplitIPPacket_By_MaxIPLen(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext,
		IWriteDataStream * pWriteDataStream,int output_card_index,int max_ip_pkt_len);

	int SplitIPPacket_By_FragmentNum(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext,
		IWriteDataStream * pWriteDataStream,int output_card_index,int fragment_num,int ip_re_order);


private:
	CIPv4FragmentTool		m_ip_frament_tool;
	CTCPFragmentTool		m_tcp_fragment_tool;

private:
	CIPFragmentComp *		m_pIPFragmentComp;

public:
	BOOL HandleFragment(PACKET_INFO_EX& packet_info);

	void CheckIPFragmentTimetout(ACE_UINT32 cur_time);
};

 extern FLUX_CONTROL_BASE_CLASS CConfigFacade * g_pCConfigFacade;
