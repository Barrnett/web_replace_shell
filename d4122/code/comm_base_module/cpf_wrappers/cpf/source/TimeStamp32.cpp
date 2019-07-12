#include "ace/OS_NS_time.h"
#include "cpf/TimeStamp32.h"
#include "cpf/other_tools.h"


//////////////////////////////////////////////////////////////////////////
//
//	����������
//
//////////////////////////////////////////////////////////////////////////


const CTimeStamp32 CTimeStamp32::zero;
const CTimeStamp32 CTimeStamp32::maxtime(ACE_INT32_MAX,ACE_INT32_MAX);

/*
// ��Ϊinline�ĺ������ŵ�.h����
CTimeStamp32 CTimeStamp32::Div( ACE_INT32 n) const
{
	CTimeStamp32 t;
	double f1 = (double)m_ts.sec/n;
	double f2 = (double)m_ts.ns/n;

	t.m_ts.ns = (int)((f1 - (int)f1) * NSS_PER_SECOND + f2);
	if (t.m_ts.ns > NSS_PER_SECOND)
	{
		t.m_ts.sec = ((int)f1) + 1;
		t.m_ts.ns -= NSS_PER_SECOND;
	}
	else
	{
		t.m_ts.sec = (int)f1;
	}

	return t;
}

//type = 0,��ʾh:m:s.ns
//type = 1,��ʾY/M/D-h:m:s.n
//type = 2,��ʾh:m:s
//type = 3,��ʾY/M/D-h:m:s
//type = 4,��ʾY_M_D
//type = 5,��ʾY_M_D-h_m_s
//type = 6,,��ʾY-M-D h:m:s
const ACE_TCHAR* CTimeStamp32::Format(int type) const
{
	return CPF::FormatTime(type,m_ts.sec,m_ts.ns);
}

*/