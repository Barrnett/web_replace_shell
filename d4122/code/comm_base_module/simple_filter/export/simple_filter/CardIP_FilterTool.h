//////////////////////////////////////////////////////////////////////////
//CardIP_FilterTool.h

#pragma once

#include "simple_filter/simple_filter.h"
#include "simple_filter/CardIP_FltCondition.h"
#include "cpf/CommonMacTCPIPDecode.h"

class SIMPLE_FILTER_CLASS CCardIP_FilterTool
{
public:
	static bool filter_packet(CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		const CCardIP_FltCondition& flt_condition,
		BYTE cardno,BYTE ifno);

	static bool filter_packet(const char * pPacket,int packetlen,
		const CCardIP_FltCondition& flt_condition,
		BYTE cardno,BYTE ifno);

};
