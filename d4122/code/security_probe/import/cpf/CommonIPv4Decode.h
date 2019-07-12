//////////////////////////////////////////////////////////////////////////
//CommonIPv4Decode.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/addr_tools.h"

class CPF_CLASS CCommonIPv4Decode
{
public:
	CCommonIPv4Decode();
	~CCommonIPv4Decode();

public:
	typedef enum
	{
		IPv4ERRSTYLE_NOERROR = 0,
		IPv4ERRSTYLE_OTHER_ERROR = 1,
		IPv4ERRSTYLE_SHORT_HEADER = 2,
		IPv4ERRSTYLE_CHECKSUM = 3,
		IPv4ERRSTYLE_VERSION = 4,
		IPv4ERRSTYLE_PACKETSHORT= 5,

	}IPv4ERRORSTYLE;

	typedef enum
	{
		IPv4PACKSTYLE_BROADCAST = 1,
		IPv4PACKSTYLE_GROUPCAST = 2,
		IPv4PACKSTYLE_OTHER = 3,
	}IPv4PACKSTYLE;


	typedef struct
	{
		unsigned int			dwPackLen;//IP本身的长度
		int						nProtoIndex;//上层的协议号

		unsigned int			dwHeaderLength; //被解码协议的协议头部长度(IPv4头部长度)

		unsigned char *			pUpperData;
		unsigned int			dwUpperProtoLength;  //被解码协议的上层协议的数据长度

		ACE_UINT32				ipSrc;//网络字节序
		ACE_UINT32				ipDst;//网络字节序


		unsigned int			dwFragment;//标志和offset都在里面，没有进行字节序的转换
		unsigned int			dwID;//没有进行字节序的转换
		unsigned int			dwTTL;

		IPv4PACKSTYLE			nPackStyle;
		IPv4ERRORSTYLE			nErrorStyle;

		void reset()
		{
			dwPackLen = 0;
			nProtoIndex = -1;

			dwHeaderLength = 0;

			pUpperData = NULL;
			dwUpperProtoLength = 0;

			ipSrc = ipDst = 0;

			dwFragment = 0;

			dwID = dwTTL = 0;

			nPackStyle = IPv4PACKSTYLE_OTHER;
			nErrorStyle = IPv4ERRSTYLE_OTHER_ERROR;
		}

		//判读是否是第一个分片
		static inline int IsFirstFragment(unsigned int tem_dwFragment)
		{
			return DF(tem_dwFragment)||(Fragment_Offset(tem_dwFragment)==0);
		}

		//分片偏移
		static inline unsigned int Fragment_Offset(unsigned int tem_dwFragment)
		{
			ACE_UINT16 wFragment = ACE_UINT16(tem_dwFragment&0XFF1F);

			return ACE_NTOHS(wFragment);
		}

		//是否禁止分片
		static inline unsigned int DF(unsigned int tem_dwFragment)
		{
			return (tem_dwFragment&0040);
		}

	}IPv4_DECODE_INFO;

	//IP协议的上层协议号码定义
	enum{
		INDEX_PID_IPv4_OTHER=0,
		INDEX_PID_IPv4_ICMP,
		INDEX_PID_IPv4_IPIGMP,
		INDEX_PID_IPv4_IPGGP,
		INDEX_PID_IPv4_IPSTREAM,
		INDEX_PID_IPv4_TCP,
		INDEX_PID_IPv4_IPEGP,
		INDEX_PID_IPv4_IGP,
		INDEX_PID_IPv4_NVPII,
		INDEX_PID_IPv4_PUP,
		INDEX_PID_IPv4_CHAOS,	
		INDEX_PID_IPv4_UDP,	
		INDEX_PID_IPv4_SEP,
		INDEX_PID_IPv4_DDP,
		INDEX_PID_IPv4_RSVP,
		INDEX_PID_IPv4_GRE,
		INDEX_PID_IPv4_IGRP,
		INDEX_PID_IPv4_IPOSPF,
		INDEX_PID_IPv4_SCTP,
		INDEX_PID_IPv4_IPSEC_ESP,
		INDEX_PID_IPv4_IPSEC_AH,
	};

public:

	static void IPv4ProtocolDecode(const BYTE* pPacket, unsigned int dwPacketLen, IPv4_DECODE_INFO& macInfo,BOOL bCalCheckSum=false);

private:
	static unsigned int GetProtoIndex(unsigned int protoid)
	{
		unsigned int nprotoindex = INDEX_PID_IPv4_OTHER;
		switch(protoid)
		{
		case 1:
			nprotoindex =INDEX_PID_IPv4_ICMP;
			break;
		case 2:
			nprotoindex =INDEX_PID_IPv4_IPIGMP;
			break;
		case 3:
			nprotoindex =INDEX_PID_IPv4_IPGGP;
			break;
		case 5:
			nprotoindex =INDEX_PID_IPv4_IPSTREAM;
			break;
		case 6:
			nprotoindex =INDEX_PID_IPv4_TCP;
			break;
		case 8:
			nprotoindex =INDEX_PID_IPv4_IPEGP;
			break;
		case 9:
			nprotoindex =INDEX_PID_IPv4_IGP;
			break;
		case 11:
			nprotoindex =INDEX_PID_IPv4_NVPII;
			break;
		case 12:
			nprotoindex =INDEX_PID_IPv4_PUP;
			break;
		case 16:
			nprotoindex =INDEX_PID_IPv4_CHAOS;
			break;
		case 17:
			nprotoindex =INDEX_PID_IPv4_UDP;
			break;
		case 33:
			nprotoindex =INDEX_PID_IPv4_SEP;
			break;
		case 37:
			nprotoindex =INDEX_PID_IPv4_DDP;
			break;
		case 46:
			nprotoindex =INDEX_PID_IPv4_RSVP;
			break;
		case 47:
			nprotoindex =INDEX_PID_IPv4_GRE;
			break;
		case 88:
			nprotoindex =INDEX_PID_IPv4_IGRP;
			break;
		case 89:
			nprotoindex =INDEX_PID_IPv4_IPOSPF;
			break;
		case 132:
			nprotoindex =INDEX_PID_IPv4_SCTP;
			break;
		case 50:
			nprotoindex =INDEX_PID_IPv4_IPSEC_ESP;
			break;
		case 51:
			nprotoindex =INDEX_PID_IPv4_IPSEC_AH;
			break;

		default:
			break;
		}

		return nprotoindex;
	}

public:
	static int GetLinkType(int nprotoindex)
	{
		if( nprotoindex == INDEX_PID_IPv4_TCP )
			return 0x01;
		if( nprotoindex == INDEX_PID_IPv4_UDP )
			return 0x02;

		return 0;
	}

};


