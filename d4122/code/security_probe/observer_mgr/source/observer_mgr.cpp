// observer_mgr.cpp : Defines the entry point for the DLL application.
//

//////////////////////////////////////////////////////////////////////////
//observer_mgr.cpp


#include "observer_mgr.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"
#include "cpf/ostypedef.h"
#include "cpf/IniFile.h"
#include "cpf/DynLoadModule.h"



//#ifdef _MANAGED
//#pragma managed(push, off)
//#endif
//
//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//					 )
//{
//    return TRUE;
//}
//
//
//
//#ifdef _MANAGED
//#pragma managed(pop)
//#endif


#define MPLAT_INI_FILE_DIR			ACE_TEXT("ini")
#define PROBE_CFG_FILENAME			ACE_TEXT("probe.ini")


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class OBSERVER_MGR : public CDynLoadModule<CMyBaseObserver,LPACECTSTR>
{

};

CObserverMgrFunc::CObserverMgrFunc()
{
	m_pPlatRunCtrl = NULL;
	m_pLogInstance = NULL;

	m_pObserverMgr = NULL;
}

CObserverMgrFunc::~CObserverMgrFunc()
{
	close();
}

BOOL CObserverMgrFunc::init(ACE_Log_Msg * pLogInstance,
							const char * monitor_event_name,
							size_t monitor_event_interval)
{
	CPF::init();

	m_pLogInstance = pLogInstance;

	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CObserverMgrFunc::init begin\n")));
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

	if( !m_pPlatRunCtrl->init(ACE_Time_Value(0,1000*5),m_pLogInstance,m_monitor_event_name.c_str(),(int)m_monitor_event_interval) )
	{
		delete m_pPlatRunCtrl;
		m_pPlatRunCtrl = NULL;

		MY_ACE_DEBUG(m_pLogInstance,(LM_ERROR,ACE_TEXT("[Err][%D]m_pPlatRunCtrl->init failed!\n")));

		return false;
	}

	if( LoadObservers() == false )
	{
		MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Err][%D]CObserverMgrFunc::init failed\n")));
		return false;
	}

	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CObserverMgrFunc::init ok\n")));

	return true;
}

void CObserverMgrFunc::close(void)
{
	UnLoadObservers();

	if (m_pPlatRunCtrl)
	{
		m_pPlatRunCtrl->close();
		delete m_pPlatRunCtrl;
		m_pPlatRunCtrl = NULL;
	}

	CPF::fini();

	return;
}

BOOL CObserverMgrFunc::Reinit()
{
	return true;
}

BOOL CObserverMgrFunc::LoadObservers()
{
	ACE_TCHAR path[PATH_MAX];

	if( !CPF::GetModuleOtherFileName(path,PATH_MAX,
		MPLAT_INI_FILE_DIR,
		PROBE_CFG_FILENAME) )
	{
		return false;
	}

	m_pObserverMgr = new OBSERVER_MGR;

	const ACE_TCHAR SECTION[] = ACE_TEXT("observer");

	if( m_pObserverMgr->LoadFromIniFile(path,SECTION,NULL) <= 0 )
	{
		MY_ACE_DEBUG(m_pLogInstance,(LM_ERROR,ACE_TEXT("[Err][%D]no any observers be loaded!\n")));
	}
	else
	{

		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,
			ACE_TEXT("[Info][%D]%d observer(s) be loaded!\n"),
			m_pObserverMgr->GetModuleNums())
			);
	}

	std::vector<std::pair<int,CMyBaseObserver*> >	vt_observer;

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

			std::vector<std::pair<int,CMyBaseObserver*> >::iterator it;

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

	std::vector<std::pair<int,CMyBaseObserver*> >::iterator it;

	for(it = vt_observer.begin(); it != vt_observer.end(); ++it )
	{
		m_pPlatRunCtrl->AttachObserver((*it).second);
	}

	return true;
}

void CObserverMgrFunc::UnLoadObservers()
{
	if( m_pObserverMgr )
	{
		m_pObserverMgr->Close();
		delete m_pObserverMgr;
		m_pObserverMgr = NULL;
	}

	return;
}

bool CObserverMgrFunc::StartTest(IRecvDataSource* pdataSource)
{
	if( !m_pPlatRunCtrl )
		return false;

	return m_pPlatRunCtrl->StartTest(pdataSource);
}

bool CObserverMgrFunc::StopTest()
{
	if( m_pPlatRunCtrl && !m_pPlatRunCtrl->IsStopped() )
	{
		m_pPlatRunCtrl->StopTest();
	}

	return true;
}

bool CObserverMgrFunc::PauseTest()
{
	if( !m_pPlatRunCtrl )
		return false;

	return m_pPlatRunCtrl->PauseTest();
}

bool CObserverMgrFunc::ContinueTest()
{
	if( !m_pPlatRunCtrl )
		return false;

	return m_pPlatRunCtrl->ContinueTest();
}

void CObserverMgrFunc::Reset()
{
	if( !m_pPlatRunCtrl )
		return ;

	//Ã»ÓÐÍ£Ö¹
	if( !IsStopped() )
	{
		ACE_ASSERT(false);
		return;
	}

	m_pPlatRunCtrl->Reset();
}

CObserverMgrFunc::ENState CObserverMgrFunc::GetRunState() const
{
	if( !m_pPlatRunCtrl )
		return STATE_UNINIT;

	return (CObserverMgrFunc::ENState)m_pPlatRunCtrl->GetRunState();
}

BOOL CObserverMgrFunc::IsStopped() const
{
	if( !m_pPlatRunCtrl )
		return false;

	return m_pPlatRunCtrl->IsStopped();
}


