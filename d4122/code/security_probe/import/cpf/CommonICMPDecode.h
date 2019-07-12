//////////////////////////////////////////////////////////////////////////
//CommonICMPDecode.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CCommonICMPDecode
{
public:
	CCommonICMPDecode(void);
	~CCommonICMPDecode(void);

public:
#pragma pack(push)
#pragma pack(1)
	// ICMP header
	typedef struct _ihdr {
		BYTE	i_type;
		BYTE	i_code; /* type sub code */
		ACE_UINT16	i_cksum;
		ACE_UINT16	i_id;
		ACE_UINT16	i_seq;

	}IcmpHeader;
#pragma pack(pop)



#pragma pack(push)
#pragma pack(1)
	// ICMP header
	typedef struct {
		IcmpHeader	icmp_header;

		int		nError;//0表示正确，1表示包太短，2表示校验和不正确,如果错误，其他的字段没有意义
		char *	i_data;//指向数据包的位置
		int		data_len;

	}ICMP_DECODE_INFO;

#pragma pack(pop)

public:
	static void  ICMPProtocolDecode(const BYTE * pPacket, DWORD dwPacketLen, ICMP_DECODE_INFO& icmp_info);
	static void  ICMPProtocolDecode_Checksum(const BYTE * pPacket, DWORD dwPacketLen, ICMP_DECODE_INFO& icmp_info);

};


