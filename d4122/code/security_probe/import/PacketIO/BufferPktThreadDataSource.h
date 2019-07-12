////////////////////////////////////////////////////////////////////////////
//BufferPktThreadDataSource.h

//这个类通过一个线程来缓存数据源读取数据，
//可以使得一个线程来处理读取数据，另外一个线程可以处理数据。
//这对于多CPU的机器是非常有效的。可以充分发挥多CPU的优势

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"
#include "cpf/CommonQueueManager.h"


class PACKETIO_CLASS CBufferPktThreadDataSource : public IExtDataSource
{
public:
	CBufferPktThreadDataSource(void);
	virtual ~CBufferPktThreadDataSource(void);

	//pRealDatasource，实际的数据源。这个模块从这个数据源读取数据，并且缓存到内存中
	//然后提供接口让应用程序来访问缓存的数据
	//bdelete:传入的pRealDatasource是否需要在CBufferPktThreadDataSource内部来delete
	//bufsize:缓存的大小.如果为0,表示不使用多线程.所有的访问直接转到pRealDatasource
	BOOL init(IRecvDataSource * pRealDatasource,bool bdelete,unsigned int bufsize,const ACE_Time_Value& empty_sleep_time);
	void fini();

	//程序可以在初始化init(..)之后,和fini(..)之前反复OpenDataSource和CloseDataSource


	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving();


	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	virtual int WaitForPacket(const ACE_Time_Value& sleep_time);

public:
	ACE_UINT32 GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen);

private:
	static ACE_THR_FUNC_RETURN MyProcThread (void * param);
	void MyThreadWork();

private:	
	INT_PTR				m_bQuit;
	ACE_hthread_t		m_hThread;
	
	unsigned int				m_share_buffer_size;
	BYTE *						m_alloc_buffer;
	TAG_COMMOM_QUEUE_MANAGER	m_share_queue;

	ACE_Time_Value				m_empty_sleep_time;
};
