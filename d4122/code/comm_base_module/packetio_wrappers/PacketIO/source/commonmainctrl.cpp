//////////////////////////////////////////////////////////////////////////
//CommonMainCtrl.cpp

#include "PacketIO/CommonMainCtrl.h"
#include "cpf/DynLoadModule.h"
#include "cpf/path_tools.h"
#include "ace/Log_Msg.h"
#include "fstream"
#include "ace/iosfwd.h"

class OBSERVER_MGR : public CDynLoadModule<CMyBaseObserver,LPACECTSTR>
{

};

CCommonMainCtrl::CCommonMainCtrl()
{
	m_pPlatRunCtrl = NULL;
	m_pLogInstance = NULL;

	m_pObserverMgr = NULL;

	m_monitor_event_interval = 0;

	m_bLogInstanceCreate = false;
}

CCommonMainCtrl::~CCommonMainCtrl()
{
	if( m_pPlatRunCtrl )
	{
		close();
	}
}

// 检查fc_protecter是否已经停止转发
bool CCommonMainCtrl::WaitProtecoterFree(int sec)
{
	const int interval = 2;
	static const char* event_name = "FC_PROTECT_TRANS_EVENT";

	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CCommonMainCtrl::WaitProtecoterFree\n")));

	do 
	{
		if( CPF::TestProcMutex_For_Server(event_name))
		{
			// 可以创建，对端没有运行
			MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CCommonMainCtrl::WaitProtecoterFree OK\n")));
			return true;
		}
		else
		{
			// 延时再次尝试
			if (sec-interval >= 0)
			{
				m_pPlatRunCtrl->SignalMonitorEvent();

				ACE_OS::sleep(interval);
				sec -= interval;
				MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CCommonMainCtrl::WaitProtecoterFree sleep\n")));
			}
			else
			{
				MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Err][%D]CCommonMainCtrl::WaitProtecoterFree false\n")));
				return false;
			}
		}
	} while (1);

	return true;
}

BOOL CCommonMainCtrl::init(ACE_Time_Value empty_sleep_time,
						   const char * datasourc_cfg_name,
						   const char * observer_list_cfg_name,
						   const char * observer_section_name,
						   ACE_Log_Msg * pLogInstance,
						   const char * monitor_event_name,
						   size_t monitor_event_interval,
						   int copy_drv_data)
{
	CPF::init();

	Inner_init(empty_sleep_time,pLogInstance,monitor_event_name,monitor_event_interval,copy_drv_data);

	bool bRet = WaitProtecoterFree(30);

	std::string str_observer_list_cfg_name;

	if( observer_list_cfg_name && observer_list_cfg_name[0] )
	{
		if( CPF::IsFullPathName(observer_list_cfg_name) )
		{
			str_observer_list_cfg_name = observer_list_cfg_name;
		}
		else
		{
			str_observer_list_cfg_name = CPF::GetModuleFullFileName(observer_list_cfg_name);
		}
	}
	
	if( !str_observer_list_cfg_name.empty() && observer_section_name && observer_section_name[0] )
	{
		if( LoadObserversFromCfg(str_observer_list_cfg_name.c_str(),observer_section_name) == false )
		{
			MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Err][%D]CCommonMainCtrl::init failed in LoadObserversFromCfg\n")));
			return false;
		}		
	}

	std::string str_datasourc_cfg_name;

	if( datasourc_cfg_name && datasourc_cfg_name[0] )
	{
		if( CPF::IsFullPathName(datasourc_cfg_name) )
		{
			str_datasourc_cfg_name = datasourc_cfg_name;
		}
		else
		{
			str_datasourc_cfg_name = CPF::GetModuleFullFileName(datasourc_cfg_name);
		}
	}

	if( !str_datasourc_cfg_name.empty() )
	{
		m_datasource_mgr.init(str_datasourc_cfg_name.c_str(),0,pLogInstance);

		if( !m_datasource_mgr.OpenRecvDataSource() )
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_ERROR,
				ACE_TEXT("[Err][%D]CCommonMainCtrl::init failed in m_datasource_mgr.OpenRecvDataSource!\n")
				));

			return false;
		}
	}
	
	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CCommonMainCtrl::init ok!\n")));

	return true;
}


BOOL CCommonMainCtrl::init(ACE_Time_Value empty_sleep_time,
						   ACE_Log_Msg * pLogInstance,
						   const char * monitor_event_name,
						   size_t monitor_event_interval,
						   int copy_drv_data)
{
	CPF::init();

	return Inner_init(empty_sleep_time,pLogInstance,monitor_event_name,monitor_event_interval,copy_drv_data);
}

