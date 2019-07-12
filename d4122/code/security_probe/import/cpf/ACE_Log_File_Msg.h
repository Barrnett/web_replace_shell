//////////////////////////////////////////////////////////////////////////
//ACE_Log_File_Msg.h

#pragma once

#include "cpf/cpf.h"
#include "ace/Log_Msg.h"

class CPeriod_File_Callback;

//��log���ļ���
class CPF_CLASS ACE_Log_File_Msg : public ACE_Log_Msg
{
public:
	ACE_Log_File_Msg(void);
	virtual ~ACE_Log_File_Msg(void);

	//interval���೤ʱ������һ���µ��ļ�����λ�Ƿ��ӣ��µ��ļ�������ǰ���ļ��������ϼ���ʱ�䣩��
	//���interval==0����ʾ�������µ��ļ���һ��һ��:interval=24*60
	//bClearAll:�Ƿ������ǰ��log�ļ���
	//���filename==NULL,��ʹ��ִ�г�������+".log".
	BOOL init(const char * filename,BOOL bClearAll=false,unsigned int interval = 0, BOOL bEnterThreadMutex=false, int iHistory = 0);

	//����path/appname.suf��log�ļ�, ���path=NULL, ��ʾ�ڵ�ǰĿ¼�����suf=NULL,��ʾ��Ҫ��׺
	BOOL init_use_appname(const char * path,const char * suf,BOOL bClearAll=false,unsigned int interval = 0,BOOL bEnterThreadMutex=false , int iHistory = 30);

	void fini();

	BOOL init_oneday_log(const char * filename,BOOL bClearAll=false,BOOL bEnterThreadMutex=false, int iHistory = 30)
	{
		return init(filename,bClearAll,24*60,bEnterThreadMutex, iHistory);
	}

	BOOL init_onehour_log(const char * filename,BOOL bClearAll=false,BOOL bEnterThreadMutex=false, int iHistory = 15)
	{
		return init(filename,bClearAll,60,bEnterThreadMutex, iHistory);
	}

private:
	BOOL Inner_init(const char * filename,BOOL bClearAll,unsigned int interval,BOOL bEnterThreadMutex=false, int iHistory=0);

private:
	CPeriod_File_Callback *	m_pPeriod_File_Callback;

};



#include "ace/Singleton.h"
#include "ace/Null_Mutex.h"

class CPF_CLASS ACE_Log_File_Msg_Singleton : public ACE_Singleton<ACE_Log_File_Msg, ACE_Null_Mutex>
{
public:
	static bool m_bFirst;

	static ACE_Log_File_Msg* instance();
	static ACE_Log_File_Msg* instance_init_use_appname(BOOL bClearAll=false,unsigned int interval = 0, BOOL bEnterThreadMutex=true, int iHistory = 30);
	static ACE_Log_File_Msg* instance_init_file(const char* pszFilePath,BOOL bClearAll=false,unsigned int interval = 0, BOOL bEnterThreadMutex=true, int iHistory = 30);
};

