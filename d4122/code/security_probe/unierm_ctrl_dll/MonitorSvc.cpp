//////////////////////////////////////////////////////////////////////////
//MonitorSvc.cpp

#include "stdafx.h"
#include "MonitorSvc.h"
#include "ace/Log_Msg.h"
#include "cpf/path_tools.h"

#include "MonitorManagerConfig.h" //service specific config
#include "ProbeServant.h"
#include "MonitorManager.h"
#include "cpf/other_tools.h"
#include "SEProgramCtrl.h"
#include "RestartTask.h"
#include "ace/High_Res_Timer.h"
//#include "LMB_serial_class.h"
//#include "ProbeSysInfo.h"
#include "config_center.h"

//#include "EZInclude.h" // add by xumx 2016-9-21

#ifndef MPLAT_INI_FILE_DIR
#define MPLAT_INI_FILE_DIR	ACE_TEXT("ini")
#endif

#define MPLAT_LOG_FILE_DIR	ACE_TEXT("log")

MonitorSvc::MonitorSvc(BOOL bDebug)
:m_bDebug(bDebug)
{	
	CPF::init();

	m_TimeId = -1;

	m_pMonitorMgr = NULL;
	m_pProbeNetCom = NULL;

	m_pSEProgramCtrl = NULL;

	m_enable_reboot = 1;
	m_nRebootPeriodDay = 3;

	m_nRebootHour = 1;
	m_nRebootMin = 0;
	m_nRebootSec = 0;
	m_nRebootTimeout_Param = 180;

	m_enable_poweroff = 0;

	m_nPoweroffHour = 0;
	m_nPoweroffMin = 0;
	m_nPoweroffSec = 0;

	m_timeback_interval = 0;
	m_timeback_lasttime = 0;

	return;
}

MonitorSvc::~MonitorSvc(void)
{
	if( m_pSEProgramCtrl )
	{
		delete m_pSEProgramCtrl;
		m_pSEProgramCtrl = NULL;
	}
	
	ConfigCenter_UnInitialize();

	CPF::fini();

	return;
}

int MonitorSvc::handle_timeout(const ACE_Time_Value &current_time, 
						       const void *act /* = 0 */)
{
	int timerid = (int)act;

	if (timerid == 1 )
	{
		if( m_pMonitorMgr )
		{
			m_pMonitorMgr->OnTimer(current_time);
		}

#ifdef OS_WINDOWS
		static int nums = 0;

		if( ++nums >= 60*10 )//10分钟检查一次
		{
			CPF::DeleteDirectory("C:\\WINDOWS\\PCHealth\\ErrorRep\\UserDumps",false);

			nums = 0;
		}
#endif

		if( m_enable_reboot )
		{
			CheckToReboot();
		}

		if( m_enable_poweroff )
		{
			CheckToPowerOff();
		}

		// 检查时间是否像后跳变
		if (	(m_timeback_interval > 0 )
			&&	m_timeback_program.size()
			)
		{
			time_t cur_time;
			time(&cur_time);

			if ( (cur_time+m_timeback_interval)<=m_timeback_lasttime)
			{

				MY_ACE_DEBUG(&m_Log, 
					//(LM_INFO, ACE_TEXT("发现时间像后跳变 %d秒\n"), m_timeback_lasttime-cur_time));
					(LM_INFO, ACE_TEXT("[Err][%D]found local time back off %d s.\n"), m_timeback_lasttime-cur_time));

				for (int i=0; i<(int)m_timeback_program.size(); ++i)
				{
	 				m_pMonitorMgr->Restart(m_timeback_program[i].c_str());
					MY_ACE_DEBUG(&m_Log, 
						//(LM_INFO, ACE_TEXT("\t 因为时间回跳，程序【%s】需要重新启动\n"), m_timeback_program[i].c_str()));
						(LM_INFO, ACE_TEXT("[Err][%D]\"%s\" restarted because of local time back off.\n"), m_timeback_program[i].c_str()));
				}
			}

			m_timeback_lasttime = cur_time;
		}

	}
	
	return 0;
}

