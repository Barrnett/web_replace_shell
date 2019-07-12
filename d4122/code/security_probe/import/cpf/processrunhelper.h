#ifndef _PROCESS_RUN_HELPER_H_20091203_
#define _PROCESS_RUN_HELPER_H_20091203_

#include "cpf.h"
#include "ostypedef.h"
#include "my_event.h"
#include <string>

class ACE_Process_Mutex;

typedef struct _ST_CMDLINE_OPT_
{
	_ST_CMDLINE_OPT_()
	{ memset(this, 0x0, sizeof(*this));}
	int  iMode;				// 0 == ǰ̨����״̬, 1=back
	char pszMutexName[PATH_MAX];	// ��������mutex������
	char pszSelfName[PATH_MAX];	// ��ǰ���̵�����
	char pszPathName[PATH_MAX];	// ��ǰ��������·��
	char pszConfName[PATH_MAX];	// ��ǰ���õ�����
	int  nProcessID;
    int  nKernelID;         // �󶨵�cpu
	bool bHelp;
}ST_CMDLINE_OPT;


class CPF_CLASS CProcessRunHelper
{
public:
	// bRegSign=true, ע��ctrl+c�ӿ�
	CProcessRunHelper(bool bRegSign = true);
	~CProcessRunHelper(void);

public:

	std::string		m_strCmdLine;
	ST_CMDLINE_OPT* GetOpt();

	void ShowHelp();

	// ���Ͳ�����-1=���ͳ���(��δ֪������ȱ�ٱ�ѡ����)��
	//						0=�����˳�( help, -v,)
	//						1=��������
	int ParseCommand(int argc,char **argv);

	// �ж��Ƿ�Ϊ��̨������ʽ
	bool IsService()	{return m_stOpt.iMode != 0;}

	// �ж��Ƿ���Ҫ���help��Ϣ
	bool IsHelp()			{return m_stOpt.bHelp;}

	// ���б�ʶ����
	void  SetRunFlag(bool bFlag=true)		{ m_bRunFlag = true;}
	bool  GetRunFlag()		{return m_bRunFlag;}

	// ���������
	// return 0 on success; -1 on failure.  If the lock could not be acquired
	// because someone else already had the lock, @c errno is set to @c EBUSY.
	int TryAcquire();


	// �¼���غ�������
	bool Create_Event(const char *strCmdLine );
	void Delete_Event( );

	my_event_t* Create_one_Event(const char *stop_event_name );

	void OnTimer();

	void SetMonitonEvent();
	int CheckStopEvent();


private:
	// �л�����̨����
	void RunAsService();

	// ע����Ϣ
	bool RegisteSignal();

	// ����ϵͳ�˳���Ϣ
	static void ProcessExitSignal(int nSig);

	bool GetNameInfo(char* argv0);

	static bool		m_bRunFlag;				// �Ƿ����б�ʶ
private:

	ST_CMDLINE_OPT	m_stOpt;		// �����ṹ

	ACE_Process_Mutex*	m_pMutex;

	my_event_t*			m_pMonitorEvent;
	my_event_t*			m_pStopEvent;
};



#endif  //_PROCESS_RUN_HELPER_H_20091203_

