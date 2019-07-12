//////////////////////////////////////////////////////////////////////////
//CommonMacTCPIPDecode.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/addr_tools.h"
#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/CommonPPPOEDecode.h"

//��macͷ��ʼ���н��룬�õ�TCP/UDP�Ļ���������Ϣ
//����������κ�һ����γ��ִ�����������dwError��Ϊ0���������Ľ���������Ч��
//����IP��,��bIP=false.����ֻ��macͷ��ַ��mac�����Լ�vlan�����ֶ���Ч��

//�����IP�����棬����tcp����udp����pIPHeader,dwIPPacketLen,dwTransPacketLen,pTransHeader��Ч
//addrTransportConnect�е�IP��ַ��Ч��addrTransportConnect�еĶ˿���Ч
//dwTransHeaderLen��Ч

class CPF_CLASS CCommonMacTCPIPDecode
{
public:
	CCommonMacTCPIPDecode(void);
	~CCommonMacTCPIPDecode(void);

	typedef struct
	{
		unsigned int dwError;

		unsigned int dwProtoOverMac;//��mac���Э�����

		inline BOOL IsIP() const
		{
			return CCommonEtherDecode::MAC_DECODE_INFO::IsIP(dwProtoOverMac);
		}

		unsigned int vlanId;//-1 ��ʾ��Ч

		// dwIndexProtoclֵ�Ķ�����CCommonIPv4Decode��

		unsigned int dwIndexProtocl;//��IP�ϵ�Э�����

		const BYTE* pMACHeader;//Mac������ͷ��
		const BYTE* pIPHeader;//IP�������ͷ��
		const BYTE* pTransHeader;//����������ͷ��

		//MAC��ַ��ͷ������ISL��vlan���ݣ���pMacAddrHeader!=pMACHeader���������
		const BYTE *pMacAddrHeader;

		const BYTE* pAppHeader;//=pTransHeader+dwTransHeaderLen,ֻ���������д�����ʱ����Ч

		inline const BYTE * GetInnerMac(DIR_IO dir_phy) const
		{
			return (dir_phy==DIR_O2I)?(DST_MAC_ADDR(pMacAddrHeader)):(SRC_MAC_ADDR(pMacAddrHeader));
		}
		inline const BYTE * GetOutterMac(DIR_IO dir_phy) const
		{
			return (dir_phy==DIR_O2I)?(SRC_MAC_ADDR(pMacAddrHeader)):(DST_MAC_ADDR(pMacAddrHeader));
		}

		//�ж��Ƿ��ǵ�һ����Ƭ
		inline int IsFirstIPFragment() const
		{
			return CCommonIPv4Decode::IPv4_DECODE_INFO::IsFirstFragment(dwIPFragment);
		}

		unsigned int			dwIPFragment;//��־��offset�������棬û�н����ֽ����ת��
		unsigned int			dwIPID;//û�н����ֽ����ת��
		unsigned int			dwTTL;//ttl

		unsigned int dwMaclen;//���������ĸ��ֽڵ�У�鳤��
		unsigned int dwIPPacketLen;

		unsigned int dwTransPacketLen;//���������ݰ�����
		unsigned int dwTransHeaderLen;//����������ͷ����
		unsigned int dwAppPacketLen;//=dwTransPacketLen-dwTransHeaderLen,ֻ���������д�����ʱ����Ч

		IPv4_CONNECT_ADDR conn_addr;

		CCommonPPPOEDecode::PPPOE_STAGE	pppoe_stage;
		const BYTE *	pPPPOEHeader;
		ACE_UINT16		pppoe_session_id;//�����ֽ���
		unsigned int	pppoe_pkt_len;

		void reset()
		{
			memset(this,0x00,sizeof(*this));

			dwError = 1;

			dwIndexProtocl = -1;

			vlanId = -1;//-1��ʾû��vlan
			pppoe_session_id = -1;//-1��ʾû��pppoe
		}

	}TCPIP_DECODE_CONTEXT;

public:
	static void RecalDataPointer(TCPIP_DECODE_CONTEXT& context,
		const BYTE * newdatahead);

public:
	//mac���뵽IP��,����IP�����ϵ���Ϣ������˿ںţ�app���ָ��ͳ��ȵ���Ч��
	static void MacDecodeToIP(const BYTE * pMacheader,unsigned int nMaclen,
		TCPIP_DECODE_CONTEXT& context,
		//bipv4_checksum,�Ƿ�Ҫ����ipv4��У�����֤
		BOOL bipv4_checksum = false);

	//mac���뵽�����
	static void MacDecodeToTransport(const BYTE * pMacheader,unsigned int nMaclen,
		TCPIP_DECODE_CONTEXT& context,
		//bipv4_checksum,btrans_checksum�Ƿ�Ҫ����ipv4,tcp/udp��У�����֤
		BOOL bipv4_checksum = false,BOOL btrans_checksum=false);


public:
	//mac���뵽IP��.
	//���bIpv4Decode����0,��ʾû�н��뵽ipv4����ipv4_decode_info,pIpv4Header,ipv4_pkt_len��Ч��
	//�������ֵΪ0����ʾû�г��ִ�
	static DWORD MacDecodeToIP(const BYTE * pMacheader,unsigned int nMaclen,
		OUT CCommonEtherDecode::MAC_DECODE_INFO& mac_decode_info,
		OUT CCommonIPv4Decode::IPv4_DECODE_INFO& ipv4_decode_info,
		OUT CCommonPPPOEDecode::PPPOE_STAGE&	pppoe_stage,
		OUT const BYTE *& pPPPOEHeader,
		OUT ACE_UINT16& pppoe_session_id,
		OUT unsigned int& pppoe_pkt_len,
		OUT unsigned int& dwProtoOverMac,//mac���Э���
		OUT const BYTE *& pIpv4Header,
		OUT unsigned int& ipv4_pkt_len,
		//bipv4_checksum,�Ƿ�Ҫ����ipv4��У�����֤
		BOOL bipv4_checksum = false);

	static void ReculateCheckSum(
		IN OUT CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext);

};