//some init work 
int MonitorSvc::open()
{
	ACE_TCHAR log_filename[MAX_PATH];

	CPF::GetModuleOtherFileName(log_filename,sizeof(log_filename), "log","mon_service.log");

	//删除30天前的日志
	CPF::DeleteTimerLogFile(CPF::GetModuleFullFileName("log"),"mon_service_","log",30*24*3600);
	
	m_Log.init_oneday_log(log_filename,false,true);

	MY_ACE_DEBUG(&m_Log,
		(LM_ERROR,
		ACE_TEXT("\n\n\n###############################################################\n")
		));
	char *pt_version = f_get_system_version(); // add by xumx, 2016-12-12
	if(pt_version != NULL)
	{
		MY_ACE_DEBUG(&m_Log,
		(LM_ERROR,
		ACE_TEXT("[Info][%D]software version: %s\n"),pt_version
		));
	}
	MY_ACE_DEBUG(&m_Log,
		(LM_ERROR,
		ACE_TEXT("###############################################################\n")
		));

	MY_ACE_DEBUG(&m_Log,
		(LM_ERROR,
		ACE_TEXT("[Info][%D]MonitorSvc::open()\n")
		));

	m_pSEProgramCtrl = new CSEProgramCtrl;

	return 0;
}

int MonitorSvc::start_svc()
{
	ConfigCenter_Initialize();

	ReadRebootInfo();

	if (this->open() != 0)
	{
		return -1;
	}

	LoadMonitorTask();

	m_pSEProgramCtrl->init(&m_Log,m_pMonitorMgr,this);

	if( LoadProbeNetcom() )
	{
		MY_ACE_DEBUG(&m_Log, 
			//(LM_INFO, ACE_TEXT("ProbeNetcom 初始化成功，可以通过socket接收命令!\n")));
			(LM_INFO, ACE_TEXT("[Info][%D]ProbeNetcom init success, socket communication is ready.\n")));
	}
	else
	{
		MY_ACE_DEBUG(&m_Log, 
			(LM_INFO, ACE_TEXT("[Err][%D]ProbeNetcom init failed.\n")));
	}

	m_ATimer.activate();

//	const ACE_Time_Value currTv = ACE_OS::gettimeofday();
	ACE_Time_Value interval = ACE_Time_Value(1,0);

	m_ATimer.timer_queue()->gettimeofday(ACE_High_Res_Timer::gettimeofday_hr);
	const ACE_Time_Value currTv = ACE_High_Res_Timer::gettimeofday_hr();

	m_ATimer.schedule(this, 
		(void *)1, 
		currTv + ACE_Time_Value(1L),
		interval);

	/*m_ATimer.schedule(this, 
		(void *)2, 
		currTv + ACE_Time_Value(5L),
		ACE_Time_Value(5L));*/
   
   return 0;
}

int MonitorSvc::stop_svc()
{
	MY_ACE_DEBUG(&m_Log, 
		(LM_DEBUG, ACE_TEXT("[Info][%D]stop_svc()!!\n")));

	m_ATimer.stop_and_remove_all_timer();

	UnLoadMonitorTask();
	UnLoadProbeNetcom();
	
	return 0;
}

