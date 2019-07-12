//////////////////////////////////////////////////////////////////////////
//CommonEtherDecode.h

#pragma once

//	==========================
//	=========MAC֡��ʽ========
//	==========================
//	
//	======IEEE 802.2/802.3��ʽ (RFC 1042) 
//	
//	|<---------802.3 MAC---------->|<--803.3 LLC->|<-802.2 SNAP->|
//	+------------+------------+----+----+----+----+---------+----+----------------------+-------+
//	|  Ŀ�ĵ�ַ  |    Դ��ַ  |����|DSAP|SSAP|CNTL| ORG code|����|       ��     ��      |  CRC  |
//	+------------+------------+----+----+----+----+---------+----+----------------------+-------+
//	6             6       2     1    1    1     3       2         38-1492             4
//	
//	======��̫���ķ�װ��ʽ(RFC 894)
//	
//	|<---------------------46-1500�ֽ�------------------>|
//	+------------+------------+----+----+----+----+---------+----+----------------------+-------+
//	|  Ŀ�ĵ�ַ  |    Դ��ַ  |����|					��     ��						|  CRC  |
//	+------------+------------+----+----+----+----+---------+----+----------------------+-------+
//	6             6		2												            4

#include "cpf/cpf.h"
#include "cpf/addr_tools.h"

class CPF_CLASS CCommonEtherDecode
{
public:
	CCommonEtherDecode(void);
	~CCommonEtherDecode(void);


	//��Э���ַ�ĳ���ֵ
	enum{
		MAC_ADDRESS_LEN = 6,
		IP_ADDRESS_LEN = 4,

		MAC_SDADDRESS_LEN = MAC_ADDRESS_LEN*2,
		IP_SDADDRESS_LEN = IP_ADDRESS_LEN*2,

		MAC_ADDRESS_LEN_EX = 8,
		MAC_SDADDRESS_LEN_EX = MAC_ADDRESS_LEN_EX*2,

	};

	typedef unsigned char			 MACADDR[MAC_ADDRESS_LEN];
	typedef unsigned char			 DSMACADDR[MAC_SDADDRESS_LEN];//Ŀ��mac��ַ��ǰ��Դmac��ַ�ں�
	typedef unsigned char			 MACADDREX[MAC_ADDRESS_LEN_EX];
	typedef unsigned char			 DSMACADDREX[MAC_SDADDRESS_LEN_EX];//Ŀ��mac��ַ��ǰ��Դmac��ַ�ں�
	
	//�����ֽ������̫���ϲ�Э������
	enum _BIG_END_ETHERNET_TYPE
	{
		PID_ETHERTYPE_IP              = 0x0800,
		PID_ETHERTYPE_ARP             = 0x0806,
		PID_ETHERTYPE_XNS             = 0x0600,// IDP
		PID_ETHERTYPE_XNS1            = 0x0807,// compability
		PID_ETHERTYPE_X25             = 0x0805,
		PID_ETHERTYPE_BANYAN_VINES1   = 0x0BAD,//VIP
		PID_ETHERTYPE_BANYAN_VINES2   = 0x80C4,//VIP
		PID_ETHERTYPE_BANYAN_ECHO1    = 0x0BAF,
		PID_ETHERTYPE_BANYAN_ECHO2    = 0x80C5,
		PID_ETHERTYPE_DNA             = 0x6003,
		PID_ETHERTYPE_RARP            = 0x8035,
		PID_ETHERTYPE_APPLETALK       = 0x809B,
		PID_ETHERTYPE_APPLETALK_ARP   = 0x80F3,
		PID_ETHERTYPE_SNA             = 0x80D5,
		PID_ETHERTYPE_IPX             = 0x8137,
		PID_ETHERTYPE_IPX1            = 0x8037,
		PID_ETHERTYPE_PPPOEE_Session			  = 0x8864,
		PID_ETHERTYPE_PPPOE_Discovery			  = 0x8863,

		PID_ETHERTYPE_LLC             = 0x0000,//special here
	};

	//LLC SAP define
	enum _LLC_SAP
	{
		LLC_SAP_SNAP          = 0xAA,
		LLC_SAP_IPXSPX        = 0xE0,
		LLC_SAP_TCPIP         = 0x06,
		LLC_SAP_NETBIOS       = 0xF0, //IBM/Microsofr NETBEUI
		LLC_SAP_SNA           = 0x04,
		LLC_SAP_BPDU          = 0x42,
		LLC_SAP_USER          = 0x80,
		LLC_SAP_LAN           = 0xF4,
		LLC_SAP_VINES         = 0xBC,
	};