BOOL CCommonMainCtrl::Inner_init(ACE_Time_Value empty_sleep_time,
								 ACE_Log_Msg * pLogInstance,
								 const char * monitor_event_name,
								 size_t monitor_event_interval,
								 int copy_drv_data)
{
	if( pLogInstance )
	{
		m_pLogInstance = pLogInstance;
		m_bLogInstanceCreate = false;
	}
	else
	{
		m_pLogInstance = ReadyLogMsg(0);
		m_bLogInstanceCreate = true;
	}

	if (monitor_event_name )
	{
		m_monitor_event_name = monitor_event_name;
	}
	m_monitor_event_interval = monitor_event_interval;

	m_pPlatRunCtrl = new CMyPlatRunCtrl;
	if (!m_pPlatRunCtrl)
	{
		return false;
	}

	if( !m_pPlatRunCtrl->init(empty_sleep_time,m_pLogInstance,m_monitor_event_name.c_str(),(int)m_monitor_event_interval,copy_drv_data) )
	{
		delete m_pPlatRunCtrl;
		m_pPlatRunCtrl = NULL;

		MY_ACE_DEBUG(m_pLogInstance,(LM_ERROR,ACE_TEXT("[Err][%D]m_pPlatRunCtrl->init failed!\n")));

		return false;
	}

	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CCommonMainCtrl::Inner_init ok\n")));

	return true;

}



void CCommonMainCtrl::attach_observer(CMyBaseObserver * pObserver)
{
	m_pPlatRunCtrl->AttachObserver(pObserver);

	return;
}

void CCommonMainCtrl::close(void)
{
	UnLoadObserversFromCfg();

	if (m_pPlatRunCtrl)
	{
		m_pPlatRunCtrl->close();
		delete m_pPlatRunCtrl;
		m_pPlatRunCtrl = NULL;
	}

	if( m_bLogInstanceCreate && m_pLogInstance )
	{
		delete m_pLogInstance;
		m_pLogInstance = NULL;
		m_bLogInstanceCreate = false;
	}

	CPF::fini();

	return;
}

BOOL CCommonMainCtrl::LoadObserversFromCfg(const char * observer_list_cfg_name,
										   const char * observer_section_name)
{
	m_pObserverMgr = new OBSERVER_MGR;

	int nloads = 0;

	int type = 0;

	LPACECTSTR suf = CPF::GetSufFromFileName(observer_list_cfg_name);

	if( suf )
	{
		if( 0 == stricmp(suf,"ini") )
		{
			type = 0;

			nloads = m_pObserverMgr->LoadFromIniFile(observer_list_cfg_name,
				observer_section_name,NULL);
		}
		else if( 0 == stricmp(suf,"xml") )
		{
			type = 1;

			nloads = m_pObserverMgr->LoadFromXmlFile(observer_list_cfg_name,
				observer_section_name,NULL);
		}
		else
		{
			type = 0;

			nloads = m_pObserverMgr->LoadFromIniFile(observer_list_cfg_name,
				observer_section_name,NULL);
		}
	}

	if( nloads <= 0 )
	{
		MY_ACE_DEBUG(m_pLogInstance,(LM_ERROR,ACE_TEXT("[Info][%D]no any observers be loaded!\n")));
	}
	else
	{

		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,
			ACE_TEXT("[Info][%D]%d observer(s) had been loaded!\n"),
			m_pObserverMgr->GetModuleNums())
			);
	}

	std::vector< std::pair<int,CMyBaseObserver*> >	vt_observer;

	if( 0 == type )
	{//排序
		for( int i = 0; i < m_pObserverMgr->GetModuleNums(); ++i )
		{
			int index = -1;

			if( sscanf(m_pObserverMgr->GetModuleInfo(i)->m_strParam.c_str(),"%d",&index) == 1 )
			{
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_INFO,
					ACE_TEXT("[Info][%D]observer: %s be loaded,param is: %s!\n"),
					m_pObserverMgr->GetModuleInfo(i)->m_strDllName.c_str(),
					m_pObserverMgr->GetModuleInfo(i)->m_strParam.c_str())
					);

				std::vector< std::pair<int,CMyBaseObserver*> >::iterator it;

				for(it = vt_observer.begin(); it != vt_observer.end(); ++it )
				{
					if( index <= (*it).first )
					{
						break;
					}
				}

				vt_observer.insert(it,std::make_pair(index,m_pObserverMgr->GetModuleInfo(i)->m_pInstance));
			}
			else
			{
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_INFO,
					ACE_TEXT("[Err][%D]param for observer: %s is error!,param is: %s\n"),
					m_pObserverMgr->GetModuleInfo(i)->m_strDllName.c_str(),
					m_pObserverMgr->GetModuleInfo(i)->m_strParam.c_str())
					);
			}

		}
	}
	else
	{
		for( int i = 0; i < m_pObserverMgr->GetModuleNums(); ++i )
		{
			int index = -1;

			//m_pInstance = g_pFluxControlFrameObserver after called pObserverInfo->LoadModule
			vt_observer.push_back(std::make_pair(index,m_pObserverMgr->GetModuleInfo(i)->m_pInstance));	
		}
	}

	std::vector< std::pair<int,CMyBaseObserver*> >::iterator it;

	for(it = vt_observer.begin(); it != vt_observer.end(); ++it )
	{
		if (m_pPlatRunCtrl->AttachObserver((*it).second) == false) { return(false); }
	}

	return true;
}

