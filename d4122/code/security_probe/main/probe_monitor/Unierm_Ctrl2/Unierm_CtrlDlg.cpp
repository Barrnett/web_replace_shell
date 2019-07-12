
// Unierm_CtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Unierm_Ctrl.h"
#include "Unierm_CtrlDlg.h"
#include "MonitorSvc.h"
#include "cpf/path_tools.h"
#include "cpf/TinyXmlEx.h"
#include "RebootTimeCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUnierm_CtrlDlg 对话框

CUnierm_CtrlDlg::CUnierm_CtrlDlg(const char * pCommandLine,CWnd* pParent /*=NULL*/)
	: CDialog(CUnierm_CtrlDlg::IDD, pParent)
	, m_strCmdLine(pCommandLine)
	, m_pStopEvent(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pMonitorSvc = NULL;

	m_pStopEvent = NULL;
	m_pMonitorEvent = NULL;
}

CUnierm_CtrlDlg::~CUnierm_CtrlDlg()
{

	return;

}
void CUnierm_CtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PRG, m_listPrg);
}

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
	ON_BN_CLICKED(IDC_BUTTON_STOP_ALL_AND_THIS, &CUnierm_CtrlDlg::OnBnClickedButtonStopAllAndThis)
END_MESSAGE_MAP()


// CUnierm_CtrlDlg 消息处理程序