	enum _SPECIAL_DSAP_FOR_IPX
	{
		PID_ETHERTYPE_RAWIPX    = 0xFFFF // special DSAP SSAP for IPX
	};

	//MAC�ϵ�Э���װ���͵Ķ���
	typedef enum
	{
		ETHERNET_8023 = 0x01,
		ETHERNET_RAW = ETHERNET_8023,//Equal to 
		ETHERNET_II	= 0x02,		// ethernet_802.3
		ETHERNET_8022 = 0x04,	// ethernet_802.3_802.2
		ETHERNET_SNAP = 0x08,	// ethernet_802.3_802.2_snap
	}MAC_CAPSULATION;

	//��MAC����������ݵĶ��� 
	enum{
		NUM_SCALE_MAC_LENGTH = 8,//��MAC�������ݰ����ȷֲ�����ĳ���
		//64�ֽ����£�65~127��128~255��256~511��512~1023��1024����
	};

	//��MAC��Э��ֲ�������±�
	enum{
		INDEX_PID_MAC_OTHER			= 0,
		INDEX_PID_MAC_IP			= 1,
		INDEX_PID_MAC_IPX			= 2,
		INDEX_PID_MAC_ARP			= 3, 
		INDEX_PID_MAC_NETBIOS		= 4,
		INDEX_PID_MAC_BPDU			= 5,
		INDEX_PID_MAC_APPLETALK		= 6,
		INDEX_PID_MAC_APPLETALKARP	= 7,
		INDEX_PID_MAC_BANYAN		= 8,
		INDEX_PID_MAC_BANYANECHO	= 9,
		INDEX_PID_MAC_XNS			= 10,
		INDEX_PID_MAC_SNA			= 11,
		INDEX_PID_MAC_DNA			= 12,
		INDEX_PID_MAC_X25			= 13,
		INDEX_PID_MAC_RARP			= 14,
		INDEX_PID_MAC_PPPOE			= 15,
	};

	//
	//MAC������Ϣ
	//
	typedef enum
	{
		MACPACKSTYLE_BROADCAST = 1,
		MACPACKSTYLE_GROUPCAST = 2,
		MACPACKSTYLE_OTHER = 3,
	}MACPACKSTYLE;

	typedef enum
	{
		MACERRSTYLE_NOERROR = 0,

		MACERRSTYLE_OTHER_ERROR = 1,
		MACERRSTYLE_SHORT_HEADER,

		MACERRSTYLE_CRC,
		MACERRSTYLE_DRIBBLE,
		MACERRSTYLE_RUNT,
		MACERRSTYLE_FRAGMENT,
		MACERRSTYLE_OVERSIZE,
		MACERRSTYLE_UNDERSIZE,
		MACERRSTYLE_JABBER,
		MACERRSTYLE_ALIGNMENT,
		MACERRSTYLE_LOCALCOLLISION,
		MACERRSTYLE_REMOTECOLLISION,
	}MACERRORSTYLE;

	typedef enum
	{
		VLAN_TYPE_NONE = 0,
		VLAN_TYPE_8021Q = 1,
		VLAN_TYPE_ISL = 2

	}VLAN_TYPE;

	///////////////////////////////////////////////////////////////////////////////

