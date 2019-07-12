//////////////////////////////////////////////////////////////////////////
//TimeAdjustTool.h

#include "PacketIO/RecvDataSource.h"

#pragma once

//////////////////////////////////////////////////////////////////////////
//CTimeAdjustTool

class CTimeAdjustTool
{
public:
	CTimeAdjustTool(void);
	~CTimeAdjustTool(void);

	BOOL init(int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp);
	void fini();

	void start_to_work();
	void stop_working();

	void do_work(PACKET_LEN_TYPE& lentype,RECV_PACKET& packet);

public:
	CTimeStamp32		m_orgin_adjust_time;
	int					m_nAdjustType;
	CTimeStamp32		m_adjustTime;
	CTimeStamp32		m_firstPacketTime;//��һ������ʱ��
};


//////////////////////////////////////////////////////////////////////////
//CCopyAndDiscardTool

#include "cpf/Octets.h"

class CCopyAndDiscardTool
{
public:
	CCopyAndDiscardTool();
	~CCopyAndDiscardTool();

public:
	BOOL init(BOOL bCopyPkt,unsigned int discard_rate);
	void fini();

	void start_to_work();
	void stop_working();

	void do_work(PACKET_LEN_TYPE& lentype,RECV_PACKET& packet);

private:
	BOOL			m_bCopyPkt;
	CCPFOctets		m_CopyData;
	unsigned int	m_discard_rate;

};


//////////////////////////////////////////////////////////////////////////
//CLoopTool

class CLoopTool
{
public:
	CLoopTool();
	~CLoopTool();

	void init(IRecvDataSource * pRealDataSource,size_t loopnums,int re_calculate_time)
	{
		m_nDefLoops = loopnums;
		m_nCurLoops = 0;

		m_re_calculate_time = re_calculate_time;

		m_pRecvDataSource = pRealDataSource;
	}

	void fini();


	void start_to_work();
	void stop_working();

	void do_work(PACKET_LEN_TYPE& lentype,RECV_PACKET& packet);

private:
	CTimeStamp32	m_delta;//Ӧ�õ��ӵ�ʱ��

	CTimeStamp32	m_lastBeginTime;//���ļ��ĵ�һ������ʱ��

	CTimeStamp32	m_lastPktTime;//��ǰ����ʱ��

	int				m_re_calculate_time;//�Ƿ�Ҫ���¼���ʱ��
	size_t			m_nDefLoops;//�������Ҫѭ�����ٴ�,ѭ���Ĵ���,0��ʾ����ѭ����������ʾѭ������
	size_t			m_nCurLoops;//��ǰ�Ѿ�ѭ���˼���

private:
	IRecvDataSource *	m_pRecvDataSource;

};

