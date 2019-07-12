//////////////////////////////////////////////////////////////////////////
//CommonPPPOEDecode.h

/******************************************************************************
*               
*  FILE NAME  :  CommonPPPOEDecode.h
*               
*  TITLE      :  
*               
*  CREATE TIME:  2007-1-25
*               
*  AUTHOR     : 
*               
*  DESCRIPTION:  PPPOE的解码类的头文件(RFC2516)
*	Version	   :	1.0
*				只实现session方式,并且向上传递数据包
* 				
*****************************************************************************/


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*PPPOE的格式

1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  VER  | TYPE  |      CODE     |          SESSION_ID           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            LENGTH             |           payload             ~
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/

#pragma once

#include "cpf/cpf.h"
#include "cpf/addr_tools.h"

class CPF_CLASS CCommonPPPOEDecode
{
public:
	CCommonPPPOEDecode(void);
	~CCommonPPPOEDecode(void);

public:
	typedef enum{
		INIT = 0,
		DISCORY = 1,
		SESSION = 2,
	}PPPOE_STAGE;

	enum{
		DATA = 0x00,

		//The PPPoE Active Discovery Initiation
		PADI = 0x09,
		//The PPPoE Active Discovery Offer (PADO)
		PADO = 0x07,

		//The PPPoE Active Discovery Request
		PADR = 0x19,
		//The PPPoE Active Discovery Session-confirmation (PADS) packet
		PADS = 0x65,

		//The PPPoE Active Discovery Terminate (PADT) packet
		PADT = 0xa7
	};


	typedef enum
	{
		PPPOE_ERROR_STYLE_NOERROR = 0,

		PPPOE_ERROR_STYLE_OTHER_ERROR = 1,

		PPPOE_ERROR_STYLE_SHORT_HEADER = 2,

		PPPOE_ERROR_STYLE_INVALID_HEADER = 3,
		PPPOE_ERROR_STYLE_TOO_SHORT = 4

	}PPPOE_ERROR_STYLE;

	enum{
		INDEX_PID_OTHER = 0,
		INDEX_PID_PPP = 1,
	};

	typedef struct  
	{
		struct {
			BYTE ver:4;
			BYTE type:4;
		}m_verheader;

		BYTE		m_code;
		WORD		m_sessionId;//网络字节序

		WORD		m_wLength;

		PPPOE_STAGE	m_stage;

		unsigned int			dwPackLen;//MAC本身的长度
		int						nProtoIndex;//上层的协议号

		unsigned int			dwHeaderLength; //被解码协议的协议头部长度(对MAC而言返回MAC帧的头部长度)

		unsigned char *			pUpperData;
		unsigned int			dwUpperProtoLength;  //被解码协议的上层协议的数据长度(对MAC而言返回IPv4的长度)

		PPPOE_ERROR_STYLE		nErrorStyle;

		void reset()
		{
			m_verheader.ver = m_verheader.type = 0;

			m_code = 0;
			m_sessionId = 0;
			m_wLength = 0;

			m_stage = INIT;

			dwPackLen = 0;
			nProtoIndex = 0;

			dwHeaderLength = 0;

			pUpperData = NULL;
			dwUpperProtoLength = 0;


			nErrorStyle = PPPOE_ERROR_STYLE_OTHER_ERROR;
		}

	}PPPOE_DECODE_INFO;

public:
	static void PPPOEDecode(const BYTE * pPacket,unsigned int dwPacketLen,PPPOE_DECODE_INFO& pppoeInfo);

	static const BYTE * GetPPPOEHeaderPos(const BYTE * mac_addr_header);

};


class CPF_CLASS CCommonPPPDecode
{
public:
	CCommonPPPDecode();
	~CCommonPPPDecode();

	enum{
		INDEX_PID_OTHER = 0,
		INDEX_PID_IP = 1,
	};

	enum{
		PID_PPPYPE_IP = 0x0021
	};

	typedef enum
	{
		PPP_ERROR_STYLE_NOERROR = 0,
		PPP_ERROR_STYLE_TOO_SHORT = 1

	}PPP_ERROR_STYLE;

	typedef struct  
	{
		unsigned int			dwPackLen;//MAC本身的长度
		int						nProtoIndex;//上层的协议号

		unsigned int			dwHeaderLength; //被解码协议的协议头部长度(对MAC而言返回MAC帧的头部长度)

		unsigned char *			pUpperData;
		unsigned int			dwUpperProtoLength;  //被解码协议的上层协议的数据长度(对MAC而言返回IPv4的长度)

		PPP_ERROR_STYLE			nErrorStyle;

	}PPP_DECODE_INFO;

public:
	static void PPPDecode(const BYTE * pPacket,unsigned int dwPacketLen,PPP_DECODE_INFO& pppInfo);


};

