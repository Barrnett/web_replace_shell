// VPKHelpDataSource.h: interface for the CVPKHelpDataSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VPKHELPDATASOURCE_H__0C76F3AD_E5AC_4D39_A628_025A9E2E5FEB__INCLUDED_)
#define AFX_VPKHELPDATASOURCE_H__0C76F3AD_E5AC_4D39_A628_025A9E2E5FEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"
#include "PacketIO/VPKGenerator.h"


class PACKETIO_CLASS CVPKHelpDataSource : public IExtDataSource  
{
public:
	CVPKHelpDataSource();
	virtual ~CVPKHelpDataSource();

	BOOL Open(IRecvDataSource * pRealDataSource,int delete_real_source = 0)
	{
		init(pRealDataSource,delete_real_source);

		return (OpenDataSource()==0);
	}
	void Close()
	{
		CloseDataSource();
		fini();
	}


	void init(IRecvDataSource * pRealDataSource,int delete_real_source = 0)
	{
		IExtDataSource::init(pRealDataSource,delete_real_source);
	}

	void fini()
	{
		IExtDataSource::fini();
	}

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving();

	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin();

	//继承者不要实现这个函数
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//使这个接口能够产生定时包
	virtual BOOL EnableTimerPacket(BOOL bEnable)
	{
		m_bEnableVirtualPacket = bEnable;
		return true;
	}
	//询问这个设备是否可以产生定时包。
	virtual BOOL IsEnableTimerPacket() const
	{
		return m_bEnableVirtualPacket;
	}
		
private:
	//当m_bEnableVirtualPacket=true，调用这个函数
	PACKET_LEN_TYPE	GetPacketWithVirtualPacket(RECV_PACKET& packet);

	inline BOOL IsBackupLastPkt() const
	{
		return (PACKET_EMPTY != m_bkType);
	}

private:
	CTimeStamp32	m_LastEffectivePacketStamp;
	PACKET_HEADER	m_virtualHeaderInfo;
	CVPKGenerator	m_vpkGenrator;
	
	//备份上一次读出了包的数据
	RECV_PACKET		m_bkRecvPacket;

	//备份上一次读出了包的数据的长度类型
	PACKET_LEN_TYPE	m_bkType;

	//是否允许产生定时包
	BOOL			m_bEnableVirtualPacket;

};

#endif // !defined(AFX_VPKHELPDATASOURCE_H__0C76F3AD_E5AC_4D39_A628_025A9E2E5FEB__INCLUDED_)
