
// Unierm_CtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Unierm_Ctrl.h"
#include "Unierm_CtrlDlg.h"
#include "MonitorSvc.h"
#include "cpf/path_tools.h"
#include "cpf/TinyXmlEx.h"
//#include "RebootTimeCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUnierm_CtrlDlg 对话框

CUnierm_CtrlDlg::CUnierm_CtrlDlg(const char * pCommandLine)
	: m_strCmdLine(pCommandLine)
	, m_pStopEvent(NULL)
{
	m_pMonitorSvc = NULL;

	m_pStopEvent = NULL;
	m_pMonitorEvent = NULL;

	m_bStopEvent = false;
}

CUnierm_CtrlDlg::~CUnierm_CtrlDlg()
{

	return;

}
/*

BEGIN_MESSAGE_MAP(CUnierm_CtrlDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CUnierm_CtrlDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CUnierm_CtrlDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_START, &CUnierm_CtrlDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CUnierm_CtrlDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, &CUnierm_CtrlDlg::OnBnClickedButtonConfig)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ALL, &CUnierm_CtrlDlg::OnBnClickedButtonStopAll)
END_MESSAGE_MAP()
*/


// CUnierm_CtrlDlg 消息处理程序

BOOL CUnierm_CtrlDlg::OnInitDialog()
{
//	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
//	SetIcon(m_hIcon, TRUE);			// 设置大图标
//	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
//	InitListCtrl();

	m_pMonitorSvc = new MonitorSvc(true);

	if( m_pMonitorSvc )
	{
		m_pMonitorSvc->start_svc();

		m_pMonitorSvc->GetMonitorTaskInfo(m_vt_task);
	}

//	RefreshListCtrl();

	{
		CTinyXmlEx xml_reader;

		{
			char xml_file_name[MAX_PATH] = {0};

			CPF::GetModuleOtherFileName(xml_file_name,sizeof(xml_file_name),"ini","ProbeService.xml");

			if( 0 != xml_reader.open(xml_file_name) )
			{
//				MY_ACE_DEBUG(loginstance, (LM_INFO, ACE_TEXT("没有找到配置文件:ProbeService.xml") ) );
				printf("cannot find:ProbeService.xml\n");
				return FALSE;
			}
		}

		TiXmlNode * pcommon_node = xml_reader.GetRootNode()->FirstChildElement("common");

		if( !pcommon_node )
		{
			printf("file format error: ProbeService.xml\n");
//			MY_ACE_DEBUG(loginstance, (LM_INFO, ACE_TEXT("ProbeService.xml配置文件格式错误!") ) );
			return FALSE;
		}

		if( m_strCmdLine.empty() )
		{
			xml_reader.GetValueLikeIni(pcommon_node,"cmd_line",m_strCmdLine);
		}	

		{
			char monitor_event_name[256] = {0};
			char stop_event_name[256] = {0};
//			int  monitor_event_interval = 0;
			int  stop_event_interval = 0;

			int nrtn = sscanf(m_strCmdLine.c_str(),"%s %s %d %d",
				monitor_event_name,stop_event_name,
				&monitor_event_interval,&stop_event_interval );

			m_pStopEvent = Create_Stop_Event(NULL,stop_event_name);

			{
				monitor_event_interval = monitor_event_interval/3;

				if( monitor_event_interval <= 0 )
					monitor_event_interval = 3;

				if(/* monitor_event_name && */strlen(monitor_event_name) > 0 )
				{
					m_pMonitorEvent = new my_event_t;

					if( -1 == MY_EVENT::event_init(m_pMonitorEvent,1,0,0,monitor_event_name) )
					{
						delete m_pMonitorEvent;
						m_pMonitorEvent = NULL;
					}

//					SetTimer(2,monitor_event_interval*1000,NULL);
				}

			}

		}
	}

//	SetTimer(1,3000,NULL);

	m_bStartTimer = true;
	m_iSecondCount = 0;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUnierm_CtrlDlg::OnBnClickedCancel()
{
	if( m_pMonitorSvc )
	{
		m_pMonitorSvc->stop_svc();

		delete m_pMonitorSvc;
		m_pMonitorSvc = NULL;
	}

	if( m_pStopEvent )
	{
		MY_EVENT::event_destroy(m_pStopEvent);
		delete m_pStopEvent;
		m_pStopEvent = NULL;
	}

	if( m_pMonitorEvent )
	{
		MY_EVENT::event_destroy(m_pMonitorEvent);
		delete m_pMonitorEvent;
		m_pMonitorEvent = NULL;
	}

//	OnCancel();
}

void CUnierm_CtrlDlg::OnOK()
{
	return;
}

CString CUnierm_CtrlDlg::GetStatString(int nRunStat)
{
	if( nRunStat == 1 )
	{
		return CString("运行");
	}
	else if( nRunStat == 0 )
	{
		return CString("停止");
	}

	return CString("未知");
}

void CUnierm_CtrlDlg::RefreshState()
{
	for(int index = 0; index < (int)m_vt_task.size(); ++index)
	{
		int nConfigStat = -1;
		m_pMonitorSvc->GetTaskConfigState(m_vt_task[index].szProcessName,nConfigStat, 1);

		int nRunStat = -1;
		m_pMonitorSvc->GetTaskRunState(m_vt_task[index].szProcessName,nRunStat);
if (1 != nRunStat)
{
		printf("%-20s%-10s%-10s\n", 
			m_vt_task[index].szProcessName, 
			GetStatString(nConfigStat).GetString(), 
			GetStatString(nRunStat).GetString());
}
	}

	return;
}

void CUnierm_CtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == 1 )
	{
		RefreshState();

		if( CheckStopEvent() == 0 )
		{
			return;
		}
	}
	else if( nIDEvent == 2 )
	{
		if( m_pMonitorEvent )
		{
			MY_EVENT::event_signal(m_pMonitorEvent);
		}

	}


