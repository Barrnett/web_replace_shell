//////////////////////////////////////////////////////////////////////////
//SerialDataSend.h

#pragma once

#include "cpf/CommonQueueManager.h"
#include "PacketIO/RecvDataSource.h"
#include "ace/Task.h"

class PACKETIO_CLASS CSerialSendTask : public ACE_Task_Base
{
public:
	CSerialSendTask();
	virtual ~CSerialSendTask();

public:
	bool 		m_bUserStop;
	
	ACE_UINT8	m_nSerialIndex;
	
	int				m_nSerialFd;
	unsigned char*	m_pSendBuf;
	TAG_COMMOM_QUEUE_MANAGER m_tagSendQueue;
	
	sem_t 		m_sem;
	
public:
	int f_CreateQueue(ACE_UINT32 nBufferSize);

private:
	unsigned char m_acPacketBuf[1600];
	ACE_INT32	m_nPacketLen;
	int svc();
};

class PACKETIO_CLASS CSerialDataSend
{
public:
	CSerialDataSend();
	virtual ~CSerialDataSend();

public:
	std::vector<CSerialSendTask*> m_vtSerialSendTask;
	
public:
	int f_Init();
	int f_AddSerialIndex(ACE_UINT8 nIndex, int nFd);
	int f_DeleteSerialIndex(ACE_UINT8 nIndex);
	
	//开始发送数据，注意不是打开设备，可能有些子类没有具体实现。
	void f_StartSend();
	//停止发送数据，注意不是关闭设备，可能有些子类没有具体实现。
	void f_StopSend();

	int f_SendPacket(unsigned char * pPacket, unsigned int nPacketlen, ACE_UINT8 nIndex);
};