BOOL MonitorSvc::LoadProbeNetcom()
{
	ACE_TCHAR conf_filename[MAX_PATH];
	CPF::GetModuleOtherFileName(conf_filename, MAX_PATH,
		MPLAT_INI_FILE_DIR,
		"ProbeService.xml");//hard coded


	CTinyXmlEx xml_reader;
	int nRet = xml_reader.open(conf_filename);
	if(nRet!=0) 
	{
		MY_ACE_DEBUG(&m_Log, 
			(LM_INFO, ACE_TEXT("[Info][%D]can not open xmlfile : %s\n"),conf_filename));

		return false;
	}

	int listen_port = 86;
	int servant_id = 1;
	int multi_thread = 0;
	int max_client_nums = 20;

	TiXmlNode *pNetComNode = xml_reader.GetRootNode()->FirstChildElement("net_com");

	if( NULL == pNetComNode )
	{
		MY_ACE_DEBUG(&m_Log, 
			(LM_INFO, ACE_TEXT("[Err][%D]no net_com info in xmlfile,we will use default:port=%d,servant_id=%d\n"),
			listen_port,servant_id));
	}
	else
	{
		if( !xml_reader.GetValueLikeIni(pNetComNode,"listen_port",listen_port) )
		{
			MY_ACE_DEBUG(&m_Log, 
				(LM_INFO, ACE_TEXT("[Err][%D]no listen_port info in net_com in xmlfile,we will use default port=%d\n"),listen_port));
		}

		if( !xml_reader.GetValueLikeIni(pNetComNode,"servant_id",servant_id) )
		{
			MY_ACE_DEBUG(&m_Log, 
				(LM_INFO, ACE_TEXT("[Err][%D]no servant_id info in net_com in xmlfile,we will use default servant_id=%d\n"),servant_id));
		}

		if( !xml_reader.GetValueLikeIni(pNetComNode,"multi_thread",multi_thread) )
		{
			MY_ACE_DEBUG(&m_Log, 
				(LM_INFO, ACE_TEXT("[Err][%D]no multi_thread info in net_com in xmlfile,we will use default multithread=%d\n"),multi_thread));
		}

		if( !xml_reader.GetValueLikeIni(pNetComNode,"max_client_nums",max_client_nums) )
		{
			MY_ACE_DEBUG(&m_Log, 
				(LM_INFO, ACE_TEXT("[Err][%D]no max_client_nums info in net_com in xmlfile,we will use default max_client_nums=%d\n"),max_client_nums));
		}
	}

	m_pProbeNetCom = new CProbeNetCom;

	if( !m_pProbeNetCom )
		return false;

	ACE_Time_Value recv_timeout(3);
	ACE_Time_Value send_timeout(3);

	if( !m_pProbeNetCom->init(listen_port
		,servant_id,multi_thread
		,max_client_nums
		,&recv_timeout
		,&send_timeout) )
	{
		return false;
	}

	m_pProbeNetCom->set_param(&m_Log,
		&m_pSEProgramCtrl->m_probe_cmd,
		&m_pSEProgramCtrl->m_config_param);

#ifdef _DEBUG
	//测试客户端发出的请求！这行代码一直保持在这里，
	//如果不测试，请在GetServant()->test_request()函数中修改！
	m_pProbeNetCom->GetServant()->test_request();
#endif//_DEBUG

	m_pProbeNetCom->start();

	return true;
}

void MonitorSvc::UnLoadProbeNetcom()
{
	if( m_pProbeNetCom )
	{
		m_pProbeNetCom->stop();
		m_pProbeNetCom->close();
		delete m_pProbeNetCom;
		m_pProbeNetCom = NULL;
	}

	return;
}

BOOL MonitorSvc::LoadMonitorTask()
{
	ACE_TCHAR conf_filename[MAX_PATH];
	CPF::GetModuleOtherFileName(conf_filename, MAX_PATH,
		MPLAT_INI_FILE_DIR,
		"ProbeService.xml");//hard coded

	CMonitorManagerConfig configFile;
	int nRet = configFile.open(conf_filename);
	if(nRet!=0) 
		return false;

	std::vector<MONITOR_TASK> aMonitorTask;
	nRet=configFile.GetMonitorTasks(aMonitorTask);

	configFile.close();

	if(nRet!=0 || aMonitorTask.size() == 0 )
	{
		MY_ACE_DEBUG(&m_Log, 
			(LM_DEBUG, ACE_TEXT("[Err][%D]no monitor_task loaded!!\n")));

		return false;
	}

	m_pMonitorMgr = new CMonitorManager(m_bDebug,&m_Log);

	m_pMonitorMgr->AddMonitorTasks(aMonitorTask);

	return true;
}

void MonitorSvc::UnLoadMonitorTask()
{
	if( m_pMonitorMgr )
	{
		m_pMonitorMgr->ReleaseAll();

		delete m_pMonitorMgr;
		m_pMonitorMgr = NULL;

	}
}


