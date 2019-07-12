//////////////////////////////////////////////////////////////////////////
//BufferPktThreadObserver.h

//����CPacketQueManager���������ݣ�Ȼ����������һ���߳�����ȡCPacketQueManager������Ȼ���ٴ���
//�����Ҫ���ڶ��CPU�������������Բ�ͬ��observerʹ�ò�ͬ��cpu���������ݡ�
//�����ﵽcpu������õ�Ŀ�ģ��������������

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

	//buffersize:��Ҫ����Ĵ�С�����buffersize==0����ʾ����Ҫʹ�û����ʹ�ö��߳�
	//pRealObserver:ʵ�ʵ�observer
	//bdelete:�����ָ��pRealObserver���Ƿ���CBufferPktThreadObserver�ڲ�delete
	//block:�Ƿ���������������������ʱ���Ƿ������ȴ��������п�
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

