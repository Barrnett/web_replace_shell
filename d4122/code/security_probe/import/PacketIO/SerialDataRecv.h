//////////////////////////////////////////////////////////////////////////
//SerialDataRecv.h

#pragma once

#include "cpf/ostypedef.h"
#include "cpf/CommonQueueManager.h"
#include "ace/Task.h"
#include "PacketIO/PacketIO.h"

#pragma pack(4)
typedef struct SERIAL_RECV_HEAD
{
	int			nSerialFd;
	ACE_UINT8	nSerialIndex;
	ACE_UINT8 	nReserve[3];
}SERIAL_RECV_HEAD_S;
#pragma pack()

typedef enum SERIAL_RECV_STAT
{
	RECV_IDLE = 0,
	RECV_DATA,
	RECV_7D,
}SERIAL_RECV_STAT_E;

class PACKETIO_CLASS CSerialRecvTask : public ACE_Task_Base
{
public:
	CSerialRecvTask();
	virtual ~CSerialRecvTask();

public:
	bool 		m_bUserStop;
	
	ACE_UINT8	m_nSerialIndex;
	
	int			m_nSerialFd;
	pthread_mutex_t* m_pmutex;
	TAG_COMMOM_QUEUE_MANAGER* m_pRecvQueue;
	
private:
	SERIAL_RECV_STAT_E m_eRecvStat;
	unsigned char m_acPacketBuf[sizeof(SERIAL_RECV_HEAD_S)+1600];
	//ACE_INT32	m_nPacketLen;
	int svc();
};

class PACKETIO_CLASS CSerialDataRecv
{
public:
	CSerialDataRecv();
	virtual ~CSerialDataRecv();

public:
	unsigned char*	m_pRecvBuf;
	TAG_COMMOM_QUEUE_MANAGER m_tagRecvQueue;
	PACKET_HEADER	m_tagPacketHead;

	pthread_mutex_t	m_mutex;

	std::vector<CSerialRecvTask*> m_vtSerialRecvTask;

public:
	int f_Init();

	int f_AddSerialIndex(ACE_UINT8 nIndex, int nFd);
	int f_DeleteSerialIndex(ACE_UINT8 nIndex);

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	void f_StartRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	void f_StopRecv();

	//接收数据
	PACKET_LEN_TYPE f_GetPacket(RECV_PACKET& packet);
};

