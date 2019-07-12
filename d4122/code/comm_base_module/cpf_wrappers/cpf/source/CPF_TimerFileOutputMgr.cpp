//////////////////////////////////////////////////////////////////////////
//CPF_TimerFileOutputMgr.cpp


#include "cpf/CPF_TimerFileOutputMgr.h"
#include "cpf/path_tools.h"


//////////////////////////////////////////////////////////////////////////

CPF_TimerFileOutputMgr::CPF_TimerFileOutputMgr(void)
{
	m_pOutputFile = 0;
	m_nFileNums = 0;
	m_nInterval = 0;

	m_tsLastCreateFileTime.m_ts.sec = 0;

	m_lpfnSwtichFiletime = NULL;
	m_pCallbackParam = NULL;

	m_file_id_offset = 0;
}

CPF_TimerFileOutputMgr::~CPF_TimerFileOutputMgr(void)
{
	Destroy();
}


bool CPF_TimerFileOutputMgr::Create(const char * pTempPath,const char * pPath,const char * psuf,
								 UINT filenums,int interval,
								 const CTimeStamp32& curTs,
								 LPFN_SwitchFileTime lpfnSwtichFiletime,
								 void * pParam,
								 int file_id_offset)
{
	Destroy();

	if (! CPF::IsDirectory(pTempPath))
		CPF::CreateFullDirecory(pTempPath, 0);
	if (! CPF::IsDirectory(pPath))
		CPF::CreateFullDirecory(pPath, 0);


	m_strTempPath = pTempPath;
	CPF::AddLastFileNameSep(m_strTempPath);

	m_strPath = pPath;
	CPF::AddLastFileNameSep(m_strPath);

	m_strSuf = psuf;

	m_nFileNums = filenums;

	m_nInterval = interval;

	m_lpfnSwtichFiletime = lpfnSwtichFiletime;
	m_pCallbackParam = pParam;

	m_file_id_offset = file_id_offset;

	return CreateAnother(curTs);

}

void CPF_TimerFileOutputMgr::Destroy()
{
	if( m_pOutputFile )
	{
		CloseOldFile();

		delete []m_pOutputFile;		

		m_pOutputFile = NULL;
	}

}


void CPF_TimerFileOutputMgr::WriteDataToFile(UINT indexfile,const void * pData,size_t datalen)
{
	ACE_ASSERT(indexfile<m_nFileNums);

	if( indexfile >= m_nFileNums )
		return;

	if( datalen > 0 )
	{
		m_pOutputFile[indexfile].write((const char *)pData,(std::streamsize)datalen);
	}
}

void CPF_TimerFileOutputMgr::OnTimer(const CTimeStamp32& curTs)
{	
	if( m_nFileNums > 0 )
	{//如果m_nFileNums<=0表示没有创建。

		if( curTs < m_tsLastCreateFileTime )
		{//时间逆转了

			CreateAnother(curTs);
		}
		else
		{
			if( (m_tsLastCreateFileTime.GetSEC() != curTs.GetSEC() && (curTs.GetSEC() % m_nInterval ==  0) )
				|| curTs.SecBetween(m_tsLastCreateFileTime) > m_nInterval + 1 )
			{
				CreateAnother(curTs);
			}
		}
	}

	return;
}

BOOL CPF_TimerFileOutputMgr::OutputJustNow(const CTimeStamp32& curTs)
{
	return CreateAnother(curTs);
}


BOOL CPF_TimerFileOutputMgr::CreateAnother(const CTimeStamp32& curTs)
{
	if( curTs.GetSEC() == 0 )
	{
		return true;
	}

	if( m_pOutputFile && m_lpfnSwtichFiletime )
	{
		(*m_lpfnSwtichFiletime)(curTs,m_pCallbackParam);
	}

	CloseOldFile();

	if( !m_pOutputFile )
	{
		m_pOutputFile = new std::ofstream[m_nFileNums];

		if( !m_pOutputFile )
			return false;
	}

	char buf[1024];

	for(UINT i = 0; i < m_nFileNums; ++i)
	{
		GetOldFileName(buf,i,curTs);

		m_pOutputFile[i].open(buf,std::ios::binary|std::ios::out);

		if( !m_pOutputFile[i].good() )
			return false;
	}

	m_tsLastCreateFileTime = curTs;

	return true;
}

void CPF_TimerFileOutputMgr::CloseOldFile()
{
	if( m_pOutputFile )
	{
		char oldname[1024];
		char newname[1024];

		for( UINT i = 0; i < m_nFileNums; i++ )
		{			
			m_pOutputFile[i].close();

			GetOldFileName(oldname,i,m_tsLastCreateFileTime);
			GetNewFileName(newname,i,m_tsLastCreateFileTime);

			ACE_OS::unlink(newname);

			 ACE_OFF_T filesize = ACE_OS::filesize(oldname);
			
			 if(filesize <= 0)
			{
				ACE_OS::unlink(oldname);
			}
			else
			{
				ACE_OS::rename(oldname,newname);
				ACE_OS::unlink(oldname);
			}
		}
	}
}

void CPF_TimerFileOutputMgr::GetOldFileName(char * buffer,UINT index,const CTimeStamp32& curTs)
{	
	ACE_OS::sprintf(buffer,"%s%d_%s.~",m_strTempPath.c_str(),index+m_file_id_offset,curTs.Format(5));

}

void CPF_TimerFileOutputMgr::GetNewFileName(char * buffer,UINT index,const CTimeStamp32& curTs)
{
	if( m_strSuf.empty() )
	{
		ACE_OS::sprintf(buffer,"%s%d_%s",
			m_strPath.c_str(),
			index+m_file_id_offset,
			curTs.Format(5));
	}
	else
	{
		ACE_OS::sprintf(buffer,"%s%d_%s.%s",
			m_strPath.c_str(),
			index+m_file_id_offset,
			curTs.Format(5),
			m_strSuf.c_str());
	}
}



//////////////////////////////////////////////////////////////////////////
//CPF_TimerFileOutputMgr_ThreadSafe
//////////////////////////////////////////////////////////////////////////

CPF_TimerFileOutputMgr_ThreadSafe::CPF_TimerFileOutputMgr_ThreadSafe()
{

}

CPF_TimerFileOutputMgr_ThreadSafe::~CPF_TimerFileOutputMgr_ThreadSafe()
{

}


void CPF_TimerFileOutputMgr_ThreadSafe::WriteDataToFile(UINT indexfile,const void * pData,size_t datalen)
{
	m_tm.acquire();

	CPF_TimerFileOutputMgr::WriteDataToFile(indexfile,pData,datalen);

	m_tm.release();
}

void CPF_TimerFileOutputMgr_ThreadSafe::OnTimer(const CTimeStamp32& curTs)
{
	m_tm.acquire();

	CPF_TimerFileOutputMgr::OnTimer(curTs);

	m_tm.release();
}


