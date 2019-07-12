//////////////////////////////////////////////////////////////////////////
//CommonNBTDecode.h

//rfc1001,rfc1002

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CCommonNBTDecode
{
public:
	//rfc1001,rfc1002
	typedef struct
	{
		WORD m_NAME_TRN_ID;
		BYTE m_rsp;//0表示qurey，1表示rsp
		BYTE m_OPCODE;//0 = query，5 = registration，6 = release，7 = WACK，8 = refresh
		BYTE m_NM_FLAGS;
		BYTE m_RCODE;
		WORD m_QDCOUNT;
		WORD m_ANCOUNT;
		WORD m_NSCOUNT;
		WORD m_ARCOUNT;

	}NetBios_ONUDP_HEADER;

	typedef struct
	{
		char m_encodeName[256];
		WORD m_QuestionType;
		WORD m_QuestionClass;

		WORD m_RRType;
		WORD m_RRClass;
		DWORD m_RRTTL;
		WORD  m_RDLength;
		WORD  m_RNodeFlags;
		DWORD m_RNodeAddress;//net_order_ip	

		int		m_nodeType;//0=B node,1= P node,2 = M node

		char  m_decodeName[16];
		char  m_RRName[16];	

		BYTE m_nNameType;

		int	  m_nGroupOrUnique;//0=unique,1=group	
		int	  m_nComputerOrDomainName;//0=domain,1=computer

	}NetBios_ONUDP_RegisterInfo;

	typedef NetBios_ONUDP_RegisterInfo	NetBios_ONUDP_RefreshInfo;

	typedef NetBios_ONUDP_RegisterInfo	NetBios_ONUDP_ReleaseInfo;

	typedef struct
	{
		WORD m_RRType;
		WORD m_RRClass;
		DWORD m_RRTTL;
		WORD  m_RDLength;

		char  m_RRName[16];	

		BYTE m_nNameType;

		std::vector< std::pair<ACE_UINT16,ACE_UINT32> >	m_vtIPAddr;

	}NetBios_ONUDP_QueryResopnse_Info;


public:
	CCommonNBTDecode(void);
	~CCommonNBTDecode(void);

public:
	//pPacket是nbt的数据头指针， dwPacketLen是输入数据的长度。
	//作为输出，是跳过头部后的数据指针，dwPacketLen是去掉头部后的数据长度。
	static BOOL  NBOnUDDecode_Header(const BYTE *& pPacket, DWORD& dwPacketLen, 
		NetBios_ONUDP_HEADER& header_info);

	static BOOL  NBOnUDPDecode_Register_Req(const BYTE * pNbtData,
		const BYTE * pRegisterInfoPacket,
		DWORD dwegisterInfoPacketLen,
		const NetBios_ONUDP_HEADER& header_info,
		NetBios_ONUDP_RegisterInfo& register_info);

	static BOOL NBOnUDPDecode_Refresh_Req(const BYTE * pNbtData,
		const BYTE * pRegisterInfoPacket,
		DWORD dwegisterInfoPacketLen,
		const NetBios_ONUDP_HEADER& header_info,
		NetBios_ONUDP_RefreshInfo& refresh_info);

	static BOOL NBOnUDPDecode_Release_Req(const BYTE * pNbtData,
		const BYTE * pRegisterInfoPacket,
		DWORD dwegisterInfoPacketLen,
		const NetBios_ONUDP_HEADER& header_info,
		NetBios_ONUDP_ReleaseInfo& release_info);

	static BOOL NBOnUDPDecode_Qurey_Positive_Response(const BYTE * pNbtData,
		const BYTE * pRegisterInfoPacket,
		DWORD dwegisterInfoPacketLen,
		const NetBios_ONUDP_HEADER& header_info,
		NetBios_ONUDP_QueryResopnse_Info& QueryResponseInfo);

public:
	//返回名字占用的长度。-1表示失败
	static int TransferEncodeNameIntoNbName(const BYTE * pNbtData,
		const BYTE * pEncodeName, char * nbbuffer, 
		int nbName_buf_len, BYTE& type);

	static BOOL IsComputerOrDomainName(int GroupOrUnique,BYTE type,
		int& nComputerOrDomainName);

};

