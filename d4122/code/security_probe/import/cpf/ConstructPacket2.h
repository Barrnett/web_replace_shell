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
	//dsturl���ض����Ŀ��url.
	//interrupt_dir=0x01��ʾͬ����
	//interrupt_dir=0x02��ʾ�෴����

	//�Ƿ�Ҫ��ϣ������Ҫ�������ݰ��ĳ���
	//����0��ʾ����Ҫ�ض���

	//����outbuflen�ĳ���Ϊ1024
	static unsigned int PackageTCPInterrupt(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		int interrupt_dir,BYTE * outbuf,int outbuflen);

public:
	//dsturl���ض����Ŀ��url.

	//�Ƿ�Ҫ�ض��������Ҫ�������ݰ��ĳ���
	//����0��ʾ����Ҫ�ض���

	//����outbuflen�ĳ���Ϊ1024
	static unsigned int PackageHttpRedir(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		const char * dsturl,const char *param,
		BYTE * outbuf,int outbuflen);

	//����outbuflen�ĳ���Ϊ256+body_len
	static unsigned int PackageHttpRspTextPacket(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		const char *body_data,int body_len,
		BYTE * outbuf,int outbuflen);

};
