//////////////////////////////////////////////////////////////////////////
//MonitorSvc.h

#pragma once

#include "unierm_ctrl_dll_macro.h"
#include "cpf/SubNTServiceBase.h"
#include "ace/streams.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "cpf/MyACETimer.h"
#include "MonitorTask.h"

class CMonitorManager;
class CProbeNetCom;
class CSEProgramCtrl;
class LMB_SCREEN_SERIAL;

class UNIERM_CTRL_DLL_CLASS MonitorSvc : public CSubNTServiceBase
{
public:
	MonitorSvc(BOOL bDebug);
	~MonitorSvc(void);

public:
	virtual int handle_timeout(const ACE_Time_Value &, const void *);
	virtual int start_svc();
	virtual int stop_svc();

public:
	void GetMonitorTaskInfo(std::vector<MONITOR_TASK>& vTask);
	int GetTaskConfigState(const char * name,int& state,BOOL bTempState);
	int ChangeConfigState(const char * prg_name,int state,BOOL bTempState);

	int GetTaskRunState(const char * name,int& state);

public:
	void StopAllTask(BOOL bTempStat);
	void StartAllTask(BOOL bTempStat);

public:
	void Reboot(int wait_second);

private:
	int open();
	BOOL LoadMonitorTask();
	void UnLoadMonitorTask();

	BOOL LoadProbeNetcom();
	void UnLoadProbeNetcom();

	void ReadRebootInfo();

	// add by xumx, 2016-12-12
	char *f_get_system_version();
	char m_version_string[64];

public:
	BOOL WriteRebootInfo();

private:
	bool CheckToReboot();
	bool CheckToPowerOff();

private:	
	CMyACETimer				m_ATimer;
	long					m_TimeId;
	ACE_Log_File_Msg        m_Log;

	CMonitorManager *		m_pMonitorMgr;
	CProbeNetCom *			m_pProbeNetCom;

	CSEProgramCtrl *		m_pSEProgramCtrl;

	BOOL					m_bDebug;

public:
	time_t						m_timeback_lasttime;
	int							m_timeback_interval;	// 时间回跳多长时间将程序重启
	std::vector<std::string>	m_timeback_program;		// 时间回跳重启的程序

	int						m_enable_reboot;//允许周期性关机

	//重新启动的周期
	int						m_nRebootPeriodDay;
	//具体时间，时分秒
	int						m_nRebootHour;
	int						m_nRebootMin;
	int						m_nRebootSec;

	int						m_nRebootTimeout_Param;

	int						m_enable_poweroff;//允许每日关机
	//具体时间，时分秒
	int						m_nPoweroffHour;
	int						m_nPoweroffMin;
	int						m_nPoweroffSec;
};

