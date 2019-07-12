#ifndef _PROCESS_RUN_HELPER_H_20091203_
#define _PROCESS_RUN_HELPER_H_20091203_

#include "cpf.h"
#include "ostypedef.h"
#include "my_event.h"
#include <string>

class ACE_Process_Mutex;

typedef struct _ST_CMDLINE_OPT_
{
	_ST_CMDLINE_OPT_()
	{ memset(this, 0x0, sizeof(*this));}
	int  iMode;				// 0 == 前台监视状态, 1=back
	char pszMutexName[PATH_MAX];	// 用来生成mutex的名称
	char pszSelfName[PATH_MAX];	// 当前进程的名字
	char pszPathName[PATH_MAX];	// 当前进程所在路径
	char pszConfName[PATH_MAX];	// 当前配置的名字
	int  nProcessID;
    int  nKernelID;         // 绑定的cpu
	bool bHelp;
}ST_CMDLINE_OPT;


class CPF_CLASS CProcessRunHelper
{
public:
	// bRegSign=true, 注册ctrl+c接口
	CProcessRunHelper(bool bRegSign = true);
	~CProcessRunHelper(void);

public:

	std::string		m_strCmdLine;
	ST_CMDLINE_OPT* GetOpt();

	void ShowHelp();

	// 解释参数，-1=解释出错(有未知参数，缺少必选参数)，
	//						0=可以退出( help, -v,)
	//						1=继续运行
	int ParseCommand(int argc,char **argv);

	// 判断是否为后台启动方式
	bool IsService()	{return m_stOpt.iMode != 0;}

	// 判断是否需要输出help信息
	bool IsHelp()			{return m_stOpt.bHelp;}

	// 运行标识访问
	void  SetRunFlag(bool bFlag=true)		{ m_bRunFlag = true;}
	bool  GetRunFlag()		{return m_bRunFlag;}

	// 申请进程锁
	// return 0 on success; -1 on failure.  If the lock could not be acquired
	// because someone else already had the lock, @c errno is set to @c EBUSY.
	int TryAcquire();


	// 事件相关函数部分
	bool Create_Event(const char *strCmdLine );
	void Delete_Event( );

	my_event_t* Create_one_Event(const char *stop_event_name );

	void OnTimer();

	void SetMonitonEvent();
	int CheckStopEvent();


private:
	// 切换到后台运行
	void RunAsService();

	// 注册消息
	bool RegisteSignal();

	// 处理系统退出消息
	static void ProcessExitSignal(int nSig);

	bool GetNameInfo(char* argv0);

	static bool		m_bRunFlag;				// 是否运行标识
private:

	ST_CMDLINE_OPT	m_stOpt;		// 参数结构

	ACE_Process_Mutex*	m_pMutex;

	my_event_t*			m_pMonitorEvent;
	my_event_t*			m_pStopEvent;
};



#endif  //_PROCESS_RUN_HELPER_H_20091203_

