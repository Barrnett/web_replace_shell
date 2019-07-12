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
	CTimeStamp32		m_firstPacketTime;//第一个包的时间
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
	CTimeStamp32	m_delta;//应该叠加的时间

	CTimeStamp32	m_lastBeginTime;//本文件的第一个包的时间

	CTimeStamp32	m_lastPktTime;//当前包的时间

	int				m_re_calculate_time;//是否要重新计算时间
	size_t			m_nDefLoops;//定义的需要循环多少次,循环的次数,0表示无限循环，其他表示循环次数
	size_t			m_nCurLoops;//当前已经循环了几次

private:
	IRecvDataSource *	m_pRecvDataSource;

};