	typedef struct
	{
		VLAN_TYPE				vlan_type;
		unsigned int			vlan_id;//-1��ʾ��Ч

		unsigned int			dwPackLen;//MAC����ĳ���
		int						nProtoIndex;//�ϲ��Э���

		unsigned int			dwHeaderLength; //������Э���Э��ͷ������(��MAC���Է���MAC֡��ͷ������)

		unsigned char *			pMacAddrHeader;//mac��ַ��ͷ��

		unsigned char *			pUpperData;
		unsigned int			dwUpperProtoLength;  //������Э����ϲ�Э������ݳ���(��MAC���Է���IPv4�ĳ���)

		MAC_CAPSULATION			dwCapsulation;  //������Э��ķ�װ����
		MACPACKSTYLE			nPackStyle;
		MACERRORSTYLE			nErrorStyle;

		void reset()
		{
			vlan_type = VLAN_TYPE_NONE;
			vlan_id = -1;

			dwPackLen = 0;
			nProtoIndex = -1;

			dwHeaderLength = 0;

			pMacAddrHeader = NULL;
			pUpperData = NULL;
			dwUpperProtoLength = 0;

			dwCapsulation = ETHERNET_8023;
			nPackStyle = MACPACKSTYLE_OTHER;
			nErrorStyle = MACERRSTYLE_OTHER_ERROR;
		}

		static inline BOOL IsIP(unsigned int nProtoIndex)
		{
			return nProtoIndex == CCommonEtherDecode::INDEX_PID_MAC_IP;
		}

		inline BOOL IsIP() const
		{
			return IsIP(nProtoIndex);
		}

		static inline BOOL IsARP(unsigned int nProtoIndex)
		{
			return nProtoIndex == CCommonEtherDecode::INDEX_PID_MAC_ARP;
		}

		inline BOOL IsARP() const
		{
			return IsARP(nProtoIndex);
		}

		static inline BOOL IsPPPOE(unsigned int nProtoIndex)
		{
			return (nProtoIndex == CCommonEtherDecode::INDEX_PID_MAC_PPPOE);
		}

		inline BOOL IsPPPOE() const
		{
			return IsPPPOE(nProtoIndex);
		}

	}MAC_DECODE_INFO;

	typedef struct
	{
		unsigned int	frType;
		unsigned int	hardType;
		unsigned int	protoType;
		unsigned int	hardAddrLen;
		unsigned int	protoAddrLen;
		unsigned int	operation;

		MACADDREX		senderMacAddr;
		ACE_UINT32		senderNetOrderIPAddr;

		MACADDREX		destMacAddr;
		ACE_UINT32		destNetOrderIPAddr;

		unsigned int	nErrorStyle;

		void reset()
		{
			memset(this,0x00,sizeof(*this));
		}

	}ARP_DECODE_INFO;

	/////////////////////////////////////////////////////////////////////////////////

	//��MAC���н���, ���ص���Ϣ�����info��, �û���һ�����ݸ���Ϣ����ͳ��
	//�ر�ע�����: �����󷵻�Ϊ MACERRSTYLE_SHORT_HEADER ʱ, ֻ�б���, �����ֲ��ʹ�����Ϣ
	//ͬʱ���ܶ�HOST �� MATRIX �����ͳ��
	static void MACProtocolDecode(const BYTE* pPacket, DWORD dwPacketLen, MAC_DECODE_INFO& macInfo);
	
	static void ARPProtocolDecode(const BYTE* pPacket, DWORD dwPacketLen, ARP_DECODE_INFO& arpInfo);

private:
	static unsigned int GetSNAPOrEthernetIndex(unsigned int protoid)
	{
		unsigned int nprotoindex = INDEX_PID_MAC_OTHER;

		switch(protoid)
		{
		case PID_ETHERTYPE_IP:
			nprotoindex = INDEX_PID_MAC_IP;
			break;
		case PID_ETHERTYPE_PPPOEE_Session:
		case PID_ETHERTYPE_PPPOE_Discovery:
			nprotoindex = INDEX_PID_MAC_PPPOE;
			break;

		case PID_ETHERTYPE_IPX:
			nprotoindex = INDEX_PID_MAC_IPX;
			break;
		case PID_ETHERTYPE_IPX1:
			nprotoindex = INDEX_PID_MAC_IPX;
			break;
		case PID_ETHERTYPE_ARP:
			nprotoindex = INDEX_PID_MAC_ARP;
			break;
		case PID_ETHERTYPE_RARP:
			nprotoindex = INDEX_PID_MAC_RARP;
			break;
		case PID_ETHERTYPE_X25:
			nprotoindex = INDEX_PID_MAC_X25;
			break;
		case PID_ETHERTYPE_APPLETALK_ARP:
			nprotoindex = INDEX_PID_MAC_APPLETALKARP;
			break;
		default:
			break;
		}

		return nprotoindex;
	}


	static unsigned int Get8022Index(unsigned int protoid)
	{
		unsigned int nprotoindex = INDEX_PID_MAC_OTHER;

		switch(protoid)
		{
		case LLC_SAP_TCPIP:
			nprotoindex = INDEX_PID_MAC_IP;
			break;
		case LLC_SAP_IPXSPX:
			nprotoindex = INDEX_PID_MAC_IPX;
			break;
		case LLC_SAP_NETBIOS:
			nprotoindex = INDEX_PID_MAC_NETBIOS;
			break;
		default:
			break;
		}

		return nprotoindex;
	}

};

