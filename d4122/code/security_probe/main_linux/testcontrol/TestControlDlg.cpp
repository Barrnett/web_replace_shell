// TestControlDlg.cpp : implementation file
//


#include "TestControlDlg.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"
#include "flux_control.h"
#include "cpf/flux_tools.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/MyStr.h"
#include "TestControlParam.h"
#include "cpf/os_syscall.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestControlDlg dialog
int CTestControlRebootHandler::Do(int type,int reason)
{
	return m_pMainDlg->RebootHandler(type,reason);
}

CTestControlDlg::CTestControlDlg(const char * pCommandLine)
	: m_strCmdLine(pCommandLine)
	, m_TestControlRebootHandler(this)
{
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

//	m_pWndTabs = NULL;

//	m_pOtherInfoPage = NULL;



	m_iSecondCount = 0;
	m_bStartTimer = false;
	m_bStopEvent = false;

	return;
}

CTestControlDlg::~CTestControlDlg()
{
	DestroyIfInfoPage();

	return;

}
/*
void CTestControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

}

#define MM_REBOOT_HANDLER (WM_USER+101)

BEGIN_MESSAGE_MAP(CTestControlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTestControlDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTestControlDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_START, &CTestControlDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTestControlDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_STOP_AND_EXIT, &CTestControlDlg::OnBnClickedButtonStopAndExit)

	ON_WM_TIMER()
	ON_MESSAGE(WM_RESTART_PROBE, &CTestControlDlg::OnRestartProbe)
	ON_MESSAGE(MM_REBOOT_HANDLER, &CTestControlDlg::OnRebootHandlerMsg)
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_BUTTON_CHANNEL_FLUX, &CTestControlDlg::OnBnClickedButtonChannelFlux)
	ON_BN_CLICKED(IDC_BUTTON_CTRL_PARAM, &CTestControlDlg::OnBnClickedButtonCtrlParam)
	ON_BN_CLICKED(IDC_BUTTON_CAP_DATA, &CTestControlDlg::OnBnClickedButtonCapData)

	ON_BN_CLICKED(IDC_BUTTON_SAVE_AUTOAMP, &CTestControlDlg::OnBnClickedButtonSaveAutoamp)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_AUTOAMP, &CTestControlDlg::OnBnClickedButtonClearAutoamp)
END_MESSAGE_MAP()
*/


// CTestControlDlg message handlers

BOOL CTestControlDlg::OnInitDialog()
{
//	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
/*
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
*/

	// TODO: Add extra initialization here

	CPF::CreateFullDirecory(CPF::GetModuleFullFileName("log"));

	char log_filename[MAX_PATH];

	CPF::GetModuleOtherFileName(log_filename, sizeof(log_filename), "log", "test_control.log");

	CPF::DeleteTimerLogFile(CPF::GetModuleFullFileName("log"),"test_control_","log",30*24*3600);

	m_log_file.init_oneday_log(log_filename,false);

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

	MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Info][%D]CTestControlDlg::OnInitDialog()\n")));
		
	char xml_file_name[MAX_PATH] = {0};

	CPF::GetModuleOtherFileName(xml_file_name,sizeof(xml_file_name),"ini","test_control.xml");

	CTinyXmlEx xml_reader;

	if( 0 != xml_reader.open(xml_file_name) )
	{
		MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Err][%D]cannot find: %s\n"),xml_file_name));
		return FALSE;
	}

	TiXmlNode * pcommon_node = xml_reader.GetRootNode()->FirstChildElement("common");

	if( !pcommon_node )
	{
		MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Err][%D]file format error: test_control.xml\n") ));	
		return FALSE;
	}

	if( m_strCmdLine.empty() )
	{
		xml_reader.GetValueLikeIni(pcommon_node,"cmd_line",m_strCmdLine);
	}	

	{
		const int empty_sleep_time = CCTestControlParam_Singleton::instance()->m_empty_sleep_time;

		char monitor_event_name[256] = {0};
		char stop_event_name[256] = {0};
		int  monitor_event_interval = 0;
		int  stop_event_interval = 0;

		sscanf(m_strCmdLine.c_str(),"%s %s %d %d",
			monitor_event_name,stop_event_name,
			&monitor_event_interval,&stop_event_interval );

		BOOL ret = 
		m_main_ctrl.init(ACE_Time_Value(0,empty_sleep_time*1000),
			NULL,
			xml_file_name,
			"observer_list",
			&m_log_file,
			monitor_event_name,
			monitor_event_interval,
			CCTestControlParam_Singleton::instance()->m_bCopyDrvData);
		if (false == ret)
		{
			return FALSE;
		}

		FLUX_CONTROL_SetStopEventInfo(stop_event_name,stop_event_interval);
	}

