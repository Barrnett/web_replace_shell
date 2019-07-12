
// Unierm_Ctrl.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Unierm_Ctrl.h"
#include "Unierm_CtrlDlg.h"
#include "cpf/other_tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUnierm_CtrlApp

BEGIN_MESSAGE_MAP(CUnierm_CtrlApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUnierm_CtrlApp ����

CUnierm_CtrlApp::CUnierm_CtrlApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CUnierm_CtrlApp ����

CUnierm_CtrlApp theApp;


// CUnierm_CtrlApp ��ʼ��

BOOL CUnierm_CtrlApp::InitInstance()
{
	if( !CPF::TestProcMutex_For_Server("CUnierm_CtrlApp.exe") )
	{
		//AfxMessageBox("�����Ѿ�����!",MB_OK|MB_ICONWARNING);

		return false;
	}

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CUnierm_CtrlDlg dlg(m_lpCmdLine);
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
