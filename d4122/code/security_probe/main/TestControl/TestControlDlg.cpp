// TestControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "TestControlDlg.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"
#include "flux_control.h"
#include "cpf/flux_tools.h"
#include "cpf/TinyXmlEx.h"

//#include "IfInfoPage.h"
#include "CardStatPage.h"
#include "OtherInfoPage.h"
#include "TabWndEx.h"

#include "CtrlParamDlg.h"
#include "cpf/os_syscall.h"

#include "TestControlParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


int CTestControlRebootHandler::Do(int type,int reason)
{
	return m_pMainDlg->RebootHandler(type,reason);
}

// CTestControlDlg dialog


CTestControlDlg::CTestControlDlg(const char * pCommandLine,CWnd* pParent /*=NULL*/)
	: CDialog(CTestControlDlg::IDD, pParent)
	, m_strCmdLine(pCommandLine)
	, m_TestControlRebootHandler(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pWndTabs = NULL;

	m_pCardStatPage = NULL;
	m_pOtherInfoPage = NULL;

	m_pEmptyDataSouce = NULL;

	m_bStartTimer = false;

	m_p_log_file = NULL;

	return;
}

CTestControlDlg::~CTestControlDlg()
{
	DestroyIfInfoPage();

	return;

}
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
	
	ON_MESSAGE(MM_REBOOT_HANDLER, &CTestControlDlg::OnRebootHandlerMsg)
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_BUTTON_CHANNEL_FLUX, &CTestControlDlg::OnBnClickedButtonChannelFlux)
	ON_BN_CLICKED(IDC_BUTTON_CTRL_PARAM, &CTestControlDlg::OnBnClickedButtonCtrlParam)
	ON_BN_CLICKED(IDC_BUTTON_CAP_DATA, &CTestControlDlg::OnBnClickedButtonCapData)

	ON_BN_CLICKED(IDC_BUTTON_SAVE_AUTOAMP, &CTestControlDlg::OnBnClickedButtonSaveAutoamp)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_AUTOAMP, &CTestControlDlg::OnBnClickedButtonClearAutoamp)
	ON_BN_CLICKED(IDC_BUTTON_OUT_PUT, &CTestControlDlg::OnBnClickedButtonOutPut)
	ON_BN_CLICKED(IDC_BUTTON_START_TIMER, &CTestControlDlg::OnBnClickedButtonStartTimer)
	ON_BN_CLICKED(IDC_BUTTON_STOP_TIMER, &CTestControlDlg::OnBnClickedButtonStopTimer)
END_MESSAGE_MAP()


// CTestControlDlg message handlers

BOOL CTestControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// TODO: Add extra initialization here
	m_p_log_file = ACE_Log_File_Msg_Singleton::instance();
	ACE_ASSERT(m_p_log_file);

	MY_ACE_DEBUG(m_p_log_file,
		(LM_ERROR,
		ACE_TEXT("\n\n\n###############################################################\n")
		));

	MY_ACE_DEBUG(m_p_log_file,
		(LM_ERROR,
		ACE_TEXT("\n%D:程序开始启动!!!!!!!!!!!!!!!!!!!\n")
		));

	MY_ACE_DEBUG(m_p_log_file,
		(LM_ERROR,
		ACE_TEXT("\n###############################################################\n\n\n")
		));

	char xml_file_name[MAX_PATH] = {0};

	CPF::GetModuleOtherFileName(xml_file_name,sizeof(xml_file_name),"ini","test_control.xml");

	CTinyXmlEx xml_reader;

	if( 0 != xml_reader.open(xml_file_name) )
	{
		AfxMessageBox("没有找到配置文件:test_control.xml",MB_OK);
		EndDialog(IDCANCEL);
		return FALSE;
	}

	TiXmlNode * pcommon_node = xml_reader.GetRootNode()->FirstChildElement("common");

	if( !pcommon_node )
	{
		AfxMessageBox("test_control.xml配置文件格式错误!",MB_OK|MB_ICONERROR);
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if( m_strCmdLine.empty() )
	{
		xml_reader.GetValueLikeIni(pcommon_node,"cmd_line",m_strCmdLine);
	}	

	{
		const int empty_sleep_time = CCTestControlParam_Singleton::instance()->m_empty_sleep_time;

		//xml_reader.GetValueLikeIni(pcommon_node,"empty_sleep_time",empty_sleep_time);

		char monitor_event_name[256] = {0};
		char stop_event_name[256] = {0};
		int  monitor_event_interval = 0;
		int  stop_event_interval = 0;

		int nrtn = sscanf(m_strCmdLine.c_str(),"%s %s %d %d",
			monitor_event_name,stop_event_name,
			&monitor_event_interval,&stop_event_interval );

		m_main_ctrl.init(ACE_Time_Value(0,empty_sleep_time*1000),
			NULL,
			xml_file_name,
			"observer_list",
			m_p_log_file,
			monitor_event_name,
			monitor_event_interval,
			CCTestControlParam_Singleton::instance()->m_bCopyDrvData);

		FLUX_CONTROL_SetStopEventInfo(stop_event_name,stop_event_interval);
	}

	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
	GetDlgItem(IDC_BUTTON_STOP_AND_EXIT)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
	
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);

	FLUX_CONTROL_SetRebootHandler(&m_TestControlRebootHandler);


	CreateIfInfoPage();

	{
		int b_auto_start = 0;

		xml_reader.GetValueLikeIni(pcommon_node,"auto_start",b_auto_start);

		if( b_auto_start )
		{
			PostMessage(WM_COMMAND,IDC_BUTTON_START,0);
		}	

	}

	MY_ACE_DEBUG(m_p_log_file,
		(LM_ERROR,
		ACE_TEXT("\n%D:程序已经初始化完成，正在启动!!!!!!!!!!!!!!!!!!!\n")
		));

	GetDlgItem(IDC_BUTTON_START_TIMER)->EnableWindow(!m_bStartTimer);
	GetDlgItem(IDC_BUTTON_STOP_TIMER)->EnableWindow(m_bStartTimer);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

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
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestControlDlg::OnBnClickedCancel()
{
	return;
}

