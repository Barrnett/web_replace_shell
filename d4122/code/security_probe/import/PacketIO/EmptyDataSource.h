//////////////////////////////////////////////////////////////////////////
//EmptyDataSource.h

#pragma once

#include "cpf/ostypedef.h"

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"


class PACKETIO_CLASS CEmptyDataSource : public IRealDataSource
{
public:
	CEmptyDataSource(void);
	virtual ~CEmptyDataSource(void);

	//程序可以在初始化init(..)之后,和fini(..)之前反复OpenDataSource和CloseDataSource

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataSource()
	{
		return 0;
	}

	//关闭设备
	virtual void CloseDataSource()
	{
		return;
	}

	//返回数据源的的名字
	virtual bool GetDataSourceName(std::string& name);

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv()
	{
		memset(&m_packet_header,0x00,sizeof(m_packet_header));

		m_start_time = ACE_OS::gettimeofday();

		return true;
	}
	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving()
	{
		return;
	}

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//使这个接口能够产生定时包
	//如果设置成功，返回true．否则返回false.
	//使用者必须在开始接收以前调用这个函数，在开始接收以后则不允许调用
	virtual BOOL EnableTimerPacket(BOOL bEnable)
	{
		return true;
	}

	//询问这个设备是否可以产生定时包。
	virtual BOOL IsEnableTimerPacket() const
	{
		return true;
	}

	//得到因为上层处理来不及，底层丢包的数目,-1表示无效
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
	{
		pkts = 0;
		bytes = 0;
	}

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState()
	{
		return 1;
	}

private:
	ACE_Time_Value m_start_time;

	PACKET_HEADER	m_packet_header;

};


