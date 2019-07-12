//////////////////////////////////////////////////////////////////////////
//CommonMainCtrl.h

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/MyPlatRunCtrl.h"
#include "PacketIO/DataSourceMgr.h"

class OBSERVER_MGR;

class PACKETIO_CLASS CCommonMainCtrl
{
public:
	enum ENState
	{
		STATE_UNINIT,//没有初始化
		STATE_STOPPING,//准备结束
		STATE_IDLE,//还没有开始测试或者已经停止测试
		STATE_RUN,//正在运行
		STATE_PAUSE,//正在运行，但是暂停
	};

public:
	CCommonMainCtrl(void);
	virtual ~CCommonMainCtrl(void);

public:
	virtual BOOL init(ACE_Time_Value empty_sleep_time,
		const char * datasourc_cfg_name,
		const char * observer_list_cfg_name,
		const char * observer_section_name,
		ACE_Log_Msg * pLogInstance,
		const char * monitor_event_name,
		size_t monitor_event_interval,
		int copy_drv_data);

	virtual BOOL init(ACE_Time_Value empty_sleep_time,
		ACE_Log_Msg * pLogInstance,
		const char * monitor_event_name,
		size_t monitor_event_interval,
		int copy_drv_data);

	//必须在StartTest之前调用。应该在主程序初始化的时候，加入。
	void attach_observer(CMyBaseObserver * pObserver);

	virtual void close();

public:
	//pdataSource=NULL,表示采用初始化的时候使用的配置文件中指定的数据源
	virtual bool StartTest(IRecvDataSource* pdataSource=NULL);
	virtual bool StartTest(IRecvDataSource* pdataSource, CSerialDataRecv* pSerialRecv);
	virtual bool StopTest();
	virtual bool PauseTest();
	virtual bool ContinueTest();

public:
	ENState GetRunState() const;
	BOOL IsStopped() const;

	BOOL GetRunInfo(CMyPlatRunCtrl::PlatRun_Info& runinfo) const;
	const CMyPlatRunCtrl::PlatRun_Info& GetRunInfo() const;

	PACKET_LEN_TYPE GetLastPacketLenType() const;

	inline CMyPlatRunCtrl * GetPlatRunCtrl() const
	{	return m_pPlatRunCtrl;	}

	int GetDataSourceConnectState();

private:
	CDataSourceMgr			m_datasource_mgr;

	CMyPlatRunCtrl *		m_pPlatRunCtrl;

	ACE_Log_Msg *			m_pLogInstance;
	BOOL					m_bLogInstanceCreate;

	std::string				m_monitor_event_name;
	size_t					m_monitor_event_interval;

	OBSERVER_MGR 	*		m_pObserverMgr;
	
private:
	BOOL Inner_init(ACE_Time_Value empty_sleep_time,
		ACE_Log_Msg * pLogInstance,
		const char * monitor_event_name,
		size_t monitor_event_interval,
		int copy_drv_data);

	BOOL LoadObserversFromCfg(
		const char * observer_list_cfg_name,
		const char * observer_section_name);

	void UnLoadObserversFromCfg();

	ACE_Log_Msg *ReadyLogMsg(unsigned int log_interval);

	bool WaitProtecoterFree(int sec);
};
