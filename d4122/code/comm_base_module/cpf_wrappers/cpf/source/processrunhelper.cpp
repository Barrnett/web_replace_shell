
#include "cpf/processrunhelper.h"
/*
#include <string.h>
#ifdef OS_LINUX
#	include <unistd.h>
#	include <sched.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <fcntl.h>
#	include <signal.h>
#endif*/
#include <stdio.h>
#include <stdlib.h>
#include "cpf/path_tools.h"
#include "ace/Process_Mutex.h"
#include "ace/Get_Opt.h"

bool CProcessRunHelper::m_bRunFlag = true;



CProcessRunHelper::CProcessRunHelper(bool bRegSign)
{
	memset(&m_stOpt, 0x0, sizeof(m_stOpt));

	m_pMutex = NULL;

	m_pMonitorEvent = NULL;
	m_pStopEvent = NULL;
//	m_bStopEvent = false;

	if (bRegSign)
	{
		RegisteSignal();
	}
}

CProcessRunHelper::~CProcessRunHelper()
{
	Delete_Event();

	if (NULL != m_pMutex)
	{
		m_pMutex->release();
		delete m_pMutex;
		m_pMutex = NULL;
	}
}


void CProcessRunHelper::ShowHelp()
{
	printf(
          "%s  [-s|S] [-k|K] [-b|B] [-h|H] [-v|V]\n"
		"               -s '控制事件名 停止事件名  轮询时间1 轮询时间2';     程序控制参数，\n"
		"               -k cpu_index;	             描述当前进程绑定cpu\n"
		"               -b;	                     描述代理进程的启动方式为后台启动，无此参数为前台启动\n"
		"               -h;	                     显示当前信息\n"
		"               -f file_full_path;	     指定配置文件\n"
		"\n\n",

// 	        "             s|S 可选参数，运行参数串，\n"
// 			"             p|P 可选参数，描述当前进程的编号\n"
//    	        "             k|K 可选参数，描述当前进程绑定cpu\n"
// 	        "             b|B 可选参数，描述代理进程的启动方式为后台启动，无此参数为前台启动\n"
// 	        "             h|H 可选参数，显示当前信息\n"
// 	        "             f|F 可选参数，制定配置文件\n"
// 	        "             v|V 可选参数，显示版本信息（当前不支持）\n",
          m_stOpt.pszSelfName);
}

int CProcessRunHelper::ParseCommand(int argc,char **argv)
{
	if (0 == argc || NULL == argv)
	{
		return -1;
	}

	GetNameInfo(argv[0]);

	m_strCmdLine = "";

	int option;
	int iRtn = 1;	// 暂时没有比选参数

	//'(":f:h:")'类似一个参数解析的模版 字母紧跟着一个冒号':'那就意味着该“参数开关”    //后期望一个参数   
	//"f:" 和"h:"说明参数列表必须是 -f parm1 -h parm2的形式   
	//options[] = "ab:" 则说明参数列表应该是 -a -b parm1类似的形式    
	static const ACE_TCHAR options[] = ACE_TEXT (":s:S:bBk:K:f:F:hHvV");   
	//ACE_Get_Opt会根据指定的参数和模版生产我们需要的参数列表   
	ACE_Get_Opt cmd_opts (argc, argv, options);  


	while ((option = cmd_opts ()) != EOF)   
	{
		switch (option)
		{
		case 's':
		case 'S':
			m_strCmdLine = cmd_opts.opt_arg();
			break;

		case 'b':
		case 'B':
			m_stOpt.iMode = 1;
			RunAsService();
			break;

/*
//	不支持进程ID
		case 'p':
		case 'P':
			m_stOpt.nProcessID = atoi(cmd_opts.opt_arg());
			sprintf(m_stOpt.pszMutexName, "mutex_%s_%d", m_stOpt.pszSelfName, m_stOpt.nProcessID);
			break;
*/
		case 'h':
		case 'H':
		case 'v':
		case 'V':
			m_stOpt.bHelp = true;
			ShowHelp();
			return 0;

		case 'f': // 指定配置文件
		case 'F':
			ACE_OS_String::strncpy (m_stOpt.pszConfName,   
				cmd_opts.opt_arg (),   
				MAXPATHLEN); 
			break;

#ifndef OS_WINDOWS
		case 'k':
		case 'K':
			m_stOpt.nKernelID= atoi(optarg);

			if(-1 != m_stOpt.nKernelID)
			{
				cpu_set_t mask;
				CPU_ZERO(&mask);
				CPU_SET(m_stOpt.nKernelID, &mask);
				//#ifdef KERNEL4
					//		if(sched_setaffinity(0, &mask)==-1)
				//#elif KERNEL6
				if(sched_setaffinity(0, sizeof(mask), &mask)==-1)
				{
					printf("current process bind kernel failed %d \n", m_stOpt.nKernelID );
				}
				else
				{
					printf("current process bind kernel %d \n", m_stOpt.nKernelID );
				}
				//#endif
			}
			break;
#endif
		default:
//		printf("\n %s ParseCmdLine: find unknow parameter [%c]\n", argv[0], ch);
			return -1;
		}
	}

	return iRtn;
}



ST_CMDLINE_OPT* CProcessRunHelper::GetOpt()
{
	return &m_stOpt;
}

