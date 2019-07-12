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
	//;�Ƿ�Ϊ����ļ���filename_0001,filename_0001����ʾ
	//	;���Ϊ0��ʾ��������n>1��ʾ���ٸ���1��ʾ���������ļ�
	//	numfiles=0

	//	;�ļ������С�����û�л���Ϊ0����һֱ����,���Ϊ4G��
	//	;����������ļ�������ÿ���ļ����������С����λΪMB.
	//	one_filesize_mb=20	

	//	;����೤ʱ��,��λΪ�롣ȱʡΪ0��ʾ��ʱ���޹ء�
	//	;����ļ���С�����ƣ���ʱ����ļ���С˭�����������������κ�һ������ͣ����
	//	captime = 3600
	BOOL init(unsigned int numfiles,unsigned int one_filsize_mb,unsigned int captime);
	void fini();


	//ÿ�洢һ�����ݰ����͵��øú���
	//0��ʾ���Լ�����
	//1��ʾ��ǰ�ļ����ˣ�Ӧ��д��һ���ļ���
	//-2��ʾ��Ϊʱ�䵽�ˣ�Ӧ��ֹͣд
	//-3��ʾ��Ϊ�ļ���С���Ƶ��ˣ�Ӧ��ֹͣд
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
	unsigned int	m_files_finished;//�Ѿ�����˼����ļ�
	ACE_UINT64		m_files_cursize;//��ǰ�ļ��Ĵ�С
	ACE_Time_Value	m_starttime;//��ʼ�����ʱ��
};