void MonitorSvc::ReadRebootInfo()
{
	std::string conf_filename = ConfigCenter_Obj()->GetProbeFullConfigName("unierm_ctrl.xml");

	CTinyXmlEx xml_reader;

	if( 0 != xml_reader.open_for_readonly(conf_filename.c_str()) )
	{
		return;
	}

	TiXmlNode * common_node = 
		xml_reader.GetRootNode()->FirstChildElement("common");

	if( !common_node )
		return;

	{
		TiXmlNode * timeback_node = common_node->FirstChildElement("timeback");

		if( timeback_node )
		{
			xml_reader.GetAttr(timeback_node, "interval", m_timeback_interval);

			m_timeback_program.clear();

			for (TiXmlNode* pItem = timeback_node->FirstChildElement("item");
							pItem;
							pItem = pItem->NextSiblingElement("item"))
			{
				std::string str_os_type;
				xml_reader.GetAttr(pItem, "os", str_os_type);

#ifdef OS_WINDOWS
				if (stricmp(str_os_type.c_str(), "windows") != 0)
				{
					continue;
				}
#else
				if (stricmp(str_os_type.c_str(), "linux") != 0)
				{
					continue;
				}
#endif

				std::string prog_name;

				if (xml_reader.GetValue(pItem, prog_name) )
				{
					m_timeback_program.push_back(prog_name);
				}
				
			}

		}
	}

	{
		TiXmlNode * restart_node = common_node->FirstChildElement("reboot");

		if( restart_node )
		{
			xml_reader.GetAttr(restart_node,"enable",m_enable_reboot);

			xml_reader.GetAttr(restart_node,"period_day",m_nRebootPeriodDay);

			m_nRebootTimeout_Param = 180;
			xml_reader.GetAttr(restart_node,"timeout_param",m_nRebootTimeout_Param);

			std::string str_time;
			xml_reader.GetAttr(restart_node,"exe_time",str_time);

			sscanf(str_time.c_str(),"%02d:%02d:%02d",
				&m_nRebootHour,&m_nRebootMin,&m_nRebootSec);
		}
	}

	{
		TiXmlNode * poweroff_node = common_node->FirstChildElement("poweroff");

		if( poweroff_node )
		{
			xml_reader.GetAttr(poweroff_node,"enable",m_enable_poweroff);

			std::string str_time;
			xml_reader.GetAttr(poweroff_node,"exe_time",str_time);

			sscanf(str_time.c_str(),"%02d:%02d:%02d",
				&m_nPoweroffHour,&m_nPoweroffMin,&m_nPoweroffSec);
		}
	}

	return;
}

BOOL MonitorSvc::WriteRebootInfo()
{
	std::string conf_filename = ConfigCenter_Obj()->GetProbeFullConfigName("unierm_ctrl.xml");

	CTinyXmlEx xml_writter;

	if( 0 != xml_writter.open_ex(conf_filename.c_str(),CTinyXmlEx::OPEN_MODE_OPEN_CREATE) )
	{
		return false;
	}

	if( !xml_writter.GetRootNode() )
	{
		xml_writter.InsertRootElement("unierm_ctrl");
	}

	TiXmlNode * common_node = 
		xml_writter.GetOrCreateFirstChildElement(xml_writter.GetRootNode(),"common");

	if( !common_node )
		return false;

	{
		TiXmlNode * timeback_node = xml_writter.GetOrCreateFirstChildElement(common_node, "timeback");

		if( timeback_node )
		{
			xml_writter.SetAttr(timeback_node, "interval", m_timeback_interval);

			for(int i=0; i<(int)m_timeback_program.size(); ++i)
			{
				TiXmlNode* pItem = xml_writter.InsertEndChildElement(timeback_node, "item");
				pItem->SetValue(m_timeback_program[i]);
			}
		}
	}


	{
		TiXmlNode * restart_node = 
			xml_writter.GetOrCreateFirstChildElement(common_node,"reboot");

		if( restart_node )
		{
			xml_writter.SetAttr(restart_node,"enable",m_enable_reboot);

			xml_writter.SetAttr(restart_node,"period_day",m_nRebootPeriodDay);

			xml_writter.SetAttr(restart_node,"timeout_param",m_nRebootTimeout_Param);

			char buf_time[32];

			sprintf(buf_time,"%02d:%02d:%02d",
				m_nRebootHour,m_nRebootMin,m_nRebootSec);

			xml_writter.SetAttr(restart_node,"exe_time",buf_time);
		}
	}

	{
		TiXmlNode * poweroff_node = 
			xml_writter.GetOrCreateFirstChildElement(common_node,"poweroff");

		if( poweroff_node )
		{
			xml_writter.SetAttr(poweroff_node,"enable",m_enable_poweroff);

			char buf_time[32];

			sprintf(buf_time,"%02d:%02d:%02d",
				m_nPoweroffHour,m_nPoweroffMin,m_nPoweroffSec);

			xml_writter.SetAttr(poweroff_node,"exe_time",buf_time);
		}
	}

	xml_writter.savefile();

	xml_writter.close();

	return true;
}

