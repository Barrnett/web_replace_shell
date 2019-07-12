//////////////////////////////////////////////////////////////////////////
//WriteFileCtrl.h

#pragma once

#include "cpf/TimeStamp32.h"

class CWriteFileCtrl
{
public:
	CWriteFileCtrl(void);
	~CWriteFileCtrl(void);

public:
	//;是否为多个文件用filename_0001,filename_0001来表示
	//	;如果为0表示无穷多个，n>1表示多少个，1表示不是连续文件
	//	numfiles=0

	//	;文件保存大小，如果没有或者为0，将一直保存,最大为4G。
	//	;如果是连续文件，则是每个文件都是这个大小。单位为MB.
	//	one_filesize_mb=20	

	//	;捕获多长时间,单位为秒。缺省为0表示与时间无关。
	//	;如果文件大小有限制，则看时间和文件大小谁先满足条件。满足任何一个都会停下来
	//	captime = 3600
	BOOL init(unsigned int numfiles,unsigned int one_filsize_mb,unsigned int captime);
	void fini();


	//每存储一个数据包，就调用该函数
	//0表示可以继续，
	//1表示当前文件满了，应该写下一个文件。
	//-2表示因为时间到了，应该停止写
	//-3表示因为文件大小限制到了，应该停止写
	int on_write_packet(unsigned int caplen);

	unsigned int get_finished_nums() const
	{
		return m_files_finished;
	}

private:


	unsigned int	m_config_numfiles;
	unsigned int	m_config_one_filsize;
	unsigned int	m_config_captime;

private:
	unsigned int	m_files_finished;//已经完成了几个文件
	ACE_UINT64		m_files_cursize;//当前文件的大小
	ACE_Time_Value	m_starttime;//开始保存的时间
};
