// EtherealDstFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "EtherealDstFileDlg.h"
#include "cpf/other_tools.h"

// CEtherealDstFileDlg dialog

IMPLEMENT_DYNAMIC(CEtherealDstFileDlg, CDialog)

CEtherealDstFileDlg::CEtherealDstFileDlg(Ethereal_Write_Param& ethreal_write_param,CWnd* pParent /*=NULL*/)
	: CDialog(CEtherealDstFileDlg::IDD, pParent)
	, m_strSaveFilePath(_T(""))
	, m_strSaveFileName(_T(""))
	, m_max_onefile_size(0)
	, m_num_files(0)
	, m_nSaveTime(0)
	, m_ethreal_write_param(ethreal_write_param)
{
}

CEtherealDstFileDlg::~CEtherealDstFileDlg()
{
}


void CEtherealDstFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILEPATH, m_strSaveFilePath);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strSaveFileName);
	DDX_Text(pDX, IDC_EDIT_MAX_ONEFILE_SIZE, m_max_onefile_size);
	DDV_MinMaxUInt(pDX, m_max_onefile_size, 0, 4096);
	DDX_Text(pDX, IDC_EDIT_MULTIFILES, m_num_files);
	DDV_MinMaxUInt(pDX, m_num_files, 0, 1024);
	DDX_Text(pDX, IDC_EDIT_SAVE_TIME, m_nSaveTime);
}



BEGIN_MESSAGE_MAP(CEtherealDstFileDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CEtherealDstFileDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CEtherealDstFileDlg::OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDC_BUTTON_DEFALUT_NAME, &CEtherealDstFileDlg::OnBnClickedButtonDefalutName)
END_MESSAGE_MAP()


// CEtherealDstFileDlg message handlers

BOOL CEtherealDstFileDlg::OnInitDialog()
{
	m_strSaveFilePath = m_ethreal_write_param.strPath.c_str();

	m_strSaveFileName = m_ethreal_write_param.strFileName.c_str();

	if( m_strSaveFileName.IsEmpty() )
	{
		m_strSaveFileName = CPF::FormatTime(5,ACE_OS::gettimeofday().sec());
		m_strSaveFileName += ".cap";
	}

	m_nSaveTime = m_ethreal_write_param.captime;
	m_num_files = m_ethreal_write_param.num_files;
	m_max_onefile_size = m_ethreal_write_param.one_filesize_mb;


	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CEtherealDstFileDlg::OnBnClickedButtonBrowser()
{
	// Part of the Code
	BROWSEINFO bi = {0};

	char lpTitle[MAX_PATH] = {0};
	char lpDisp[MAX_PATH] = {0};

	strcpy(lpTitle, "请选择文件夹。");

	bi.hwndOwner = this->GetSafeHwnd();
	bi.lpszTitle = lpTitle;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = lpDisp;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	LPITEMIDLIST lpItem = SHBrowseForFolder(&bi);
	if(lpItem != NULL)
	{
		SHGetPathFromIDList(lpItem, lpDisp);

		GetDlgItem(IDC_EDIT_FILEPATH)->SetWindowText(lpDisp);
		m_strSaveFilePath = lpDisp;
	}

	return;
}


void CEtherealDstFileDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if(m_strSaveFilePath.IsEmpty())
	{
		AfxMessageBox("路径不能为空");
		return;
	}

	if(m_strSaveFileName.IsEmpty())
	{
		AfxMessageBox("文件名不能为空");
		return;
	}

	m_ethreal_write_param.strPath = m_strSaveFilePath.GetString();
	m_ethreal_write_param.strFileName = m_strSaveFileName.GetString();

	m_ethreal_write_param.captime = m_nSaveTime;
	m_ethreal_write_param.num_files = m_num_files;
	m_ethreal_write_param.one_filesize_mb = m_max_onefile_size;

	OnOK();
}

void CEtherealDstFileDlg::OnBnClickedButtonDefalutName()
{
	m_strSaveFileName = CPF::FormatTime(5,ACE_OS::gettimeofday().sec());
	m_strSaveFileName += ".cap";

	GetDlgItem(IDC_EDIT_FILENAME)->SetWindowText(m_strSaveFileName.GetString());

	return;
}
