//////////////////////////////////////////////////////////////////////////
//ConstructPacket2.h

#pragma once


#include "cpf/CommonMacTCPIPDecode.h"

class CPF_CLASS CConstructPacket2
{
public:
	CConstructPacket2(void);
	~CConstructPacket2(void);

public:
	//dsturl：重定向的目标url.
	//interrupt_dir=0x01表示同方向
	//interrupt_dir=0x02表示相反方向

	//是否要阻断，如果需要返回数据包的长度
	//返回0表示不需要重定向

	//建议outbuflen的长度为1024
	static unsigned int PackageTCPInterrupt(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		int interrupt_dir,BYTE * outbuf,int outbuflen);

public:
	//dsturl：重定向的目标url.

	//是否要重定向，如果需要返回数据包的长度
	//返回0表示不需要重定向

	//建议outbuflen的长度为1024
	static unsigned int PackageHttpRedir(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		const char * dsturl,const char *param,
		BYTE * outbuf,int outbuflen);

	//建议outbuflen的长度为256+body_len
	static unsigned int PackageHttpRspTextPacket(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		const char *body_data,int body_len,
		BYTE * outbuf,int outbuflen);

};
