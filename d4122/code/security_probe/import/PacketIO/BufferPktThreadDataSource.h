////////////////////////////////////////////////////////////////////////////
//BufferPktThreadDataSource.h

//�����ͨ��һ���߳�����������Դ��ȡ���ݣ�
//����ʹ��һ���߳��������ȡ���ݣ�����һ���߳̿��Դ������ݡ�
//����ڶ�CPU�Ļ����Ƿǳ���Ч�ġ����Գ�ַ��Ӷ�CPU������

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"
#include "cpf/CommonQueueManager.h"


class PACKETIO_CLASS CBufferPktThreadDataSource : public IExtDataSource
{
public:
	CBufferPktThreadDataSource(void);
	virtual ~CBufferPktThreadDataSource(void);

	//pRealDatasource��ʵ�ʵ�����Դ�����ģ����������Դ��ȡ���ݣ����һ��浽�ڴ���
	//Ȼ���ṩ�ӿ���Ӧ�ó��������ʻ��������
	//bdelete:�����pRealDatasource�Ƿ���Ҫ��CBufferPktThreadDataSource�ڲ���delete
	//bufsize:����Ĵ�С.���Ϊ0,��ʾ��ʹ�ö��߳�.���еķ���ֱ��ת��pRealDatasource
	BOOL init(IRecvDataSource * pRealDatasource,bool bdelete,unsigned int bufsize,const ACE_Time_Value& empty_sleep_time);
	void fini();

	//��������ڳ�ʼ��init(..)֮��,��fini(..)֮ǰ����OpenDataSource��CloseDataSource


	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();


	//��������
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
