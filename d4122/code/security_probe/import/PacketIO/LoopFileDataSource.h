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

	//loopnums循环的次数,0表示无限循环，其他表示循环次数
	//re_calculate_time：对于循环回放的文件，时间是否重新计算。
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


	//loopnums循环的次数,0表示无限循环，其他表示循环次数
	//re_calculate_time：对于循环回放的文件，时间是否重新计算。
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

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving();

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);


	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin();

private:
	CTimeStamp32	m_delta;//应该叠加的时间
	
	CTimeStamp32	m_lastBeginTime;//本文件的第一个包的时间

	CTimeStamp32	m_lastPktTime;//当前包的时间

	int				m_re_calculate_time;//是否要重新计算时间
	size_t			m_nDefLoops;//定义的需要循环多少次,循环的次数,0表示无限循环，其他表示循环次数
	size_t			m_nCurLoops;//当前已经循环了几次
};
