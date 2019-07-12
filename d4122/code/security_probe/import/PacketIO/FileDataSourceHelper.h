//////////////////////////////////////////////////////////////////////////
//FileDataSourceHelper

#pragma once

#include "PacketIO/RecvDataSource.h"
#include "PacketIO/PacketIO.h"

//class IFileRecvDataSource;

class PACKETIO_CLASS CFileDataSourceHelper : public IRecvDataSource
{
public:
	CFileDataSourceHelper(void);
	virtual ~CFileDataSourceHelper(void);

public:
	BOOL Open(const char * filename,DRIVER_TYPE type)
	{
		if( init(filename,type) == FALSE )
			return false;

		return OpenDataSource() == 0;
	}

	void Close()
	{
		CloseDataSource();
		fini();
	}

	//type表示文件类型：0表示是ethreal所有认识的格式，1是cpt
	BOOL init(const char * filename,DRIVER_TYPE type);
	void fini();

	virtual int OpenDataSource();

	//关闭设备
	virtual void CloseDataSource();


	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv()
	{
		if( m_pFileRecvDataSource )
			return m_pFileRecvDataSource->StartToRecv();

		return false;
	}
	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving()
	{
		if( m_pFileRecvDataSource )
			return m_pFileRecvDataSource->StopRecving();

	}

	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin()
	{
		if( m_pFileRecvDataSource )
			return m_pFileRecvDataSource->SeekToBegin();

		return false;
	}

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet)
	{
		return m_pFileRecvDataSource->GetPacket(packet);
	}

	//得到因为上层处理来不及，底层丢包的数目
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
	{
		if( m_pFileRecvDataSource )
		{
			m_pFileRecvDataSource->GetDroppedPkts(pkts,bytes);
		}
		else
		{
			pkts = bytes = 0;
		}
		
	}

private:
	IFileRecvDataSource *	m_pFileRecvDataSource;

};
