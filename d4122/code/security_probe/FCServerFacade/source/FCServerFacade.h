//////////////////////////////////////////////////////////////////////////
//FCServerFacade.h

#pragma once

#include "cpf/TinyXmlEx.h"
#include "ace/Event_Handler.h"
#include "cpf/MyACETimer.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "SockManager.h"
#include "cpf/my_event.h"
#include "cpf/CPF_ShareMem.h"
#include "UNS_define.h"
#include "LMB_serial_class.h"
#include "ProbeSysInfo.h"
#include "EZInclude.h"

class CProbeNetCom;
class LMB_SCREEN_SERIAL;
class CProbeSysInfo;

class CFCServerFacade : public ACE_Event_Handler
{
friend class CFCLiveDoCmd;

public:
	CFCServerFacade(void);
	virtual ~CFCServerFacade(void);

public:
	BOOL init(
		ACE_Log_File_Msg * pLogInstance,
		const char * monitor_event_name,
		int monitor_event_interval);

	void fini();

	BOOL Start();
	void Stop();

public:
	BOOL BackupSysCfgFile(const char * sys_cfg_file);
	BOOL LoadSysCfgFile(const char * sys_cfg_file, int incude_local_machine);

	BOOL RestoreOrig();

public:
	void OnFCReconnectedToServer(BOOL bConnectOK,CConnectToServerMgr * sock_mgr);

public:
	char m_strSoftVersion[64];

	enum{
		TIMER_Test_ConnectToServer=1,
		TIMER_SET_EVENT = 2,
		TIMER_LMB_SHOW = 3, // add by xumx 2016-10-24
	};

	CMyACETimer 				m_ActiveTimer;

	CPF_ShareMem m_iUnsStatShm;
	UNS_STAT_SHM_S* m_pUnsStatShm;
	LMB_SCREEN_SERIAL *		m_pLMB_SCREEN_SERIAL;
	CProbeSysInfo *		m_pProbeSysInfo;
	// 单独启动一个线程，响应液晶屏的key
	ACE_thread_t m_LMB_thread_Id;
	ACE_hthread_t m_LMB_thread_Handle;

	
	virtual int handle_timeout(const ACE_Time_Value& tv,const void *arg);

	void schedule_timer();

	char* f_GetSoftVersion(void);
	int f_UiInit();
	void f_RefreshLmbData(unsigned int nCurLine);
	void handle_LBM_show();


private:
	BOOL LoadProbeNetcom();

	void UnLoadProbeNetcom();

private:
	void TestConnectToServer();

	//返回1，表示这次成功返回
	//返回0，表示本次没有成功返回，但是还暂时不认为出问题
	//返回-1，表示已经阻塞了
	int SendEchoToServer(int& out_total_fail_nums,int& out_connect_fail_nums);

private:
	my_event_t *	m_pMonitorEvent;
	int				m_monitor_event_interval;

private:
	int	m_nListenPort;

	BOOL ReadListenPort(int& nListenPort);

public:
	inline int GetListenPort() const
	{
		return m_nListenPort;
	}


public:
	CFCSockManager		m_FCSockManager;

	CProbeNetCom *		m_pProbeNetCom;

private:
	ACE_Log_File_Msg *		m_pLogInstance;

public:
	inline ACE_Log_File_Msg * GetLogInstance() const
	{
		return m_pLogInstance;
	}
private:
	ACE_Recursive_Thread_Mutex	m_tm;

private:
	ACE_UINT32		m_prg_start_time;

public:
	//获取程序运行的时间长度
	ACE_UINT32		GetPgrRunTime_Second() const;

};
