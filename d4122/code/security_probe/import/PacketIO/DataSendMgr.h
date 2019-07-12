//////////////////////////////////////////////////////////////////////////
//DataSendMgr.h

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/WriteDataStream.h"
#include "PacketIO/DataSend_Param_Info.h"

class CDataSendParamReader;

class PACKETIO_CLASS CDataSendMgr
{
public:
	CDataSendMgr(void);
	~CDataSendMgr(void);

	CDataSendMgr(const CDataSendMgr& other)
	{
		ACE_ASSERT(FALSE);
	}

public:
	BOOL init(const char * filename,int type,ACE_Log_Msg * pLogInstance=NULL);
	void close();
	void writefile(const char * filename=NULL);

	IWriteDataStream * OpenWriteDataStream();
	void CloseWriteDataStream();

	IWriteDataStream * GetCurWriteDataStream() const
	{
		return m_pWriteDataSteam;
	}


	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	int GetConnectState();

	bool GetDataStreamName(std::string& name);

public:
	CDataSend_Param_Info	m_datasend_param_info;

private:
	IWriteDataStream *	m_pWriteDataSteam;

	ACE_Log_Msg *		m_pLogInstance;

	CDataSendParamReader *	m_pDataSendParamReader;


private:
#ifdef OS_WINDOWS
	IWriteDataStream * OpenSendWinpCapDataStream(DRIVER_TYPE drvtype);
#endif
	IWriteDataStream * OpenSendEtherioDataStream(DRIVER_TYPE drvtype);
	IWriteDataStream * OpenEtherealDataStream(DRIVER_TYPE drvtype);
	IWriteDataStream * OpenCptDataStream(DRIVER_TYPE drvtype);
	IWriteDataStream * OpenShareMemDataStream(DRIVER_TYPE drvtype);

};


#include "ace/Singleton.h"
#include "ace/Null_Mutex.h"

class PACKETIO_CLASS CDataSendMgr_Singleton : public ACE_Singleton<CDataSendMgr,ACE_Null_Mutex>
{
public:
	static CDataSendMgr * instance()
	{
		return ACE_Singleton<CDataSendMgr,ACE_Null_Mutex>::instance();
	}

};

