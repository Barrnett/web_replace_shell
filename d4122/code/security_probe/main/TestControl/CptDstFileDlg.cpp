// CptDstFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "CptDstFileDlg.h"
#include "cpf/other_tools.h"


// CCptDstFileDlg dialog

IMPLEMENT_DYNAMIC(CCptDstFileDlg, CDialog)

CCptDstFileDlg::CCptDstFileDlg(Cpt_Write_Param& cpt_write_param,CWnd* pParent /*=NULL*/)
	: CDialog(CCptDstFileDlg::IDD, pParent)
	, m_strSaveFilePath(_T(""))
	, m_strSaveFileName(_T(""))
	, m_max_onefile_size(1024)
	, m_num_files(0)
	, m_nSaveTime(0)
	, m_nBufferSizeMB(0)
	, m_cpt_write_param(cpt_write_param)
{
}

CCptDstFileDlg::~CCptDstFileDlg()
{
}

void CCptDstFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILEPATH, m_strSaveFilePath);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strSaveFileName);
	DDX_Text(pDX, IDC_EDIT_MAX_ONEFILE_SIZE, m_max_onefile_size);
	DDV_MinMaxUInt(pDX, m_max_onefile_size, 0, 4096);
	DDX_Text(pDX, IDC_EDIT_MULTIFILES, m_num_files);
	DDV_MinMaxUInt(pDX, m_num_files, 0, 1024);
	DDX_Text(pDX, IDC_EDIT_SAVE_TIME, m_nSaveTime);
	DDX_Text(pDX, IDC_EDIT_BUFFER_SIZE, m_nBufferSizeMB);
	DDV_MinMaxUInt(pDX, m_nBufferSizeMB, 1, 20);
}


BEGIN_MESSAGE_MAP(CCptDstFileDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCptDstFileDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CCptDstFileDlg::OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDC_BUTTON_DEFALUT_NAME, &CCptDstFileDlg::OnBnClickedButtonDefalutName)
END_MESSAGE_MAP()


// CCptDstFileDlg message handlers

BOOL CCptDstFileDlg::OnInitDialog()
{
	m_strSaveFilePath = m_cpt_write_param.strPath.c_str();
	m_strSaveFileName = m_cpt_write_param.strFileName.c_str();

	if( m_strSaveFileName.IsEmpty() )
	{
		m_strSaveFileName = CPF::FormatTime(5,ACE_OS::gettimeofday().sec());
		m_strSaveFileName += ".cpt";
	}

	m_nSaveTime = m_cpt_write_param.captime;
	m_num_files = m_cpt_write_param.num_files;
	m_max_onefile_size = m_cpt_write_param.one_filesize_mb;
	m_nBufferSizeMB = m_cpt_write_param.buffer_size_mb;

	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CCptDstFileDlg::OnBnClickedButtonBrowser()
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


void CCptDstFileDlg::OnBnClickedOk()
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

	m_cpt_write_param.strPath = m_strSaveFilePath.GetString();
	m_cpt_write_param.strFileName = m_strSaveFileName.GetString();

	m_cpt_write_param.captime = m_nSaveTime;
	m_cpt_write_param.num_files = m_num_files;
	m_cpt_write_param.one_filesize_mb = m_max_onefile_size;
	m_cpt_write_param.buffer_size_mb = m_nBufferSizeMB;

	OnOK();
}

void CCptDstFileDlg::OnBnClickedButtonDefalutName()
{
	m_strSaveFileName = CPF::FormatTime(5,ACE_OS::gettimeofday().sec());
	m_strSaveFileName += ".cpt";

	GetDlgItem(IDC_EDIT_FILENAME)->SetWindowText(m_strSaveFileName.GetString());

	return;
}

