// CapConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "CapConfigDlg.h"
#include "CptDstFileDlg.h"
#include "EtherealDstFileDlg.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"
#include "flux_control.h"

// CCapConfigDlg dialog

IMPLEMENT_DYNAMIC(CCapConfigDlg, CDialog)

CCapConfigDlg::CCapConfigDlg(const std::vector<std::string>& vt_cardinfo,CWnd* pParent /*=NULL*/)
	: CDialog(CCapConfigDlg::IDD, pParent)
	, m_bFilter(FALSE)
	, m_cap_file_type(-1)
{
	m_bCapping = false;

	m_cpt_write_param.captime = 0;
	m_cpt_write_param.num_files = 0;
	m_cpt_write_param.one_filesize_mb = 1024;

	m_ethreal_write_param.captime = 0;
	m_ethreal_write_param.num_files = 0;
	m_ethreal_write_param.one_filesize_mb = 100;

	m_begin_cap_time = 0;

	m_vt_cardinfo = vt_cardinfo;

}

CCapConfigDlg::~CCapConfigDlg()
{
}

void CCapConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_FILTER, m_bFilter);
	DDX_Radio(pDX, IDC_RADIO_CPT, m_cap_file_type);
	
	DDX_Control(pDX, IDC_LIST_CARD, m_listbox_card);

}


BEGIN_MESSAGE_MAP(CCapConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_FLT, &CCapConfigDlg::OnBnClickedButtonConfigFlt)
	ON_BN_CLICKED(IDC_BUTTON_FILE_CONFIG, &CCapConfigDlg::OnBnClickedButtonFileConfig)
	ON_BN_CLICKED(IDOK, &CCapConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CAP, &CCapConfigDlg::OnBnClickedButtonStopCap)
	ON_BN_CLICKED(IDCANCEL, &CCapConfigDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_CPT, &CCapConfigDlg::OnBnClickedRadioCpt)
	ON_BN_CLICKED(IDC_RADIO_ETHREAL, &CCapConfigDlg::OnBnClickedRadioEthreal)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCapConfigDlg message handlers
BOOL CCapConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(size_t i = 0; i < m_vt_cardinfo.size(); ++i)
	{
		int index = m_listbox_card.AddString(m_vt_cardinfo[i].c_str());
		m_listbox_card.SetItemData(index,(DWORD)i);
	}

	EnableCtrls();

	return true;
}


void CCapConfigDlg::OnBnClickedButtonConfigFlt()
{
	std::string exe_name = CPF::GetModuleFullFileName("Simflt_edit.exe");

	std::string param = CPF::GetModuleOtherFileName("ini","test_cap.flt");

	std::string cmd_line = exe_name;
	cmd_line += " ";
	cmd_line += param;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	if( !CreateProcess( NULL, // No module name (use command line). 
		(LPSTR)cmd_line.c_str(), // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi ) )            // Pointer to PROCESS_INFORMATION structure.
	{
		AfxMessageBox("不能启动过滤器编辑程序!",MB_OK|MB_ICONERROR);
	}

	return;
}

void CCapConfigDlg::OnBnClickedButtonFileConfig()
{

}



void CCapConfigDlg::OnBnClickedOk()
{
	UpdateData(true);

	std::vector<std::string> vt_cap_card;

	for(int j = 0; j < m_listbox_card.GetCount(); ++j)
	{
		if( m_listbox_card.GetCheck(j) )
		{
			CString item_text;
			m_listbox_card.GetText(j,item_text);

			vt_cap_card.push_back(item_text.GetString());
		}
	}

	if( vt_cap_card.size() == 0 )
	{
		MessageBox("没有选择捕获任何网卡的数据！", "提示");

		return;
	}

	std::string flt_file_name = CPF::GetModuleOtherFileName("ini","test_cap.flt");

	if( m_cap_file_type == 0 )
	{
		if( m_cpt_write_param.strPath.empty() )
		{
			AfxMessageBox("没有输入保存文件路径");
			return;
		}

		if( m_cpt_write_param.strFileName.empty() )
		{
			AfxMessageBox("没有输入文件名");
			return;
		}

		if( !FLUX_CONTROL_StartCapCptData(m_bFilter,flt_file_name.c_str(),
			vt_cap_card,m_cpt_write_param) )
		{
			AfxMessageBox("启动捕获失败！");

			return;
		}
	}
	else
	{
		if( m_ethreal_write_param.strPath.empty() )
		{
			AfxMessageBox("没有输入保存文件路径");
			return;
		}

		if( m_ethreal_write_param.strFileName.empty() )
		{
			AfxMessageBox("没有输入文件名");
			return;
		}

		if( !FLUX_CONTROL_StartCapEthrealData(m_bFilter,flt_file_name.c_str(),
			vt_cap_card,m_ethreal_write_param) )
		{
			AfxMessageBox("启动捕获失败！");

			return;
		}
	}

	m_begin_cap_time = GetTickCount();

	GetDlgItem(IDC_EDIT_TIME)->SetWindowText("0");

	m_bCapping = true;

	SetTimer(1,2000,NULL);

	EnableCtrls();

	return;	
}

void CCapConfigDlg::OnBnClickedButtonStopCap()
{
	KillTimer(1);

	FLUX_CONTROL_StopCapData();

	m_bCapping = false;

	EnableCtrls();

	return;
}

void CCapConfigDlg::OnBnClickedCancel()
{
	if( m_bCapping )
	{
		AfxMessageBox("请先停止捕获！",MB_OK);
		return;
	}

	OnCancel();
}


void CCapConfigDlg::EnableCtrls()
{
	GetDlgItem(IDOK)->EnableWindow(!m_bCapping);
	GetDlgItem(IDC_BUTTON_STOP_CAP)->EnableWindow(m_bCapping);
	GetDlgItem(IDCANCEL)->EnableWindow(!m_bCapping);
	GetDlgItem(IDC_CHECK_FILTER)->EnableWindow(!m_bCapping);

	GetDlgItem(IDC_BUTTON_CONFIG_FLT)->EnableWindow(!m_bCapping);

	GetDlgItem(IDC_RADIO_CPT)->EnableWindow(!m_bCapping);
	GetDlgItem(IDC_RADIO_ETHREAL)->EnableWindow(!m_bCapping);

	return;
}

void CCapConfigDlg::OnBnClickedRadioCpt()
{
	UpdateData(true);

	m_cpt_write_param.strPath = m_str_save_path.GetString();

	CCptDstFileDlg dlg(m_cpt_write_param);

	if( dlg.DoModal() == IDOK )
	{
		m_str_save_path = m_cpt_write_param.strPath.c_str();
	}

	return;
}

void CCapConfigDlg::OnBnClickedRadioEthreal()
{
	UpdateData(true);

	m_ethreal_write_param.strPath = m_str_save_path.GetString();

	CEtherealDstFileDlg dlg(m_ethreal_write_param);

	if( dlg.DoModal() == IDOK )
	{
		m_str_save_path = m_ethreal_write_param.strPath.c_str();
	}

	return;
}

void CCapConfigDlg::OnTimer(UINT_PTR nIDEvent)
{
	DWORD dwcur_tick = GetTickCount();

	DWORD dwDiffSec = (GetTickCount()-m_begin_cap_time)/1000;
	
	GetDlgItem(IDC_EDIT_TIME)->SetWindowText(
		CPF::GetTimeLastInfo(dwDiffSec).c_str());

	CDialog::OnTimer(nIDEvent);

	return;
}

