//////////////////////////////////////////////////////////////////////////
//CommonMacTCPIPDecode.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/addr_tools.h"
#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/CommonPPPOEDecode.h"

//从mac头开始进行解码，得到TCP/UDP的基本解码信息
//如果数据在任何一个层次出现错误，则整个包dwError不为0，整个包的解码结果都无效。
//不是IP包,则bIP=false.而且只有mac头地址和mac长度以及vlan三个字段有效。

//如果在IP包上面，不是tcp或者udp，则pIPHeader,dwIPPacketLen,dwTransPacketLen,pTransHeader有效
//addrTransportConnect中的IP地址有效，addrTransportConnect中的端口无效
//dwTransHeaderLen无效

class CPF_CLASS CCommonMacTCPIPDecode
{
public:
	CCommonMacTCPIPDecode(void);
	~CCommonMacTCPIPDecode(void);

	typedef struct
	{
		unsigned int dwError;

		unsigned int dwProtoOverMac;//在mac层的协议号码

		inline BOOL IsIP() const
		{
			return CCommonEtherDecode::MAC_DECODE_INFO::IsIP(dwProtoOverMac);
		}

		unsigned int vlanId;//-1 表示无效

		// dwIndexProtocl值的定义在CCommonIPv4Decode中

		unsigned int dwIndexProtocl;//在IP上的协议号码

		const BYTE* pMACHeader;//Mac的数据头部
		const BYTE* pIPHeader;//IP层的数据头部
		const BYTE* pTransHeader;//传输层的数据头部

		//MAC地址的头部，是ISL的vlan数据，则pMacAddrHeader!=pMACHeader，否则相等
		const BYTE *pMacAddrHeader;

		const BYTE* pAppHeader;//=pTransHeader+dwTransHeaderLen,只有在数据有传输层的时候有效

		inline const BYTE * GetInnerMac(DIR_IO dir_phy) const
		{
			return (dir_phy==DIR_O2I)?(DST_MAC_ADDR(pMacAddrHeader)):(SRC_MAC_ADDR(pMacAddrHeader));
		}
		inline const BYTE * GetOutterMac(DIR_IO dir_phy) const
		{
			return (dir_phy==DIR_O2I)?(SRC_MAC_ADDR(pMacAddrHeader)):(DST_MAC_ADDR(pMacAddrHeader));
		}

		//判读是否是第一个分片
		inline int IsFirstIPFragment() const
		{
			return CCommonIPv4Decode::IPv4_DECODE_INFO::IsFirstFragment(dwIPFragment);
		}

		unsigned int			dwIPFragment;//标志和offset都在里面，没有进行字节序的转换
		unsigned int			dwIPID;//没有进行字节序的转换
		unsigned int			dwTTL;//ttl

		unsigned int dwMaclen;//包含后面四个字节的校验长度
		unsigned int dwIPPacketLen;

		unsigned int dwTransPacketLen;//传输层的数据包长度
		unsigned int dwTransHeaderLen;//传输层的数据头长度
		unsigned int dwAppPacketLen;//=dwTransPacketLen-dwTransHeaderLen,只有在数据有传输层的时候有效

		IPv4_CONNECT_ADDR conn_addr;

		CCommonPPPOEDecode::PPPOE_STAGE	pppoe_stage;
		const BYTE *	pPPPOEHeader;
		ACE_UINT16		pppoe_session_id;//网络字节序
		unsigned int	pppoe_pkt_len;

		void reset()
		{
			memset(this,0x00,sizeof(*this));

			dwError = 1;

			dwIndexProtocl = -1;

			vlanId = -1;//-1表示没有vlan
			pppoe_session_id = -1;//-1表示没有pppoe
		}

	}TCPIP_DECODE_CONTEXT;

public:
	static void RecalDataPointer(TCPIP_DECODE_CONTEXT& context,
		const BYTE * newdatahead);

public:
	//mac解码到IP层,对于IP层以上的信息（比如端口号，app层的指针和长度等无效）
	static void MacDecodeToIP(const BYTE * pMacheader,unsigned int nMaclen,
		TCPIP_DECODE_CONTEXT& context,
		//bipv4_checksum,是否要进行ipv4的校验和验证
		BOOL bipv4_checksum = false);

	//mac解码到传输层
	static void MacDecodeToTransport(const BYTE * pMacheader,unsigned int nMaclen,
		TCPIP_DECODE_CONTEXT& context,
		//bipv4_checksum,btrans_checksum是否要进行ipv4,tcp/udp的校验和验证
		BOOL bipv4_checksum = false,BOOL btrans_checksum=false);


public:
	//mac解码到IP层.
	//如果bIpv4Decode返回0,表示没有解码到ipv4，则ipv4_decode_info,pIpv4Header,ipv4_pkt_len无效）
	//如果返回值为0，表示没有出现错。
	static DWORD MacDecodeToIP(const BYTE * pMacheader,unsigned int nMaclen,
		OUT CCommonEtherDecode::MAC_DECODE_INFO& mac_decode_info,
		OUT CCommonIPv4Decode::IPv4_DECODE_INFO& ipv4_decode_info,
		OUT CCommonPPPOEDecode::PPPOE_STAGE&	pppoe_stage,
		OUT const BYTE *& pPPPOEHeader,
		OUT ACE_UINT16& pppoe_session_id,
		OUT unsigned int& pppoe_pkt_len,
		OUT unsigned int& dwProtoOverMac,//mac层的协议号
		OUT const BYTE *& pIpv4Header,
		OUT unsigned int& ipv4_pkt_len,
		//bipv4_checksum,是否要进行ipv4的校验和验证
		BOOL bipv4_checksum = false);

	static void ReculateCheckSum(
		IN OUT CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext);

};


