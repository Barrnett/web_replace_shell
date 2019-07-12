//////////////////////////////////////////////////////////////////////////
//CPF_TimerFileOutputMgr.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/TimeStamp32.h"
#include <fstream>

typedef void (* LPFN_SwitchFileTime)(const CTimeStamp32& curTs,void * param);

class CPF_CLASS CPF_TimerFileOutputMgr
{
public:
	CPF_TimerFileOutputMgr(void);
	virtual ~CPF_TimerFileOutputMgr(void);


public:
	//��Ҫ�����ļ���ժҪ����ļ��ʱ��
	//�ļ������·��,�ļ��ĺ�׺,�ļ���ʵ��������ʱ������ʾ.
	//��ʾ�ļ���ŵ�ƫ�ƣ�ʵ��������ļ���ŵ�id��:file_id_offset+indexfile
	bool Create(const char * pTempPath,const char * pPath,const char * psuf,UINT filenums,int interval,
		const CTimeStamp32& curTs,
		LPFN_SwitchFileTime lpfnSwtichFiletime=NULL,
		void * pParam=NULL,
		int file_id_offset=0);

	void Destroy();

	virtual void WriteDataToFile(UINT indexfile,const void * pData,size_t datalen);

	virtual void OnTimer(const CTimeStamp32& curTs);

	//�������
	virtual BOOL OutputJustNow(const CTimeStamp32& curTs);

private:
	BOOL CreateAnother(const CTimeStamp32& curTs);

	void CloseOldFile();

	void GetOldFileName(char * buffer,UINT index,const CTimeStamp32& curTs);
	void GetNewFileName(char * buffer,UINT index,const CTimeStamp32& curTs);

private:
	std::ofstream *		m_pOutputFile;
	UINT				m_nFileNums;
	int					m_nInterval;
	CTimeStamp32		m_tsLastCreateFileTime;

	std::string			m_strTempPath;				//��ʱ�ļ���·��
	std::string			m_strPath;					//�����ļ���·��

	std::string			m_strSuf;

	LPFN_SwitchFileTime 	m_lpfnSwtichFiletime;
	void *					m_pCallbackParam;

private:
	int						m_file_id_offset;
};

#include "ace/Recursive_Thread_Mutex.h"

class CPF_CLASS CPF_TimerFileOutputMgr_ThreadSafe : public CPF_TimerFileOutputMgr
{
public:
	CPF_TimerFileOutputMgr_ThreadSafe();
	virtual ~CPF_TimerFileOutputMgr_ThreadSafe();

public:
	virtual void WriteDataToFile(UINT indexfile,const void * pData,size_t datalen);

	virtual void OnTimer(const CTimeStamp32& curTs);

private:
	ACE_Recursive_Thread_Mutex	m_tm;

};