// 	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
// 	GetDlgItem(IDC_BUTTON_STOP_AND_EXIT)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
// 	
// 	GetDlgItem(IDC_BUTTON_START)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);

	FLUX_CONTROL_SetRebootHandler(&m_TestControlRebootHandler);	

	{

	
	}

	CreateIfInfoPage();

/*
	{
		int b_auto_start = 0;

		xml_reader.GetValueLikeIni(pcommon_node,"auto_start",b_auto_start);

		if( b_auto_start )
		{
			PostMessage(WM_COMMAND,IDC_BUTTON_START,0);
		}	

	}
*/

	MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Info][%D]CTestControlDlg::OnInitDialog ok!\n")));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*
void CTestControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
*/

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

/*
void CTestControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}*/


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
/*
HCURSOR CTestControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestControlDlg::OnBnClickedCancel()
{
	return;
}
*/


void CTestControlDlg::OnBnClickedOk()
{
	DestroyIfInfoPage();

	m_main_ctrl.close();

//	OnOK();

	return;
}

int CTestControlDlg::start_test()
{
	IRecvDataSource * pRecvDataSource = FLUX_CONTROL_GetRecvDataSource();

	if( !pRecvDataSource )
	{
		CreateEmpyeDataSource();

		pRecvDataSource = m_pEmptyDataSouce;
	}

	/*add by zhongjh*/
	CSerialDataRecv* pSerialDataRecv = FLUX_CONTROL_GetSerialDataRecv();
	if (!pSerialDataRecv)
	{
		pSerialDataRecv = new CSerialDataRecv;
		if (!pSerialDataRecv)
		{
			MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("[Err][%D]pSerialDataRecv create failed.\n")));
		}
	}

	//if( !m_main_ctrl.StartTest(pRecvDataSource) )
	if (!m_main_ctrl.StartTest(pRecvDataSource, pSerialDataRecv))
	{
		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("[Err][%D]m_main_ctrl.StartTest failed.\n")
			));
		return -2;
	}


// 	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
// 	GetDlgItem(IDC_BUTTON_STOP_AND_EXIT)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
// 
// 	GetDlgItem(IDC_BUTTON_START)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);
// 
// 	GetDlgItem(IDOK)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);


// 	if( FLUX_CONTROL_GetTestMode() == 0 
// 		&& pRecvDataSource->GetDrvType() != DRIVER_TYPE_EMPTY)
// 	{
// 		m_CheckMgr.start_check();
// 	}

// 	m_CheckMgr.SetCanPostMessage();

	if( pRecvDataSource->GetDrvType() == DRIVER_TYPE_EMPTY )
	{
		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			//ACE_TEXT("\n%D:系统没有打开数据接口，进入空转!!!!!!!!!!!!!!!!!!!\n")
			ACE_TEXT("[Err][%D]program do nothing because of DRIVER_TYPE_EMPTY!!!!!!!!!!!!!!!!!!!\n")
			));

		return 1;
	}
	else
	{
		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("[Info][%D]CTestControlDlg::start_test ok!!!!!!!!!!!!!!!!!!!\n")
			));

		return 0;
	}
}

void CTestControlDlg::OnBnClickedButtonStart()
{
	int ncode = start_test();

/*
	if( m_pOtherInfoPage )
	{
		m_pOtherInfoPage->start_test();
	}

	for(size_t i = 0; i < m_vt_IfInfo_page.size(); ++i)
	{
		m_vt_IfInfo_page[i]->start_test();
	}
*/
//	SetTimer(TIMER_RESET_WDT,1*1000,NULL);
//	SetTimer(IIMER_CHECK_STOP_EVENT,2*1000,NULL);

	m_iSecondCount = 0;
	m_bStartTimer = true;

	return;
}

void CTestControlDlg::stop_test()
{
	m_bStartTimer = false;

/*
	KillTimer(TIMER_RESET_WDT);
	KillTimer(IIMER_CHECK_STOP_EVENT);
	KillTimer(TIMER_EMPTY_DATA_SOURCE);

	if( FLUX_CONTROL_GetTestMode() == 0 )
	{
		m_CheckMgr.stop_check();
	}
	*/

	m_main_ctrl.StopTest();

/*
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
		GetDlgItem(IDC_BUTTON_STOP_AND_EXIT)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);
	
		GetDlgItem(IDOK)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);
	
		if( m_pOtherInfoPage )
		{
			m_pOtherInfoPage->stop_test();
		}

		for(size_t i = 0; i < m_vt_IfInfo_page.size(); ++i)
		{
			m_vt_IfInfo_page[i]->stop_test();
		}
*/		

	return;
}