void CTestControlDlg::OnBnClickedOk()
{
	DestroyIfInfoPage();

	m_main_ctrl.close();	

	OnOK();

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

	if( !m_main_ctrl.StartTest(pRecvDataSource) )
	{
		return -2;
	}

	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
	GetDlgItem(IDC_BUTTON_STOP_AND_EXIT)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);

	GetDlgItem(IDOK)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);

	if( CCTestControlParam_Singleton::instance()->m_log_flux )
	{
		OnBnClickedButtonStartTimer();
	}
	else
	{
#ifdef _DEBUG
		OnBnClickedButtonStartTimer();
#else
		//do nothing
#endif
	}

	if( pRecvDataSource->GetDrvType() == DRIVER_TYPE_EMPTY )
	{
		MY_ACE_DEBUG(m_p_log_file,
			(LM_ERROR,
			ACE_TEXT("\n%D:系统没有打开数据接口，进入空转!!!!!!!!!!!!!!!!!!!\n")
			));

		return 1;
	}
	else
	{
		MY_ACE_DEBUG(m_p_log_file,
			(LM_ERROR,
			ACE_TEXT("\n%D:程序启动成功!!!!!!!!!!!!!!!!!!!\n")
			));

		return 0;
	}

}

void CTestControlDlg::OnBnClickedButtonStart()
{
	int ncode = start_test();

	if( -2 == ncode )
	{
		AfxMessageBox("启动测试失败",MB_OK|MB_ICONERROR);

		EndDialog(IDOK);

		return;
	}

	if( 1 == ncode )
	{
		CString strTitle;

		GetWindowText(strTitle);

		strTitle += "(emptye_data_source!)";

		SetWindowText(strTitle);

		SetTimer(TIMER_EMPTY_DATA_SOURCE,20*60*1000,NULL);

	}

	if( m_pCardStatPage )
	{
		m_pCardStatPage->start_test();
	}

	if( m_pOtherInfoPage )
	{
		m_pOtherInfoPage->start_test();
	}

	SetTimer(IIMER_CHECK_STOP_EVENT,2*1000,NULL);

	return;
}

void CTestControlDlg::stop_test()
{
	KillTimer(IIMER_CHECK_STOP_EVENT);
	KillTimer(TIMER_EMPTY_DATA_SOURCE);	

	m_main_ctrl.StopTest();

	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
	GetDlgItem(IDC_BUTTON_STOP_AND_EXIT)->EnableWindow(m_main_ctrl.GetRunState()==CCommonMainCtrl::STATE_RUN);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);

	GetDlgItem(IDOK)->EnableWindow(m_main_ctrl.GetRunState()!=CCommonMainCtrl::STATE_RUN);

	if( m_pCardStatPage )
	{
		m_pCardStatPage->stop_test();
	}

	if( m_pOtherInfoPage )
	{
		m_pOtherInfoPage->stop_test();
	}

	DestroyEmpyeDataSource();

	return;
}

void CTestControlDlg::OnBnClickedButtonStop()
{
	if( AfxMessageBox("你是否真的要停止测试?",MB_YESNO) == IDNO )
	{
		return;
	}

	CWaitCursor wait_cursor;

	stop_test();

	return;
}


