// FCServerDlg.cpp : implementation file
//

//#include "stdafx.h"
//#include "FCServer.h"
#include "FCServerDlg.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"
#include "config_fc_server_xml.h"
#include "config_center.h"



CFCServerDlg::CFCServerDlg(BOOL b_restore_orig_prg, const char * pCommandLine)
	: m_strTimeSpend(_T(""))
	, m_b_restore_orig_prg(b_restore_orig_prg)
	, m_strCmdLine(pCommandLine)
	, m_pStopEvent(NULL)
{
	m_iSecondCount = 0;
	m_bStartTimer = false;
	m_bStopEvent = false;

}


// CFCServerDlg message handlers

BOOL CFCServerDlg::OnInitDialog()
{
	CPF::CreateFullDirecory(CPF::GetModuleFullFileName("log"));

	char log_filename[MAX_PATH];

	CPF::GetModuleOtherFileName(log_filename, sizeof(log_filename), "log", "fc_server.log");

	//删除30天前的日志
	CPF::DeleteTimerLogFile(CPF::GetModuleFullFileName("log"),"fc_server_","log",30*24*3600);
	
	m_log_file.init_oneday_log(log_filename,false,true);

	bool bRtn = FCServerConfig_Initialize(&m_log_file);
	if (!bRtn)
	{
		MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Err][%D]file not found :fc_server.xml.\n")));
		return FALSE;
	}


	MY_ACE_DEBUG(&m_log_file,
		(LM_ERROR,
		ACE_TEXT("\n\n\n###############################################################\n")
		));

	char *pt_version = f_get_system_version();
	if(pt_version != NULL)
	{
		MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Info][%D]software version: %s\n"), pt_version));
	}

	MY_ACE_DEBUG(&m_log_file,
		(LM_ERROR,
		ACE_TEXT("###############################################################\n")
		));

	MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Info][%D]CFCServerDlg::OnInitDialog()\n")));
	
	{
		if( m_strCmdLine.empty() )
		{
			m_strCmdLine = FCServerConfig_Obj()->exe_m_strCmdLine;
		}	

		if( m_b_restore_orig_prg )
		{
			m_fcServer.init_for_restor_orig(&m_log_file);

		}
		else
		{
			char monitor_event_name[256] = {0};
			char stop_event_name[256] = {0};
			int  monitor_event_interval = 0;
			int  stop_event_interval = 0;

			int nrtn = sscanf(m_strCmdLine.c_str(),"%s %s %d %d",
				monitor_event_name,stop_event_name,
				&monitor_event_interval,&stop_event_interval );

			BOOL ret = m_fcServer.init_normal(&m_log_file,monitor_event_name,monitor_event_interval);
			if (FALSE == ret)
			{
				MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Err][%D]m_fcServer.init_normal failed!\n")));
				return FALSE;
			}
			
			m_pStopEvent = Create_Stop_Event(&m_log_file,stop_event_name);
		}
	}
	
	MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Info][%D]CFCServerDlg::OnInitDialog ok!\n")));

	m_dwBegingTickcount = GetTickCount();

//	SetTimer(2,2*1000,NULL);
//	SetTimer(3,5*60*1000,NULL);

	m_iSecondCount = 0;
	m_bStartTimer = true;
	m_bStopEvent = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFCServerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	return;
}


void CFCServerDlg::OnBnClickedButtonStart()
{
	if( m_b_restore_orig_prg )
		return;

	m_dwBegingTickcount = GetTickCount();

	m_fcServer.Start();
}

void CFCServerDlg::OnBnClickedButtonStop()
{
	if( m_b_restore_orig_prg )
		return;

	m_fcServer.Stop();
}

void CFCServerDlg::OnBnClickedButtonStopAndExit()
{
	OnBnClickedButtonStop();

	OnBnClickedOk();
}


void CFCServerDlg::OnBnClickedOk()
{
	m_fcServer.fini();

	m_log_file.fini();

	if( m_pStopEvent )
	{
		MY_EVENT::event_destroy(m_pStopEvent);
		delete m_pStopEvent;
		m_pStopEvent = NULL;
	}
	FCServerConfig_UnInitialize();
}

void CFCServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == 2 )
	{
		if( CheckStopEvent() == 0 )
		{
			return;
		}

		GetRunTimeInfo();
//		UpdateData(false);
	}
	else if( nIDEvent == 3 )
	{		
	}
	return;
}

void CFCServerDlg::GetRunTimeInfo()
{
	DWORD dwDiffSec = (GetTickCount()-m_dwBegingTickcount)/1000;

	m_strTimeSpend = CPF::GetTimeLastInfo(dwDiffSec).c_str();

}
my_event_t * CFCServerDlg::Create_Stop_Event(ACE_Log_File_Msg * loginstance,const char *stop_event_name )
{
	if( stop_event_name && strlen(stop_event_name) > 0 )
	{
		my_event_t * event = new my_event_t;

		if( -1 == MY_EVENT::event_init(event,true,0,0,stop_event_name) )
		{
			MY_ACE_DEBUG(loginstance,
				(LM_INFO,
				ACE_TEXT("[Err][%D]create stop_event '%s' failed.\n"), stop_event_name
				));

			printf("create stop_event '%s' failed.\n\n", stop_event_name);
			delete event;
			return NULL;
		}
		else
		{
			MY_ACE_DEBUG(loginstance,
				(LM_INFO,
				ACE_TEXT("[Info][%D]create stop_event '%s' success.\n"),stop_event_name
				));

			printf("create stop_event '%s' success.\n\n",stop_event_name);

			return event;
		}
	}
	else
	{
		MY_ACE_DEBUG(loginstance,
			(LM_INFO,
			ACE_TEXT("[Info][%D]stop_event is empty.\n")
			));

		printf("stop_event is empty\n\n");

		return NULL;
	}

	return NULL;
}

int CFCServerDlg::CheckStopEvent()
{
	if( !m_pStopEvent )
	{
		return -1;
	}

	ACE_Time_Value time_wait(0,0);

	int nRet = MY_EVENT::event_timedwait(m_pStopEvent, &time_wait);

	if( 0 == nRet )
	{
	//	KillTimer(1);

		MY_ACE_DEBUG(&m_log_file,
			(LM_INFO,
			ACE_TEXT("[Info][%D]recv stop event,and exit this program!\n")
			));

		CString strTitle = "";

//		GetWindowText(strTitle);

		strTitle += "[Info][%D](recv stop event,and exit this program!)";

		SetWindowText(strTitle);

		m_fcServer.Stop();

		m_fcServer.fini();

		if( m_pStopEvent )
		{
			MY_EVENT::event_destroy(m_pStopEvent);
			delete m_pStopEvent;
			m_pStopEvent = NULL;
		}

		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("----------------------------------------------------------------\n")
			));

		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("[Info][%D]%s!!!!!!!!!!!!!!!!!!!\n"),strTitle.GetString()
			));

		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("----------------------------------------------------------------\n")
			));

		m_log_file.fini();

		m_bStopEvent = true;

//		OnOK();

		return 0;
	}

	return -1;
}

void CFCServerDlg::OneSecondCall()
{
	if (!m_bStartTimer)
		return;

	m_iSecondCount ++;
	if (m_iSecondCount % 2 == 0)
	{
		OnTimer(2);
	}
	
	return;

}

// add by xumx, 2016-12-12
char *CFCServerDlg::f_get_system_version()
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

