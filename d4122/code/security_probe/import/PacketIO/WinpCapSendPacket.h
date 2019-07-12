//////////////////////////////////////////////////////////////////////////
//WinpCapSendPacket.h

#ifndef __PACKETOPERATION_H__
#define __PACKETOPERATION_H__


#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS

#include "PacketIO/PacketIO.h"
#include "PacketIO/WriteDataStream.h"

class PACKETIO_CLASS CWinpCapSendPacket
{
public:
	CWinpCapSendPacket();
	virtual ~CWinpCapSendPacket();

public:
	BOOL SendPacket(const char *pszPacket,DWORD dwPacketLen);
	BOOL Open(int index);
	void Close();

	int GetConnectState();

private:
	void * m_lpAdapter;
};


class PACKETIO_CLASS CWinpcapWriteDataStream : public IDevWriteDataStream
{
public:
	CWinpcapWriteDataStream();
	virtual ~CWinpcapWriteDataStream();

public:
	//�൱��init+OpenDataStream
	BOOL Open(int index);
	//�൱��CloseDataStream+fini
	void Close();

	BOOL init(int index);
	void fini();

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataStream();

	//�ر��豸
	virtual void CloseDataStream();

	virtual int WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums);

	virtual int GetConnectState();

	virtual bool GetDataStreamName(std::string& name);

private:
	CWinpCapSendPacket	m_winpcapSendPacket;
	int					m_nIndex;

};

#endif//OS_WINDOWS

#endif // __PACKETOPERATION_H__

