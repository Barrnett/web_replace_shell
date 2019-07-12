//////////////////////////////////////////////////////////////////////////
//LoopFileDataSource.h

#pragma once

#include "PacketIO/RecvDataSource.h"
#include "PacketIO/PacketIO.h"

class PACKETIO_CLASS CLoopFileDataSource : public IExtDataSource
{
public:
	CLoopFileDataSource(void);
	virtual ~CLoopFileDataSource(void);

	//loopnumsѭ���Ĵ���,0��ʾ����ѭ����������ʾѭ������
	//re_calculate_time������ѭ���طŵ��ļ���ʱ���Ƿ����¼��㡣
	BOOL Open(IRecvDataSource * pRealDataSource,size_t loopnums,int delete_real_source,int re_calculate_time)
	{
		init(pRealDataSource,loopnums,delete_real_source,re_calculate_time);

		return (OpenDataSource()==0);
	}
	void Close()
	{
		CloseDataSource();
		fini();
	}


	//loopnumsѭ���Ĵ���,0��ʾ����ѭ����������ʾѭ������
	//re_calculate_time������ѭ���طŵ��ļ���ʱ���Ƿ����¼��㡣
	void init(IRecvDataSource * pRealDataSource,size_t loopnums,int delete_real_source,int re_calculate_time)
	{
		m_nDefLoops = loopnums;
		m_nCurLoops = 0;

		m_re_calculate_time = re_calculate_time;

		IExtDataSource::init(pRealDataSource,delete_real_source);

	}

	void fini()
	{
		IExtDataSource::fini();		
	}

	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops);

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);


	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin();

private:
	CTimeStamp32	m_delta;//Ӧ�õ��ӵ�ʱ��
	
	CTimeStamp32	m_lastBeginTime;//���ļ��ĵ�һ������ʱ��

	CTimeStamp32	m_lastPktTime;//��ǰ����ʱ��

	int				m_re_calculate_time;//�Ƿ�Ҫ���¼���ʱ��
	size_t			m_nDefLoops;//�������Ҫѭ�����ٴ�,ѭ���Ĵ���,0��ʾ����ѭ����������ʾѭ������
	size_t			m_nCurLoops;//��ǰ�Ѿ�ѭ���˼���
};