void CTestControlDlg::OnBnClickedButtonStopAndExit()
{
	if( AfxMessageBox("你是否真的要停止测试并且退出程序吗?",MB_YESNO) == IDNO )
	{
		return;
	}

	CWaitCursor wait_cursor;

	stop_test();

	OnBnClickedOk();

	return;
}




LRESULT CTestControlDlg::OnRebootHandlerMsg(WPARAM wparam,LPARAM lparam)
{
	if( wparam == 0 )
	{//退出整个程序

		stop_test();

		EndDialog(IDCANCEL);
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


void CTestControlDlg::OnTimer(UINT_PTR nIDEvent)
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

		EndDialog(IDCANCEL);
	}

	CDialog::OnTimer(nIDEvent);
}

int CTestControlDlg::CheckStopEvent()
{
	if( FLUX_CONTROL_IsGetStopEvent() )
	{
		KillTimer(IIMER_CHECK_STOP_EVENT);

		MY_ACE_DEBUG(m_p_log_file,
			(LM_INFO,
			ACE_TEXT("%D:recv stop event,and exit this program!\n")
			));

		CString strTitle;

		GetWindowText(strTitle);

		strTitle += "(recv stop event,and exit this program!)";

		SetWindowText(strTitle);

		m_main_ctrl.StopTest();	

		m_main_ctrl.close();

		MY_ACE_DEBUG(m_p_log_file,
			(LM_ERROR,
			ACE_TEXT("\n\n----------------------------------------------------------------\n")
			));

		MY_ACE_DEBUG(m_p_log_file,
			(LM_ERROR,
			ACE_TEXT("\n%D:%s!!!!!!!!!!!!!!!!!!!\n"),strTitle.GetString()
			));

		MY_ACE_DEBUG(m_p_log_file,
			(LM_ERROR,
			ACE_TEXT("\n----------------------------------------------------------------\n\n")
			));

		CDialog::EndDialog(IDOK);

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
	m_pWndTabs = new CTabWndEx;

	CRect rect;

	GetDlgItem(IDC_STATIC_POS)->GetClientRect(rect);

	GetDlgItem(IDC_STATIC_POS)->ClientToScreen(rect);

	this->ScreenToClient(rect);

	if (!m_pWndTabs->Create(CTabWndEx::STYLE_3D, rect, this, 4001,CTabWndEx::LOCATION_TOP))
	{
		return ;      // fail to create
	}

	//const int nIfLineNums = FLUX_CONTROL_GetDevIfLineNums();

	//if( nIfLineNums <= 0 )
	//{
	//	return;
	//}

	//for(int pos = 0; pos < nIfLineNums; ++pos)
	//{
	//	CIfInfoPage * pIfInfoPage = new CIfInfoPage(*m_p_log_file,pos,CCTestControlParam_Singleton::instance()->m_log_flux);

	//	pIfInfoPage->Create(CIfInfoPage::IDD,m_pWndTabs);		

	//	int line_index = FLUX_CONTROL_GetDevIfLineIndex(pos);

	//	char buf[64];
	//	sprintf(buf,"线路%d",line_index+1);

	//	m_pWndTabs->AddTab(pIfInfoPage,buf,-1);

	//	m_vt_IfInfo_page.push_back(pIfInfoPage);
	//}

	{
		m_pCardStatPage = new CCardStatPage();

		m_pCardStatPage->Create(CCardStatPage::IDD,m_pWndTabs);

		m_pWndTabs->AddTab(m_pCardStatPage,"设备接口流量",-1);

	}

	{
		m_pOtherInfoPage = new COtherInfoPage(*m_p_log_file,m_main_ctrl,CCTestControlParam_Singleton::instance()->m_log_flux);

		m_pOtherInfoPage->Create(COtherInfoPage::IDD,m_pWndTabs);

		m_pWndTabs->AddTab(m_pOtherInfoPage,"内存和链接",-1);

	}


	return;
}

void CTestControlDlg::DestroyIfInfoPage()
{
	if( m_pWndTabs )
	{
		delete m_pWndTabs;
		m_pWndTabs = NULL;
	}

	if( m_pCardStatPage )
	{
		delete m_pCardStatPage;
		m_pCardStatPage = NULL;
	}

	if( m_pOtherInfoPage )
	{
		delete m_pOtherInfoPage;
		m_pOtherInfoPage = NULL;
	}

	return;
}


void CTestControlDlg::OnBnClickedButtonChannelFlux()
{
	return;
}

void CTestControlDlg::OnBnClickedButtonCtrlParam()
{
	CCtrlParamDlg dlg;

	if( dlg.DoModal() == IDCANCEL )
		return;

	return;
}

#include "CapConfigDlg.h"


void CTestControlDlg::OnBnClickedButtonCapData()
{
	static int last_cap_file_type = 1;
	static BOOL last_bFilter = 0;
	static char last_str_save_path[MAX_PATH] = {0};

	std::vector<std::string> vt_cardinfo;

	BOOL bDrvOK = TRUE;
	FLUX_CONTROL_GetAllNetCardInfo(bDrvOK,vt_cardinfo);

	CCapConfigDlg dlg(vt_cardinfo);

	dlg.m_cap_file_type = last_cap_file_type;
	dlg.m_bFilter = last_bFilter;
	dlg.m_str_save_path = last_str_save_path;

	dlg.DoModal();

	last_cap_file_type = dlg.m_cap_file_type;
	last_bFilter = dlg.m_bFilter;
	strcpy(last_str_save_path,dlg.m_str_save_path.GetString());

	return;
}


void CTestControlDlg::OnBnClickedButtonSaveAutoamp()
{
	return;
}

void CTestControlDlg::OnBnClickedButtonClearAutoamp()
{

	return;
}

int CTestControlDlg::RebootHandler(int type,int reason)
{
	PostMessage(MM_REBOOT_HANDLER,(WPARAM)type,(LPARAM)reason);

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

void CTestControlDlg::OnBnClickedButtonOutPut()
{
	if( IDNO == AfxMessageBox("这个功能只为厂家调试时使用,您真的要立即输出吗?",MB_YESNO|MB_ICONQUESTION) )
	{
		return;
	}

	FLUX_CONTROL_Do_OutPut();

	return;
}

void CTestControlDlg::OnBnClickedButtonStartTimer()
{
	m_bStartTimer = true;

	if( m_pCardStatPage )
	{
		m_pCardStatPage->StartTimer();
	}

	if( m_pOtherInfoPage )
	{
		m_pOtherInfoPage->StartTimer();
	}

	GetDlgItem(IDC_BUTTON_START_TIMER)->EnableWindow(!m_bStartTimer);
	GetDlgItem(IDC_BUTTON_STOP_TIMER)->EnableWindow(m_bStartTimer);

	return;
}

void CTestControlDlg::OnBnClickedButtonStopTimer()
{
	m_bStartTimer = false;

	if( m_pCardStatPage )
	{
		m_pCardStatPage->StopTimer();
	}

	if( m_pOtherInfoPage )
	{
		m_pOtherInfoPage->StopTimer();
	}

	GetDlgItem(IDC_BUTTON_START_TIMER)->EnableWindow(!m_bStartTimer);
	GetDlgItem(IDC_BUTTON_STOP_TIMER)->EnableWindow(m_bStartTimer);

	return;
}





//#define DIFF_TIME_EX(old_time,new_time)	(((new_time)>=(old_time))?(new_time)-(old_time):(old_time=(new_time),((new_time)-old_time)))
//
//#define DIFF_TIME_STAMP_SEC_EX(old_time,new_time)	(((new_time).GetSEC()>=old_time.GetSEC())?(new_time).GetSEC()-old_time.GetSEC():(old_time=(new_time),(new_time).GetSEC()-old_time.GetSEC()))
//
//#define DIFF_TIME_VALUE_SEC_EX(old_time,new_time)	(((new_time).sec()>=old_time.sec())?(new_time).sec()-old_time.sec():(old_time=(new_time),(new_time).sec()-old_time.sec()))
//

//void test()
//{
//	{
//		CTimeStamp32 oldtime(5,0);
//
//		CTimeStamp32 newtime(4,0);
//		CTimeStamp32 oldtime2(5,0);
//
//
//		CTimeStamp32 diff = DIFF_TIME_EX(oldtime,oldtime2-newtime);
//
//		int k = 0;
//
//	}
//
//	{
//		ACE_Time_Value oldtime(5,0);
//
//		ACE_Time_Value newtime(4,0);
//		ACE_Time_Value oldtime2(5,0);
//
//
//		ACE_Time_Value diff = DIFF_TIME_EX(oldtime,oldtime2-newtime);
//
//		int k = 0;
//
//	}
//
//	{
//		CTimeStamp32 oldtime(5,0);
//		CTimeStamp32 oldtime2(5,0);
//
//		CTimeStamp32 newtime(4,0);
//
//
//		int diff = DIFF_TIME_STAMP_SEC_EX(oldtime,oldtime2-newtime);
//
//		int k = 0;
//
//	}
//
//	{
//		ACE_Time_Value oldtime(5,0);
//		ACE_Time_Value oldtime2(5,0);
//
//		ACE_Time_Value newtime(4,0);
//
//		time_t diff = DIFF_TIME_VALUE_SEC_EX(oldtime,oldtime2-newtime);
//
//		int k = 0;
//
//	}
//
//}