BOOL CUnierm_CtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitListCtrl();

	m_pMonitorSvc = new MonitorSvc(true);

	if( m_pMonitorSvc )
	{
		m_pMonitorSvc->start_svc();

		m_pMonitorSvc->GetMonitorTaskInfo(m_vt_task);
	}

	RefreshListCtrl();

	{
		CTinyXmlEx xml_reader;

		{
			char xml_file_name[MAX_PATH] = {0};

			CPF::GetModuleOtherFileName(xml_file_name,sizeof(xml_file_name),"ini","ProbeService.xml");

			if( 0 != xml_reader.open(xml_file_name) )
			{
				AfxMessageBox("没有找到配置文件:ProbeService.xml",MB_OK);
				EndDialog(IDCANCEL);
				return FALSE;
			}
		}

		TiXmlNode * pcommon_node = xml_reader.GetRootNode()->FirstChildElement("common");

		if( !pcommon_node )
		{
			AfxMessageBox("fc_server.xml配置文件格式错误!",MB_OK|MB_ICONERROR);
			EndDialog(IDCANCEL);
			return FALSE;
		}

		if( m_strCmdLine.empty() )
		{
			xml_reader.GetValueLikeIni(pcommon_node,"cmd_line",m_strCmdLine);
		}	

		{
			char monitor_event_name[256] = {0};
			char stop_event_name[256] = {0};
			int  monitor_event_interval = 0;
			int  stop_event_interval = 0;

			int nrtn = sscanf(m_strCmdLine.c_str(),"%s %s %d %d",
				monitor_event_name,stop_event_name,
				&monitor_event_interval,&stop_event_interval );

			m_pStopEvent = Create_Stop_Event(NULL,stop_event_name);

			{
				monitor_event_interval = monitor_event_interval/3;

				if( monitor_event_interval <= 0 )
					monitor_event_interval = 3;

				if( monitor_event_name && strlen(monitor_event_name) > 0 )
				{
					m_pMonitorEvent = new ACE_event_t;

					if( -1 == ACE_OS::event_init(m_pMonitorEvent,0,1,0,monitor_event_name) )
					{
						delete m_pMonitorEvent;
						m_pMonitorEvent = NULL;
					}

					SetTimer(2,monitor_event_interval*1000,NULL);
				}

			}

		}
	}

	SetTimer(1,3000,NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUnierm_CtrlDlg::OnBnClickedButtonRefresh()
{
	m_vt_task.clear();

	if( m_pMonitorSvc )
	{
		m_pMonitorSvc->GetMonitorTaskInfo(m_vt_task);
	}

	RefreshListCtrl();

}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUnierm_CtrlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUnierm_CtrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUnierm_CtrlDlg::OnBnClickedCancel()
{
	if( IDNO == AfxMessageBox("您确定要退出总控程序吗?",MB_YESNO|MB_ICONQUESTION) )
	{
		return;
	}

	if( m_pMonitorSvc )
	{
		m_pMonitorSvc->stop_svc();

		delete m_pMonitorSvc;
		m_pMonitorSvc = NULL;
	}

	if( m_pStopEvent )
	{
		ACE_OS::event_destroy(m_pStopEvent);
		delete m_pStopEvent;
		m_pStopEvent = NULL;
	}

	if( m_pMonitorEvent )
	{
		ACE_OS::event_destroy(m_pMonitorEvent);
		delete m_pMonitorEvent;
		m_pMonitorEvent = NULL;
	}

	OnCancel();
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

void CUnierm_CtrlDlg::InitListCtrl()
{
	m_listPrg.SetExtendedStyle(
		(m_listPrg.GetExtendedStyle() 
		| LVS_EX_FULLROWSELECT 
		| LVS_EX_GRIDLINES));

	int index = 0;

	{
		m_listPrg.InsertColumn(index++,"程序名称",LVCFMT_LEFT,160);

		m_listPrg.InsertColumn(index++,"当前运行状态",LVCFMT_LEFT,110);		
		m_listPrg.InsertColumn(index++,"配置运行状态",LVCFMT_LEFT,110);
		m_listPrg.InsertColumn(index++,"文件配置状态",LVCFMT_LEFT,110);

		m_listPrg.InsertColumn(index++,"描述",LVCFMT_LEFT,160);
	}

	return;
}

void CUnierm_CtrlDlg::RefreshListCtrl()
{
	m_listPrg.DeleteAllItems();

	for(size_t i = 0; i < m_vt_task.size(); ++i)
	{
		int nItem = m_listPrg.InsertItem(m_listPrg.GetItemCount(),
			m_vt_task[i].szProcessName);

		m_listPrg.SetItemData(nItem,(DWORD_PTR)i);

		int col = 1;

		{
			int nRunStat = -1;
			m_pMonitorSvc->GetTaskRunState(m_vt_task[i].szProcessName,nRunStat);

			m_listPrg.SetItemText(nItem,col,GetStatString(nRunStat));

			++col;
		}

		{
			int nRunConfigStat = -1;
			m_pMonitorSvc->GetTaskConfigState(m_vt_task[i].szProcessName,nRunConfigStat,1);

			m_listPrg.SetItemText(nItem,col,GetStatString(nRunConfigStat));

			++col;
		}
	
		{
			int nPermantConfigStat = -1;
			m_pMonitorSvc->GetTaskConfigState(m_vt_task[i].szProcessName,nPermantConfigStat,0);

			m_listPrg.SetItemText(nItem,col,GetStatString(nPermantConfigStat));

			++col;
		}

		m_listPrg.SetItemText(nItem,col,m_vt_task[i].szDesc);

	}

	return;
}

void CUnierm_CtrlDlg::OnBnClickedButtonStop()
{
	POSITION pos = m_listPrg.GetFirstSelectedItemPosition();

	int nitem = -1;

	if( pos )
	{
		nitem = m_listPrg.GetNextSelectedItem(pos);
	}

	if( nitem == -1 )
	{
		AfxMessageBox("请先选择一个程序再停止!",MB_OK|MB_ICONINFORMATION);

		return;
	}

	CWaitCursor wait_cursor;

	int index = (int)m_listPrg.GetItemData(nitem);

	int config_state = 1;

	m_pMonitorSvc->GetTaskConfigState(m_vt_task[index].szProcessName,config_state,1);

	if( config_state == 0 )
	{
		AfxMessageBox("该程序已经是停止状态!",MB_OK|MB_ICONINFORMATION);
		return;
	}

	{
		CString strInfo;

		strInfo.Format("您真地要停止程序: '%s'  吗?",m_vt_task[index].szProcessName);
		
		if( IDNO == AfxMessageBox(strInfo.GetString(),MB_YESNO|MB_ICONINFORMATION) )
		{
			return;
		}
	}

	if( 0 == m_pMonitorSvc->ChangeConfigState(m_vt_task[index].szProcessName,0,true) )
	{
		CString strInfo;

		strInfo.Format("停止 '%s' 成功!",m_vt_task[index].szDesc);
		
		AfxMessageBox(strInfo.GetString(),MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		CString strInfo;

		strInfo.Format("停止 '%s' 失败!",m_vt_task[index].szDesc);

		AfxMessageBox(strInfo.GetString(),MB_OK|MB_ICONINFORMATION);
	}

	RefreshState();

	return;
}

void CUnierm_CtrlDlg::OnBnClickedButtonStopAll()
{
	if( IDNO == AfxMessageBox("您真地要停止所有程序吗？",MB_YESNO|MB_ICONINFORMATION) )
	{
		return;
	}

	CWaitCursor wait_cursor;

	CString str_info;

	for(int nitem = 0; nitem < m_listPrg.GetItemCount(); ++nitem )
	{
		int index = (int)m_listPrg.GetItemData(nitem);

		int config_state = 1;

		m_pMonitorSvc->GetTaskConfigState(m_vt_task[index].szProcessName,config_state,1);

		if( config_state == 0 )
		{		
			str_info.AppendFormat("'%s' 已经是停止状态\n",m_vt_task[index].szDesc);

			continue;
		}

		if( 0 == m_pMonitorSvc->ChangeConfigState(m_vt_task[index].szProcessName,0,true) )
		{
			str_info.AppendFormat("停止 '%s' 成功\n",m_vt_task[index].szDesc);
		}
		else
		{
			str_info.AppendFormat("停止 '%s' 失败\n",m_vt_task[index].szDesc);
		}
	}

	AfxMessageBox(str_info.GetString(),MB_OK|MB_ICONINFORMATION);

	RefreshState();

	return;
}


void CUnierm_CtrlDlg::OnBnClickedButtonStopAllAndThis()
{
	if( IDNO == AfxMessageBox("您真地要停止所有程序(包括本身)吗？",MB_YESNO|MB_ICONINFORMATION) )
	{
		return;
	}

	CWaitCursor wait_cursor;

	CString str_info;

	for(int nitem = 0; nitem < m_listPrg.GetItemCount(); ++nitem )
	{
		int index = (int)m_listPrg.GetItemData(nitem);

		int config_state = 1;

		m_pMonitorSvc->GetTaskConfigState(m_vt_task[index].szProcessName,config_state,1);

		if( config_state == 0 )
		{		
			str_info.AppendFormat("'%s' 已经是停止状态\n",m_vt_task[index].szDesc);

			continue;
		}

		if( 0 == m_pMonitorSvc->ChangeConfigState(m_vt_task[index].szProcessName,0,true) )
		{
			str_info.AppendFormat("停止 '%s' 成功\n",m_vt_task[index].szDesc);
		}
		else
		{
			str_info.AppendFormat("停止 '%s' 失败\n",m_vt_task[index].szDesc);
		}
	}

	str_info += "\n\n点击确定后，本程序将自动退出!";

	AfxMessageBox(str_info.GetString(),MB_OK|MB_ICONINFORMATION);

	{
		if( m_pMonitorSvc )
		{
			m_pMonitorSvc->stop_svc();

			delete m_pMonitorSvc;
			m_pMonitorSvc = NULL;
		}

		if( m_pStopEvent )
		{
			ACE_OS::event_destroy(m_pStopEvent);
			delete m_pStopEvent;
			m_pStopEvent = NULL;
		}

		if( m_pMonitorEvent )
		{
			ACE_OS::event_destroy(m_pMonitorEvent);
			delete m_pMonitorEvent;
			m_pMonitorEvent = NULL;
		}
	}

	CDialog::OnCancel();

	return;
}



void CUnierm_CtrlDlg::OnBnClickedButtonStart()
{
	POSITION pos = m_listPrg.GetFirstSelectedItemPosition();

	int nitem = -1;

	if( pos )
	{
		nitem = m_listPrg.GetNextSelectedItem(pos);
	}

	if( nitem == -1 )
	{
		AfxMessageBox("请先选择一个程序再启动!",MB_OK|MB_ICONINFORMATION);

		return;
	}

	CWaitCursor wait_cursor;

	int index = (int)m_listPrg.GetItemData(nitem);

	{
		int permantStat = 1;

		m_pMonitorSvc->GetTaskConfigState(m_vt_task[index].szProcessName,permantStat,0);

		if( permantStat == 0 )
		{
			AfxMessageBox("系统的配置禁止了该程序的运行状态!",MB_OK|MB_ICONINFORMATION);
			return;
		}
	}

	int run_state = 0;

	m_pMonitorSvc->GetTaskRunState(m_vt_task[index].szProcessName,run_state);

	if( run_state == 1 )
	{
		AfxMessageBox("该程序已经是运行状态!",MB_OK|MB_ICONINFORMATION);
		return;
	}

	if( 0 == m_pMonitorSvc->ChangeConfigState(m_vt_task[index].szProcessName,1,true) )
	{
		CString strInfo;

		strInfo.Format("启动 '%s' 成功!",m_vt_task[index].szDesc);

		AfxMessageBox(strInfo.GetString(),MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		CString strInfo;

		strInfo.Format("启动 '%s' 失败!",m_vt_task[index].szDesc);

		AfxMessageBox(strInfo.GetString(),MB_OK|MB_ICONINFORMATION);
	}

	RefreshState();

	return;

}

void CUnierm_CtrlDlg::RefreshState()
{
	for(int nItem = 0; nItem < m_listPrg.GetItemCount(); ++nItem)
	{
		int index = (int)m_listPrg.GetItemData(nItem);

		int col = 1;

		{
			int nRunStat = -1;
			m_pMonitorSvc->GetTaskRunState(m_vt_task[index].szProcessName,nRunStat);

			m_listPrg.SetItemText(nItem,col,GetStatString(nRunStat));

			++col;
		}

		{
			int nRunConfigStat = -1;
			m_pMonitorSvc->GetTaskConfigState(m_vt_task[index].szProcessName,nRunConfigStat,1);

			m_listPrg.SetItemText(nItem,col,GetStatString(nRunConfigStat));

			++col;
		}

		{
			int nPermantConfigStat = -1;
			m_pMonitorSvc->GetTaskConfigState(m_vt_task[index].szProcessName,nPermantConfigStat,0);

			m_listPrg.SetItemText(nItem,col,GetStatString(nPermantConfigStat));

			++col;
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
			ACE_OS::event_signal(m_pMonitorEvent);
		}

	}


	CDialog::OnTimer(nIDEvent);

	return;
}



ACE_event_t * CUnierm_CtrlDlg::Create_Stop_Event(ACE_Log_File_Msg * loginstance,const char *stop_event_name )
{
	if( stop_event_name && strlen(stop_event_name) > 0 )
	{
		ACE_event_t * event = new ACE_event_t;

		if( -1 == ACE_OS::event_init(event,true,0,0,stop_event_name) )
		{
			MY_ACE_DEBUG(loginstance,
				(LM_INFO,
				ACE_TEXT("stop_event 创建失败!\n")
				));

			printf("stop_event 创建失败\n");
			delete event;
			return NULL;
		}
		else
		{
			MY_ACE_DEBUG(loginstance,
				(LM_INFO,
				ACE_TEXT("创建stop_event : '%s' 成功!\n"),stop_event_name
				));

			printf("创建stop_event : '%s' 成功!\n\n",stop_event_name);

			return event;
		}
	}
	else
	{
		MY_ACE_DEBUG(loginstance,
			(LM_INFO,
			ACE_TEXT("stop_event的名称为空，没有创建stop_event!\n")
			));

		printf("stop_event的名称为空，没有创建stop_event!\n\n");

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

	int nRet = ACE_OS::event_timedwait(m_pStopEvent, &time_wait);

	if( 0 == nRet )
	{
		KillTimer(1);

		CString strTitle;

		GetWindowText(strTitle);

		strTitle += "(recv stop event,and exit this program!)";

		SetWindowText(strTitle);

		if( m_pStopEvent )
		{
			ACE_OS::event_destroy(m_pStopEvent);
			delete m_pStopEvent;
			m_pStopEvent = NULL;
		}

		if( m_pMonitorEvent )
		{
			ACE_OS::event_destroy(m_pMonitorEvent);
			delete m_pMonitorEvent;
			m_pMonitorEvent = NULL;
		}

		CDialog::EndDialog(IDOK);

		return 0;
	}

	return -1;
}


void CUnierm_CtrlDlg::OnBnClickedButtonConfig()
{
	CRebootTimeCfgDlg dlg(m_pMonitorSvc);

	dlg.DoModal();

	return;
}

