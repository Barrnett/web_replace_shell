//////////////////////////////////////////////////////////////////////////
//ZCCPTDataSource.h

#pragma once


#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

class CCapDataRead;

class PACKETIO_CLASS CZCCPTDataSource : public ISingleFileRecvDataSource
{
public:
	CZCCPTDataSource(void);
	virtual ~CZCCPTDataSource(void);

public:
	virtual BOOL Open(const char * filename,BOOL bcur_systime,
		unsigned int nReadNums,unsigned int bufferSizeMB)
	{
		m_nReadNums = nReadNums;
		m_nBufferSizeMB = bufferSizeMB;

		return ISingleFileRecvDataSource::Open(filename,bcur_systime);
	}

	//这个数据源统共有多少个包，多少个字节,-1表示无效
	//该函数必须在OpenDataSource返回成功后才能调用

	//一般对于文件类型的数据源才有意义
	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops);


	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving();

	virtual bool GetDataSourceName(std::string& name);

	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin();

	virtual int GetConnectState();

public:
	virtual int OpenDataSource();
	virtual void CloseDataSource();

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

private:
	CCapDataRead *	m_pCapDataRead;
	PACKET_HEADER	m_headerInfo; 
	CTimeStamp32	m_lastPktStamp;

	unsigned int	m_nReadNums;
	unsigned int	m_nBufferSizeMB;


};


//////////////////////////////////////////////////////////////////////////
//CVPKZCCPTDataSource
//////////////////////////////////////////////////////////////////////////
#include "VPKHelpDataSource.h"

class PACKETIO_CLASS CVPKZCCPTDataSource : public CVPKHelpDataSource
{
public:
	CVPKZCCPTDataSource();
	virtual ~CVPKZCCPTDataSource();

public:
	BOOL Open(const char *filename,BOOL bcur_systime);
	void Close();

private:
	CZCCPTDataSource *	m_pZCCPTDataSource;

};