int CmpTime1(const struct tm * ptm,
			int nRebootHour,
			int	nRebootMin,
			int nRebootSec )
{
	if( ptm->tm_hour < nRebootHour )
		return -1;

	if( ptm->tm_hour > nRebootHour )
		return 1;

	if( ptm->tm_min < nRebootMin )
		return -1;

	if( ptm->tm_min > nRebootMin )
		return 1;

	if( ptm->tm_sec < nRebootSec )
		return -1;

	if( ptm->tm_sec > nRebootSec )
		return 1;

	return 0;
}

BOOL CmpTime2(const struct tm * ptm,
			 int nRebootHour,
			 int nRebootMin,
			 int nRebootSec,
			 int timeout_sec)
{
	int ncmp_value = CmpTime1(ptm,nRebootHour,nRebootMin,nRebootSec);

	if( ncmp_value == 0 )
		return true;

	if( ncmp_value < 0 )
		return false;

	int next_hour = nRebootHour;
	int next_min = nRebootMin;
	int next_secode = nRebootSec + timeout_sec;

	if( next_secode >= 60 )
	{
		next_min += next_secode/60;
		next_secode = next_secode%60;

		if( next_min >= 60 )
		{
			next_hour += next_min/60;
			next_min = next_min%60;
		}
	}

	if( CmpTime1(ptm,next_hour,next_min,next_secode) <= 0 )
	{
		return true;
	}

	return false;
}


bool MonitorSvc::CheckToReboot()
{
	if( m_nRebootPeriodDay == 0 
		|| GetTickCount() < (DWORD)( (m_nRebootPeriodDay * 24 * 3600 - 2*m_nRebootTimeout_Param)*1000 ) )
	{
		return false;
	}
	
	static bool brebooting = false;

	//只执行一次
	if( brebooting )
	{
		return false;
	}

	time_t cur_temp_time = 0;

	time(&cur_temp_time);

	struct tm * ptm = localtime(&cur_temp_time);

	if( CmpTime2(ptm,m_nRebootHour,m_nRebootMin,m_nRebootSec,m_nRebootTimeout_Param) )	
	{//执行重新启动

		MY_ACE_DEBUG(&m_Log, 
			(LM_DEBUG, ACE_TEXT("[Info][%D]period reboot by monito_service begin!!\n")));

		brebooting = true;

		m_pMonitorMgr->StopAll(1);

		CRestartTask * restart_task = new CRestartTask;

		restart_task->sleep_and_do(20*1000,1);

		MY_ACE_DEBUG(&m_Log, 
			(LM_DEBUG, ACE_TEXT("[Info][%D]period reboot by monito_service end!!\n")));

		return true;
	}

	return false;		
}

bool MonitorSvc::CheckToPowerOff()
{
	static bool bpoweroffing = false;

	//只执行一次
	if( bpoweroffing )
		return false;

	const int time_out_sec = 5*60;

	//机器的启动时间，应该足够长，防止关机之后，重新启动，很快又要关机
	DWORD dwCount = GetTickCount();

	if( dwCount < 2 * time_out_sec * 1000 )
	{
		return false;
	}

	time_t cur_temp_time = 0;

	time(&cur_temp_time);

	struct tm * ptm = localtime(&cur_temp_time);

	if( CmpTime2(ptm,m_nPoweroffHour,m_nPoweroffMin,m_nPoweroffSec,time_out_sec) )	
	{//执行关机

		bpoweroffing = true;

		m_pMonitorMgr->StopAll(1);

		CRestartTask * restart_task = new CRestartTask;

		restart_task->sleep_and_do(20*1000,2);

		MY_ACE_DEBUG(&m_Log, 
			(LM_DEBUG, ACE_TEXT("[Info][%D]period power off by monito_service!!\n")));

		return true;
	}

	return false;		
}


