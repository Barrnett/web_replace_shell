#ifndef _OBSERVER_MGR_H_2006_05_11
#define _OBSERVER_MGR_H_2006_05_11
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"
#include "PacketIO/MyPlatRunCtrl.h"



#if defined(OBSERVER_MGR_EXPORTS)
#define OBSERVERMGR_API		MPLAT_API_Export_Flag
#define OBSERVERMGR_CLASS	MPLAT_CLASS_Export_Flag
#else
#define OBSERVERMGR_API		MPLAT_API_Import_Flag
#define OBSERVERMGR_CLASS	MPLAT_CLASS_Import_Flag
#endif

class CMyBaseObserver;
class OBSERVER_MGR;
class ACE_Log_File_Msg;


class OBSERVERMGR_CLASS CObserverMgrFunc
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
	CObserverMgrFunc();
	~CObserverMgrFunc();

	//如果参数为空，表示使用ACE_Log_Msg::instance();
	BOOL init(ACE_Log_Msg * pLogInstance=NULL,const char * monitor_event_name=NULL,size_t monitor_event_interval=0);
	void close();

	bool StartTest(IRecvDataSource* pdataSource);
	bool StopTest();
	bool PauseTest();
	bool ContinueTest();

	//必须在停止测试的时候才能调用
	void Reset();

	ENState GetRunState() const;
	BOOL IsStopped() const;

	BOOL Reinit();

	inline BOOL GetRunInfo(CMyPlatRunCtrl::PlatRun_Info& runinfo) const
	{
		if( m_pPlatRunCtrl )
		{
			m_pPlatRunCtrl->GetRunInfo(runinfo);

			return TRUE;
		}

		return FALSE;	
	}

	inline PACKET_LEN_TYPE GetLastPacketLenType() const
	{
		if( m_pPlatRunCtrl )
		{
			return m_pPlatRunCtrl->GetLastPacketLenType();
		}

		return PACKET_NO_MORE;
	}

	CMyPlatRunCtrl * GetPlatRunCtrl() const
	{
		return m_pPlatRunCtrl;
	}

private:
	BOOL LoadObservers();
	void UnLoadObservers();

private:
	ACE_Log_Msg *			m_pLogInstance;
	CMyPlatRunCtrl *		m_pPlatRunCtrl;

	BOOL					m_bLoadNetScan;

	std::string				m_monitor_event_name;
	size_t					m_monitor_event_interval;

	OBSERVER_MGR 	*		m_pObserverMgr;
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_OBSERVER_MGR_H_2006_05_11