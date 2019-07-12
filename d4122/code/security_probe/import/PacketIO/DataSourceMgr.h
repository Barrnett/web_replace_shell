#ifndef _DATA_SOURCE_MGR_2006_05_09
#define _DATA_SOURCE_MGR_2006_05_09
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"
#include "PacketIO/DataSource_Param_Info.h"

class CDataSourceParamReader;

class PACKETIO_CLASS CDataSourceMgr
{
public:
	CDataSourceMgr(void);
	~CDataSourceMgr(void);

	CDataSourceMgr(const CDataSourceMgr& other)
	{
		ACE_ASSERT(FALSE);
	}

	BOOL init(const char * filename,int type,ACE_Log_Msg * pLogInstance=NULL);
	void close();
	void writefile(const char * filename=NULL);

	IRecvDataSource * OpenRecvDataSource();
	void CloseRecvDataSource();

	IRecvDataSource * GetCurRecvDataSource() const
	{	return m_pRecvDataSource;}

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	int GetConnectState();

public:
	CDataSource_Param_Info		m_datasource_paraminfo;

private:
#ifdef OS_WINDOWS
	IRecvDataSource * OpenWinpCapDataSource(DRIVER_TYPE type);	
#endif

	IRecvDataSource * OpenEtherioDataSource(DRIVER_TYPE type);

	IRecvDataSource * OpenMultiFilePlaybackDataSource(DRIVER_TYPE type);
	IRecvDataSource * OpenMemoryDataSource(DRIVER_TYPE type);

	IRecvDataSource * OpenShareMemDataSource(DRIVER_TYPE type);

private:
	IRecvDataSource *			m_pRecvDataSource;
	ACE_Log_Msg *				m_pLogInstance;
	CDataSourceParamReader *	m_pDataSourceParamReader;




};

#include "ace/Singleton.h"
#include "ace/Null_Mutex.h"
class PACKETIO_CLASS CDataSourceMgr_Singleton : public ACE_Singleton<CDataSourceMgr,ACE_Null_Mutex>
{
public:
	static CDataSourceMgr * instance()
	{
		return ACE_Singleton<CDataSourceMgr,ACE_Null_Mutex>::instance();
	}

};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_DATA_SOURCE_MGR_2006_05_09