void MonitorSvc::Reboot(int wait_second)
{
	m_pMonitorMgr->StopAll(1);

	CRestartTask * restart_task = new CRestartTask;

	restart_task->sleep_and_do(wait_second*1000,1);

	MY_ACE_DEBUG(&m_Log, 
		(LM_DEBUG, ACE_TEXT("[Info][%D]test reboot by ui!!\n")));

	return;
}

void MonitorSvc::GetMonitorTaskInfo(std::vector<MONITOR_TASK>& vTask)
{
	vTask.clear();

	if(m_pMonitorMgr)
	{
		m_pMonitorMgr->GetMonitorTaskInfo(vTask);
	}

	return;
}

int MonitorSvc::GetTaskConfigState(const char * name,int& state,BOOL bTempState)
{
	if(m_pMonitorMgr)
	{
		return m_pMonitorMgr->GetConfigState(name,state,bTempState);
	}

	return -1;
}

int MonitorSvc::GetTaskRunState(const char * name,int& state)
{
	if(m_pMonitorMgr)
	{
		return m_pMonitorMgr->GetTaskRunState(name,state);
	}

	return -1;
}

int MonitorSvc::ChangeConfigState(const char * prg_name,int state,BOOL bTempState)
{
	int nrtn = 0;

	if( m_pMonitorMgr )
	{	
		nrtn = m_pMonitorMgr->ChangeConfigState(prg_name,state,bTempState);

		if( nrtn == 0 && !bTempState )
		{
			ACE_TCHAR conf_filename[MAX_PATH];
			CPF::GetModuleOtherFileName(conf_filename, MAX_PATH,
				"ini","ProbeService.xml");//hard coded

			CMonitorManagerConfig config;
			if( config.open(conf_filename) == 0 )
			{
				config.SetMonitorTaskState(prg_name,state);
			}
		}

	}

	return nrtn;
}


void MonitorSvc::StopAllTask(BOOL bTempStat)
{
	if( m_pMonitorMgr )
	{	
		m_pMonitorMgr->StopAll(bTempStat);
	}

	return;
}

void MonitorSvc::StartAllTask(BOOL bTempStat)
{
	if( m_pMonitorMgr )
	{	
		m_pMonitorMgr->StartAll(bTempStat);
	}

	return;
}

// add by xumx, 2016-12-12
char *MonitorSvc::f_get_system_version()
{
	CTinyXmlEx xml_doc;

	// 打开文件
	BOOL result = xml_doc.open_for_readonly("/home/ts/version/version.xml");
	if(result < 0){ return(NULL); }

	// XML文件的根节点
	TiXmlNode *xml_root_node = xml_doc.GetRootNode();

	// 版本信息节点
	TiXmlNode *version_root = xml_doc.FirstChildElement(xml_root_node, "SERVICE");
	if(version_root == NULL)
	{
		xml_doc.close();
		return(NULL);
	}

	std::string version;
	xml_doc.GetValueLikeIni(version_root, "VERSION", version);
	if((version.length() == 0) || (version.length() > 3)){ xml_doc.close(); return(NULL); }

	std::string release;
	xml_doc.GetValueLikeIni(version_root, "RELEASE", release);
	if((release.length() == 0) || (release.length() > 3)){ xml_doc.close(); return(NULL); }
	
	std::string build;
	xml_doc.GetValueLikeIni(version_root, "BUILD", build);
	if((build.length() == 0) || (build.length() > 3)){ xml_doc.close(); return(NULL); }

	memset(m_version_string, 0, 64);
	sprintf(m_version_string, "%s.%s.%s", version.c_str(), release.c_str(), build.c_str());

	xml_doc.close();

	return(m_version_string);
}
//////////////////////////

