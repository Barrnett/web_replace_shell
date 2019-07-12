//////////////////////////////////////////////////////////////////////////
//SEProgramCtrl.cpp
#include "stdafx.h"

#include "SEProgramCtrl.h"
#include "cpf/path_tools.h"
//#include "ProbeSysInfo.h"


CSEProgramCtrl::CSEProgramCtrl(void)
{
	m_pMonitorMgr = NULL;
	m_pLogInstance = NULL;
	//m_pProbeSysInfo = NULL;

	m_pMonitorSvc = NULL;

	return;
}

CSEProgramCtrl::~CSEProgramCtrl(void)
{
#if 0
	if( m_pProbeSysInfo )
	{
		delete m_pProbeSysInfo;
		m_pProbeSysInfo = NULL;
	}
#endif
	return;
}


void CSEProgramCtrl::init(ACE_Log_Msg* pLogInstance,
						  CMonitorManager * pMonitorMgr,
						  MonitorSvc * pMonitorSvc)
{
	m_pMonitorMgr = pMonitorMgr;
	m_pLogInstance = pLogInstance;
	m_pMonitorSvc = pMonitorSvc;

	#if 0
	m_pProbeSysInfo = new CProbeSysInfo;
	m_pProbeSysInfo->init(NULL);
	#endif

	ReadProbeServiceConfigParam();

	

	//if( m_config_param.nSupportRestoreModule & PROBESERVICE_CONFIG_PARAM::RESTORE_PROBE )
	{
		m_probe_cmd.init_ex(m_pLogInstance,
			m_pMonitorMgr,
			m_pMonitorSvc,
			&m_config_param,			
			/*m_pProbeSysInfo*/NULL);

		MY_ACE_DEBUG(m_pLogInstance, 
			(LM_INFO, ACE_TEXT("[Info][%D]m_probe_cmd init ok\n")));

	}

	

	//just for test
	/*while( 1 )
	{
		RefreshSysInfo();

	ACE_OS::sleep(2);
	}*/
	
	return;
}


void CSEProgramCtrl::RefreshSysInfo()
{
	#if 0
	if( m_pProbeSysInfo )
	{
		m_pProbeSysInfo->Refresh();
	}
	#endif

	return;
}

BOOL CSEProgramCtrl::ReadProbeServiceConfigParam()
{
	ACE_TCHAR conf_filename[MAX_PATH];
	CPF::GetModuleOtherFileName(conf_filename, MAX_PATH,
		"ini","ProbeService.xml");//hard coded

	CMonitorManagerConfig xml_reader;

	int nRet = xml_reader.open(conf_filename);

	if(nRet!=0) 
	{
		MY_ACE_DEBUG(m_pLogInstance, 
			(LM_INFO, ACE_TEXT("[Err][%D]can not open xmlfile : %s\n"),conf_filename));

		return false;
	}

	xml_reader.ReadProbeServiceConfigParam(m_config_param);

	return true;

}