void CTestControlDlg::OnBnClickedButtonStop()
{
/*
	if( AfxMessageBox("你是否真的要停止测试?",MB_YESNO) == IDNO )
	{
		return;
	}
*/

//#ifndef _DEBUG
//	if( m_pBypassTool->GetBypassType() == 0
//		&& FLUX_CONTROL_GetTestMode() == 0 )
//	{
//		if( AfxMessageBox("系统没有bypass功能，停止测试将导致网络中断。\n\n你是否真的要停止测试?",MB_YESNO) == IDNO )
//		{
//			return;
//		}
//	}
//#endif

/*
	CWaitCursor wait_cursor;
*/

	stop_test();

	return;
}


void CTestControlDlg::OnBnClickedButtonStopAndExit()
{
	stop_test();

	OnBnClickedOk();

	return;
}


void CTestControlDlg::OnTimer(unsigned int  nIDEvent)
{
	if( IIMER_CHECK_STOP_EVENT == nIDEvent )
	{
		if( CheckStopEvent() == 0 )
		{
			return;
		}		
	}
	else if( TIMER_EMPTY_DATA_SOURCE == nIDEvent )
	{
		stop_test();

//		EndDialog(IDCANCEL);
	}

//	CDialog::OnTimer(nIDEvent);
}



int CTestControlDlg::CheckStopEvent()
{
	m_bStopEvent |= FLUX_CONTROL_IsGetStopEvent();

	if( m_bStopEvent )
	{
// 		KillTimer(IIMER_CHECK_STOP_EVENT);

		MY_ACE_DEBUG(&m_log_file,
			(LM_INFO,
			ACE_TEXT("[Info][%D]recv stop event,and exit this program!\n")
			));

/*
		CString strTitle;

		GetWindowText(strTitle);

		strTitle += "(recv stop event,and exit this program!)";

		SetWindowText(strTitle);

		*/

		m_main_ctrl.StopTest();

		m_main_ctrl.close();

/*
		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("\n\n----------------------------------------------------------------\n")
			));

		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("\n%D:%s!!!!!!!!!!!!!!!!!!!\n"),strTitle.GetString()
			));

		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("\n----------------------------------------------------------------\n\n")
			));
*/

//		CDialog::EndDialog(IDOK);

		return 0;
	}

	return -1;
}

void CTestControlDlg::OnClose()
{
	//do nothing,
	//目的是为了让用户不使用这个功能
}


void CTestControlDlg::CreateIfInfoPage()
{
/*
	const int nIfLineNums = FLUX_CONTROL_GetIfLineNums();

	if( nIfLineNums <= 0 )
	{
		return;
	}
	for(int i = 0; i < nIfLineNums; ++i)
	{
		int line_index = FLUX_CONTROL_GetIfLineIndex(i);

		CIfInfoPage * pIfInfoPage = new CIfInfoPage(m_log_file,line_index);

		m_vt_IfInfo_page.push_back(pIfInfoPage);
	}


	{
		m_pOtherInfoPage = new COtherInfoPage(m_log_file,m_main_ctrl);

		m_pOtherInfoPage->Create(COtherInfoPage::IDD,m_pWndTabs);

		m_pWndTabs->AddTab(m_pOtherInfoPage,"内存和链接",-1);

	}

	{
		m_pStatTablePage = new CStatTablePage;

		m_pStatTablePage->Create(CStatTablePage::IDD,m_pWndTabs);

		m_pWndTabs->AddTab(m_pStatTablePage,"统计表",-1);

	}
*/

	return;
}

void CTestControlDlg::DestroyIfInfoPage()
{
	for(size_t i = 0; i < m_vt_IfInfo_page.size(); ++i)
	{
		delete m_vt_IfInfo_page[i];
	}

	m_vt_IfInfo_page.clear();

/*
	if( m_pOtherInfoPage )
	{
		delete m_pOtherInfoPage;
		m_pOtherInfoPage = NULL;
	}

	if( m_pStatTablePage )
	{
		delete m_pStatTablePage;
		m_pStatTablePage = NULL;
	}
*/
	

	return;
}



