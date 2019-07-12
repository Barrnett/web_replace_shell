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



//0��ʾ���Լ�����
//1��ʾ��ǰ�ļ����ˣ�Ӧ��д��һ���ļ���
//-2��ʾ��Ϊʱ�䵽�ˣ�Ӧ��ֹͣд
//-33��ʾ��Ϊ�ļ���С���Ƶ��ˣ�Ӧ��ֹͣд
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

