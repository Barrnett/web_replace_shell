//////////////////////////////////////////////////////////////////////////
//ACE_MemmapLog_Msg.cpp

#include "cpf/ACE_Memmap_Log_Msg.h"
#include "ace/Mem_Map.h"
#include "ace/Log_Msg_Callback.h"
#include "ace/Log_Record.h"
#include "cpf/other_tools.h"
#include "cpf/path_tools.h"

struct  BUFFER_HEADER
{
	char		head_token[32];
};


class CMemmap_Callback : public ACE_Log_Msg_Callback
{
public:
	CMemmap_Callback();
	virtual ~CMemmap_Callback();

public:
	BOOL init(const char * filename,unsigned int bufsize);
	void fini();

protected:
	void log(ACE_Log_Record& log_record);

private:
	ACE_Mem_Map		m_LiveLogShareMemory;
	char *			m_pBuffer;

	unsigned int	m_nMaxBufSize;
	unsigned int	m_cursize;

	unsigned int	m_nLogNums;

};

CMemmap_Callback::CMemmap_Callback(void)
{
	m_pBuffer = NULL;

	m_cursize = 0;
	m_nMaxBufSize = 0;

	m_nLogNums = 0;
}

CMemmap_Callback::~CMemmap_Callback(void)
{
	fini();
}


BOOL CMemmap_Callback::init(const char * filename,unsigned int bufsize)
{
	if( (int)bufsize <= 0 || filename == NULL || *filename == 0 )
	{
		return false;
	}

	m_nLogNums = 0;

	m_nMaxBufSize = bufsize;

	int nrtn = m_LiveLogShareMemory.map(filename,bufsize,
		O_RDWR | O_CREAT,ACE_DEFAULT_FILE_PERMS,PROT_RDWR,
		ACE_MAP_SHARED);

	if( 0 == nrtn )
	{
		m_pBuffer = (char *)m_LiveLogShareMemory.addr();
		memset(m_pBuffer,0x00,bufsize);

		BUFFER_HEADER * pBufferHeader = (BUFFER_HEADER *)m_pBuffer;

		strcpy(pBufferHeader->head_token,"begin_time=");

		strcat(pBufferHeader->head_token,CPF::get_curtime_string(5));
		pBufferHeader->head_token[sizeof(pBufferHeader->head_token)-1] = '\n';
		pBufferHeader->head_token[sizeof(pBufferHeader->head_token)-2] = '\r';
	}

	m_cursize = 0;

	return (nrtn == 0);
}


void CMemmap_Callback::fini()
{
	m_LiveLogShareMemory.sync();
	m_LiveLogShareMemory.close();

	return;
};

void CMemmap_Callback::log(ACE_Log_Record& log_record)
{
	static const char end_token[] = "\r\n***tail(%u)***\r\n\r\n";

	if( m_pBuffer && log_record.msg_data_len() > 0 )
	{
		if( sizeof(BUFFER_HEADER) 
			+ m_cursize
			+ log_record.msg_data_len() 
			+ sizeof(end_token) + 16 >= m_nMaxBufSize )//16 用于显示整数的字符串需要的空间
		{
			m_cursize = 0;
		}

		memcpy(m_pBuffer+sizeof(BUFFER_HEADER)+m_cursize,
			log_record.msg_data(),log_record.msg_data_len() );

		m_cursize += (unsigned int)log_record.msg_data_len();

		sprintf(m_pBuffer+sizeof(BUFFER_HEADER) + m_cursize,
			end_token,++m_nLogNums);
	}

	
	return;

}


//////////////////////////////////////////////////////////////////////////
//ACE_Memmap_Log_Msg

ACE_Memmap_Log_Msg::ACE_Memmap_Log_Msg()
{
	m_pMemmapCallback = NULL;
}

ACE_Memmap_Log_Msg::~ACE_Memmap_Log_Msg()
{
	fini();
}

BOOL ACE_Memmap_Log_Msg::init(const char * filename,unsigned int bufsize)
{
	if( (int)bufsize <= 0 || filename == NULL || *filename == 0 )
	{
		return false;
	}

	if( m_pMemmapCallback )
	{
		delete m_pMemmapCallback;
		m_pMemmapCallback = NULL;
	}

	m_pMemmapCallback = new CMemmap_Callback;

	if( !m_pMemmapCallback )
	{
		return false;
	}

	if( !m_pMemmapCallback->init(filename,bufsize) )
	{
		delete m_pMemmapCallback;
		m_pMemmapCallback = NULL;

		return false;
	}

	this->set_flags(ACE_Log_Msg::MSG_CALLBACK);
	this->clr_flags(ACE_Log_Msg::STDERR);

	this->msg_callback(m_pMemmapCallback);

	return true;
}

void ACE_Memmap_Log_Msg::fini()
{
	char full_filename[MAX_PATH]={0};

	CPF::GetModuleFileName(full_filename,sizeof(full_filename));

	LPACECTSTR filename = CPF::GetFileNameFromFullName(full_filename);

	MY_ACE_DEBUG(this,(LM_DEBUG,"[Info][%D]normal close program : '%s'!\n",filename));

	this->clr_flags(ACE_Log_Msg::MSG_CALLBACK);

	if( m_pMemmapCallback )
	{
		m_pMemmapCallback->fini();

		delete m_pMemmapCallback;
		m_pMemmapCallback = NULL;
	}

	return;
}