// 这个函数只能调用一次
void CProcessRunHelper::RunAsService()
{
	int iChildID = ACE_OS::fork();
	if ( iChildID == -1 ) 
	{
		perror( "fork fail" );	// 出错
		exit(1);
	} 
	else if( iChildID > 0 ) 
	{
		exit(1);	// 主进程:: 退出
	}

	// 现在是子进程了
	if ( (ACE_OS::setsid()) < 0)
	{
		perror("set pgrp err");
		exit(1);
	}

	// 切换到程序所在路径下
/*
	char szPathName[MAX_PATH];
	CPF::GetModulePathName(szPathName, MAX_PATH);
	if ( (chdir(szPathName)) < 0)
	{
		perror("chdir fail");
		exit(1);
	}
*/

	umask(0);

#ifndef OS_WINDOWS
	int fd_out;
	if((fd_out=open("/dev/null",O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR))==-1)
	{
		exit(1);
	}
	if(dup2(fd_out,STDERR_FILENO) == -1)
	{
		exit(1);
	}
#endif
}
// 注册消息
bool CProcessRunHelper::RegisteSignal()
{
//	signal(SIGUSR2, &CCmdProcessor::ProcessSignal);	
	if (SIG_ERR == ACE_OS::signal(SIGTERM, &CProcessRunHelper::ProcessExitSignal) )   //kill
	{
		printf("signal SIGTERM err\n");
		return false;
	}
	if (SIG_ERR == ACE_OS::signal(SIGINT, &CProcessRunHelper::ProcessExitSignal) )   //kill
	{
		printf("signal SIGTERM err\n");
		return false;
	}

	if(SIG_ERR ==ACE_OS::signal(SIGPIPE,  &CProcessRunHelper::ProcessExitSignal) )	// socket连接断产生的异常
	{
		printf("signal sigpipe err\n");
		return false;
	}

	return true;
}

// 处理系统消息
void CProcessRunHelper::ProcessExitSignal(int nSig)
{
//	printf("RECV signal: id = %d\n", nSig );
	sigset_t  newmask,oldmask;
	ACE_OS::sigemptyset(&newmask);
	ACE_OS::sigaddset(&newmask, nSig);
	ACE_OS::sigprocmask(SIG_BLOCK, &newmask, &oldmask );
	switch(nSig)
	{
	case SIGTERM:
	case SIGINT:
		m_bRunFlag = false;
		break;
	}

	ACE_OS::sigprocmask(SIG_SETMASK, &oldmask, NULL);
}


bool CProcessRunHelper::GetNameInfo(char* argv0)
{
	char* pPos = strrchr(argv0, FILENAME_SEPERATOR);
	pPos = (NULL==pPos) ? argv0 : pPos+1;
	strcpy(m_stOpt.pszSelfName, pPos);

	strcpy(m_stOpt.pszPathName, CPF::GetModulePathName());

	// 缺省的锁名字就是进程名字，如果有PID，则在加上PID
	sprintf(m_stOpt.pszMutexName, "mutex_%s", m_stOpt.pszSelfName);

	return true;
}


int CProcessRunHelper::TryAcquire()
{
	if (NULL != m_pMutex)	return -1;

	m_pMutex = new ACE_Process_Mutex(m_stOpt.pszMutexName);
	if (NULL == m_pMutex)	return -1;

	if (0 == m_pMutex->tryacquire())	return 0;

	delete m_pMutex;
	m_pMutex = NULL;

	return -1;
}


//////////////////////////////////////////////////////////////////////////
bool CProcessRunHelper::Create_Event(const char *strCmdLine )
{
	if (!strCmdLine || ACE_OS::strlen(strCmdLine)<5 )
		return false;

	char monitor_event_name[256] = {0};
	char stop_event_name[256] = {0};
	int  monitor_event_interval = 0;
	int  stop_event_interval = 0;

	int nrtn = sscanf(strCmdLine, "%s %s %d %d",
		monitor_event_name,stop_event_name,
		&monitor_event_interval,&stop_event_interval );

	m_pMonitorEvent = Create_one_Event(monitor_event_name);
	m_pStopEvent = Create_one_Event(stop_event_name);

	if (!m_pMonitorEvent || !m_pStopEvent)
	{
		Delete_Event();
		return false;
	}

	return true;
}

void CProcessRunHelper::Delete_Event( )
{
	if (m_pMonitorEvent )
	{
		MY_EVENT::event_destroy(m_pMonitorEvent);
		delete m_pMonitorEvent;
	}
	if (m_pStopEvent)
	{
		MY_EVENT::event_destroy(m_pStopEvent);
		delete m_pStopEvent;
	}

	m_pMonitorEvent = NULL;
	m_pStopEvent = NULL;
}
my_event_t * CProcessRunHelper::Create_one_Event(const char *event_name )
{
//	m_bStopEvent = false;

	if( event_name && strlen(event_name) > 0 )
	{
		my_event_t * event = new my_event_t;

		if( -1 == MY_EVENT::event_init_for_server(event,true,0,0,event_name) )
		{
			printf("CProcessRunHelper: [%s] 创建失败\n", event_name);
			delete event;
			return NULL;
		}
		else
		{
			printf("CProcessRunHelper: [%s] 创建成功!\n\n",event_name);
			return event;
		}
	}
	else
	{
		printf("CProcessRunHelper: event的名称为空，没有创建!\n\n");
		return NULL;
	}

	return NULL;
}

void CProcessRunHelper::OnTimer()
{
	SetMonitonEvent();

	CheckStopEvent();
}

void CProcessRunHelper::SetMonitonEvent()
{
	if (m_pMonitorEvent)
	{
		MY_EVENT::event_signal(m_pMonitorEvent);
	}
}

int CProcessRunHelper::CheckStopEvent()
{
	if( !m_pStopEvent )
	{
		return -1;
	}

	ACE_Time_Value time_wait(0,0);

	int nRet = MY_EVENT::event_timedwait(m_pStopEvent, &time_wait);

	if( 0 == nRet )
	{
		MY_EVENT::event_destroy(m_pStopEvent);

		delete m_pStopEvent;

		m_pStopEvent = NULL;
		
		m_bRunFlag = false;
//		m_bStopEvent = true;

		return 0;
	}

	return -1;
}

