//////////////////////////////////////////////////////////////////////////
//BufferPktThreadObserver.h

//利用CPacketQueManager来缓存数据，然后启动另外一个线程来读取CPacketQueManager的数据然后再处理
//这个主要用于多个CPU的情况，这个可以不同的observer使用不同的cpu来处理数据。
//这样达到cpu充分利用的目的，以提高整体性能

#pragma once

#include "PacketIO/MyBaseObserver.h"
#include "ace/Task.h"

class CPacketQueManager;
class CPQMReader;
class CPQMWriter;
class CObserverBaseThread;

class PACKETIO_CLASS CBufferPktThreadObserver : public CMyBaseObserver
{
public:
	CBufferPktThreadObserver(void);
	virtual ~CBufferPktThreadObserver(void);

	//buffersize:需要缓存的大小，如果buffersize==0，表示不需要使用缓存和使用多线程
	//pRealObserver:实际的observer
	//bdelete:传入的指针pRealObserver，是否在CBufferPktThreadObserver内部delete
	//block:是否阻塞，即当缓冲区满的时候，是否阻塞等待缓冲区有空
	BOOL init(unsigned int buffersize,CMyBaseObserver * pRealObserver,
		bool bdelete=false,bool block=true);
	void fini();

	virtual BOOL Reset();
	virtual BOOL OnStart();
	virtual BOOL OnStop(int type);
	virtual BOOL OnPause();
	virtual BOOL OnContinue();
	virtual BOOL Push(PACKET_LEN_TYPE type,RECV_PACKET& packet);

private:
	static ACE_THR_FUNC_RETURN MyProcThread (void * param);
	void MyThreadWork();

private:
	CPacketQueManager *	m_pPQM;
	CPQMWriter *		m_pWriter;
	CPQMReader *		m_pReader;

private:
	INT_PTR				m_bQuit;
	ACE_hthread_t		m_hThread;


	CMyBaseObserver *	m_pRealObserver;
	bool				m_bdelete;	
	bool				m_block;

};