void CCommonMainCtrl::UnLoadObserversFromCfg()
{
	if( m_pPlatRunCtrl )
	{
		m_pPlatRunCtrl->DetachAllObserver();
	}

	if( m_pObserverMgr )
	{
		m_pObserverMgr->Close();
		delete m_pObserverMgr;
		m_pObserverMgr = NULL;
	}

	return;
}

bool CCommonMainCtrl::StartTest(IRecvDataSource* pdataSource)
{
	if( !m_pPlatRunCtrl )
		return false;

	if( pdataSource )
	{
		return m_pPlatRunCtrl->StartTest(pdataSource);
	}
	else
	{
		return m_pPlatRunCtrl->StartTest(m_datasource_mgr.GetCurRecvDataSource());
	}
}

/*add by zhongjh*/
bool CCommonMainCtrl::StartTest(IRecvDataSource* pdataSource, CSerialDataRecv* pSerialRecv)
{
	if( !m_pPlatRunCtrl )
		return false;

	return m_pPlatRunCtrl->StartTest(pdataSource, pSerialRecv);
}

bool CCommonMainCtrl::StopTest()
{
	ACE_Time_Value start_time = ACE_OS::gettimeofday();

	if( m_pPlatRunCtrl && !m_pPlatRunCtrl->IsStopped() )
	{
		m_pPlatRunCtrl->StopTest();
	}

	return true;
}

bool CCommonMainCtrl::PauseTest()
{
	if( !m_pPlatRunCtrl )
		return false;

	return m_pPlatRunCtrl->PauseTest();
}

bool CCommonMainCtrl::ContinueTest()
{
	if( !m_pPlatRunCtrl )
		return false;

	return m_pPlatRunCtrl->ContinueTest();
}
//
//void CCommonMainCtrl::Reset()
//{
//	if( !m_pPlatRunCtrl )
//		return ;
//
//	//没有停止
//	if( !IsStopped() )
//	{
//		ACE_ASSERT(false);
//		return;
//	}
//
//	m_pPlatRunCtrl->Reset();
//}

CCommonMainCtrl::ENState CCommonMainCtrl::GetRunState() const
{
	if( !m_pPlatRunCtrl )
		return STATE_UNINIT;

	return (CCommonMainCtrl::ENState)m_pPlatRunCtrl->GetRunState();
}

BOOL CCommonMainCtrl::IsStopped() const
{
	if( !m_pPlatRunCtrl )
		return false;

	return m_pPlatRunCtrl->IsStopped();
}

BOOL CCommonMainCtrl::GetRunInfo(CMyPlatRunCtrl::PlatRun_Info& runinfo) const
{
	if( m_pPlatRunCtrl )
	{
		m_pPlatRunCtrl->GetRunInfo(runinfo);

		return TRUE;
	}

	return FALSE;	
}

const CMyPlatRunCtrl::PlatRun_Info& CCommonMainCtrl::GetRunInfo() const
{
	return m_pPlatRunCtrl->GetRunInfo();
}

PACKET_LEN_TYPE CCommonMainCtrl::GetLastPacketLenType() const
{
	if( m_pPlatRunCtrl )
	{
		return m_pPlatRunCtrl->GetLastPacketLenType();
	}

	return PACKET_NO_MORE;
}

int CCommonMainCtrl::GetDataSourceConnectState()
{
	IRecvDataSource * pRecvDataSource = 
		m_datasource_mgr.GetCurRecvDataSource();

	if( pRecvDataSource )
	{
		return pRecvDataSource->GetConnectState();
	}

	return 0;
}

ACE_Log_Msg *CCommonMainCtrl::ReadyLogMsg(unsigned int log_interval)
{
	const char * logpath_name = CPF::GetModuleFullFileName("log");

	CPF::CreateFullDirecory(logpath_name);

	const char * pFullName = CPF::GetModuleFileName();

	const char * pFileName = CPF::GetFileNameFromFullName(pFullName);

	const char * ptempName = CPF::JoinPathToPath(logpath_name,pFileName);

	std::string strLogName;

	CPF::ReplaceSufFromFileName(ptempName,"log",strLogName);

	std::ofstream * pLogOutFile = new std::ofstream(strLogName.c_str(),std::ios::out|std::ios::app);

	if(!pLogOutFile->good())
	{
		printf("can not open log file: '%s'\n", strLogName.c_str());
	}
	else
	{
		printf("open log file: '%s' success\n", strLogName.c_str());
	}

	ACE_Log_Msg * pLogInstance = new ACE_Log_Msg;

	if( pLogInstance )
	{
		pLogInstance->msg_ostream(pLogOutFile,1);

		pLogInstance->set_flags(ACE_Log_Msg::OSTREAM);
		pLogInstance->clr_flags(ACE_Log_Msg::STDERR);
	}

	return pLogInstance;
}

