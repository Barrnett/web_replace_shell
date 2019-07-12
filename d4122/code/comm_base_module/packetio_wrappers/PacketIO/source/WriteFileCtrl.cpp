//////////////////////////////////////////////////////////////////////////
//WriteFileCtrl.cpp

#include "WriteFileCtrl.h"

CWriteFileCtrl::CWriteFileCtrl(void)
{
	m_config_numfiles = 1;
	m_config_one_filsize = 100*1024*1024;
	m_config_captime = 0;

	m_files_finished = 0;
	m_files_cursize = 0;
}

CWriteFileCtrl::~CWriteFileCtrl(void)
{
}


BOOL CWriteFileCtrl::init(unsigned int numfiles,unsigned int one_filsize_mb,unsigned int captime)
{
	m_config_numfiles = numfiles;
	m_config_one_filsize = one_filsize_mb*1024*1024;
	m_config_captime = captime;

	m_files_finished = 0;
	m_files_cursize = 0;

	m_starttime = ACE_OS::gettimeofday();

	return true;
}

void CWriteFileCtrl::fini()
{
	return;
}



//0表示可以继续，
//1表示当前文件满了，应该写下一个文件。
//-2表示因为时间到了，应该停止写
//-33表示因为文件大小限制到了，应该停止写
int CWriteFileCtrl::on_write_packet(unsigned int caplen)
{
	m_files_cursize += caplen;

	if( m_config_one_filsize != 0 
		&& m_files_cursize >= m_config_one_filsize )
	{
		++m_files_finished;

		if( m_files_finished >= m_config_numfiles )
		{
			return -3;
		}
		else
		{
			m_files_cursize = 0;
			return 1;
		}
	}

	if( m_config_captime != 0 
		&& (unsigned int)(ACE_OS::gettimeofday()-m_starttime).sec() > m_config_captime )
	{
		return -2;
	}

	return 0;
}

