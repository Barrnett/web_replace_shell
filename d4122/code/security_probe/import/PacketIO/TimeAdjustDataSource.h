//////////////////////////////////////////////////////////////////////////
//TimeAdjustDataSource.h

#pragma once

#include "PacketIO/RecvDataSource.h"
#include "PacketIO/PacketIO.h"

class CTimeAdjustTool;

class PACKETIO_CLASS CTimeAdjustDataSource : public IExtDataSource
{
public:
	CTimeAdjustDataSource(void);
	virtual ~CTimeAdjustDataSource(void);

	//nAdjustType;调整时间类型：为0表示不调整，
	//  为1表示使用系统当前时间来调整来代替每个包的时间
	//	;为2表示使用具体时间m_strAdjustBeginTime来调整（目前不支持）
	//	;读文件的数据的时候，新的每个包的时间 = m_strAdjustBeginTime+(包的时间-第一个包的时间）
	//	;缺省为0

	//nAdjustType=2的时候，pAdjustTimeStamp有效:如果为NULL，表示不调整
	//如果pAdjustTimeStamp->sec,pAdjustTimeStamp->ns为0，表示使用当前时间来调整
	//其他使用指定的时间来调整
	BOOL Open(IRecvDataSource * pRealDataSource,int delete_real_source,
		int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp)
	{
		init(pRealDataSource,delete_real_source,nAdjustType,pAdjustTimeStamp);

		return (OpenDataSource()==0);
	}

	void Close()
	{
		CloseDataSource();
		fini();
	}


	//nAdjustType=2的时候，pAdjustTimeStamp有效:如果为NULL，表示不调整
	//或者pAdjustTimeStamp->sec,pAdjustTimeStamp->ns为0，表示使用当前时间来便宜调整
	//其他使用指定的时间来调整
	void init(IRecvDataSource * pRealDataSource,int delete_real_source,
		int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp);
	
	void fini();

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving();

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);


private:
	CTimeAdjustTool *	m_pTimeAdjustTool;
};