//	CDialog::OnTimer(nIDEvent);

	return;
}



my_event_t * CUnierm_CtrlDlg::Create_Stop_Event(ACE_Log_File_Msg * loginstance,const char *stop_event_name )
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
				ACE_TEXT("[Info][%D]create stop_event '%s' success.\n"), stop_event_name
				));

			printf("create stop_event '%s' success.\n\n", stop_event_name);

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

int CUnierm_CtrlDlg::CheckStopEvent()
{
	if( !m_pStopEvent )
	{
		return -1;
	}

	ACE_Time_Value time_wait(0,0);

	int nRet = MY_EVENT::event_timedwait(m_pStopEvent, &time_wait);

	if( 0 == nRet )
	{
//		KillTimer(1);

//		CString strTitle;
//		strTitle += "recv stop event,and exit this program!\n";
//		MY_ACE_DEBUG(loginstance, (LM_INFO, ACE_TEXT(strTitle) ) );
		printf("recv stop event,and exit this program!\n");


		if( m_pStopEvent )
		{
			MY_EVENT::event_destroy(m_pStopEvent);
			delete m_pStopEvent;
			m_pStopEvent = NULL;
		}

		if( m_pMonitorEvent )
		{
			MY_EVENT::event_destroy(m_pMonitorEvent);
			delete m_pMonitorEvent;
			m_pMonitorEvent = NULL;
		}

//		CDialog::EndDialog(IDOK);
	
		m_bStopEvent = true;
		return 0;
	}

	return -1;
}


void CUnierm_CtrlDlg::OneSecondCall()
{
	if (!m_bStartTimer)
		return;

	m_iSecondCount ++;
//	if (m_iSecondCount % monitor_event_interval == 0)
	if (m_iSecondCount % 10 == 0)
	{
		OnTimer(2);
	}
	if (m_iSecondCount % 3 == 0)
	{
		OnTimer(1);
	}

	return;

//	SetTimer(2,monitor_event_interval*1000,NULL);
//	SetTimer(1,3000,NULL);

}