/*
void CTestControlDlg::OnBnClickedButtonChannelFlux()
{
	CChannelFluxSheet dlg("通道流量分析");

	dlg.DoModal();

	return;
}

void CTestControlDlg::OnBnClickedButtonCtrlParam()
{
	CCtrlParamDlg dlg;

	dlg.m_bResetWDTInLoop = m_bResetWDTInLoop;
	dlg.m_bResetWDTInUI = m_bResetWDTInUI;

	if( dlg.DoModal() == IDCANCEL )
		return;

	//m_cur_functype = dlg.m_cur_functype;

	//m_CheckMgr.EnableCheck(m_cur_functype!=FUNC_TYPE_HARD_PASS);

	m_bResetWDTInLoop = dlg.m_bResetWDTInLoop;
	m_bResetWDTInUI = dlg.m_bResetWDTInUI;

	m_live_stat_sync_on_add_stat = dlg.m_bSyncAddStat;
	m_live_stat_sync_on_del_stat = dlg.m_bSyncDelStat;

	FLUX_CONTROL_EnableResetWDTInLoop(m_bResetWDTInLoop);

	char xml_file_name[MAX_PATH] = {0};

	CPF::GetModuleOtherFileName(xml_file_name,sizeof(xml_file_name),"ini","test_control.xml");

	CTinyXmlEx xml_writter;

	if( 0 != xml_writter.open_ex(xml_file_name,CTinyXmlEx::OPEN_MODE_OPEN_CREATE) )
	{
		return;
	}

	TiXmlNode * pcommon_node = xml_writter.GetRootNode()->FirstChildElement("common");

	if( !pcommon_node )
	{
		return;
	}

	xml_writter.SetValueLikeIni(pcommon_node,"reset_wdt_in_ui",m_bResetWDTInUI);
	xml_writter.SetValueLikeIni(pcommon_node,"reset_wdt_in_loop",m_bResetWDTInLoop);
	xml_writter.SetValueLikeIni(pcommon_node,"live_log_level",dlg.m_live_log_level);

	xml_writter.SetValueLikeIni(pcommon_node,"live_stat_sync_on_add_stat",m_live_stat_sync_on_add_stat);
	xml_writter.SetValueLikeIni(pcommon_node,"live_stat_sync_on_del_stat",m_live_stat_sync_on_del_stat);

	xml_writter.savefile();

	xml_writter.close();

	return;
}

#include "CapConfigDlg.h"


void CTestControlDlg::OnBnClickedButtonCapData()
{
	static IF_DATA_TYPE last_cap_if_data_type = IF_DATA_TYPE_BEFORE_TRANS;
	static int last_cap_file_type = 1;
	static BOOL last_bFilter = 0;
	static char last_str_save_path[MAX_PATH] = {0};

	CCapConfigDlg dlg;

	dlg.m_cap_if_data_type = last_cap_if_data_type;
	dlg.m_cap_file_type = last_cap_file_type;
	dlg.m_bFilter = last_bFilter;
	dlg.m_str_save_path = last_str_save_path;

	dlg.DoModal();

	last_cap_if_data_type = dlg.m_cap_if_data_type;
	last_cap_file_type = dlg.m_cap_file_type;
	last_bFilter = dlg.m_bFilter;
	strcpy(last_str_save_path,dlg.m_str_save_path.GetString());

	return;
}


void CTestControlDlg::OnBnClickedButtonSaveAutoamp()
{
	if( IDNO == AfxMessageBox("您真的要导出automap表?",MB_YESNO|MB_ICONQUESTION) )
	{
		return;
	}

	FLUX_CONTROL_SaveAutoMap();

	AfxMessageBox("导出完成!",MB_OK);

	return;
}

void CTestControlDlg::OnBnClickedButtonClearAutoamp()
{
	if( IDNO == AfxMessageBox("这个功能只为厂家调试时使用,您真的要清空automap表?",MB_YESNO|MB_ICONQUESTION) )
	{
		return;
	}

	FLUX_CONTROL_ClearAutoMap();

	AfxMessageBox("清空完成!",MB_OK);

	return;
}

*/
int CTestControlDlg::RebootHandler(int type,int reason)
{
	return OnRebootHandlerMsg(type, reason);
//	PostMessage(MM_REBOOT_HANDLER,(WPARAM)type,(LPARAM)reason);

	return 0;
}

BOOL CTestControlDlg::CreateEmpyeDataSource()
{
	m_pEmptyDataSouce = new CEmptyDataSource;

	m_pEmptyDataSouce->OpenDataSource();

	return true;
}

void CTestControlDlg::DestroyEmpyeDataSource()
{
	if( m_pEmptyDataSouce )
	{
		m_pEmptyDataSouce->CloseDataSource();
		delete m_pEmptyDataSouce;
		m_pEmptyDataSouce = NULL;
	}

	return;
}


void CTestControlDlg::OneSecondCall()
{
	if (!m_bStartTimer)
		return;

	m_iSecondCount ++;

	if (m_iSecondCount % 2 == 0)
	{
		OnTimer(IIMER_CHECK_STOP_EVENT);
	}

	return;
}

int CTestControlDlg::OnRebootHandlerMsg(int wparam, int reason)
{
	if( wparam == 0 )
	{//退出整个程序

		m_bStopEvent = true;

	}
	else if( wparam == 1)
	{//重新启动机器

		stop_test();

		CPF::MySystemShutdown(0x02);
	}
	else if( wparam == 2)
	{//关闭设备

		stop_test();

		CPF::MySystemShutdown(0x08);
	}

	return 0;
}

// add by xumx, 2016-12-12
char *CTestControlDlg::f_get_system_version()
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

