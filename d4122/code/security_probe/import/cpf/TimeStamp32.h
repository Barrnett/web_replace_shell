#ifndef __TIMESTAMP32_H__20060412__
#define __TIMESTAMP32_H__20060412__

#include "cpf/cpf.h"
#include "cpf/other_tools.h"
#include "ace/Time_Value.h"

typedef struct tagTime_Stamp
{
	ACE_INT32	sec;

	ACE_INT32	ns;

	inline void from_timeval(const struct timeval& tv)
	{
		sec = tv.tv_sec;
		ns = tv.tv_usec*1000;
	}

	inline void from_ace_timevalue(const ACE_Time_Value& tv)
	{
		sec = (ACE_INT32)tv.sec();
		ns = tv.usec()*1000;
	}

	inline void to_timeval(struct timeval& tv) const
	{
		tv.tv_sec = sec;
		tv.tv_usec = ns/1000;
	}

	inline struct timeval to_timeval() const
	{
		struct timeval tv;

		to_timeval(tv);

		return tv;
	}

	inline void to_ace_timevalue(ACE_Time_Value& tv) const
	{
		tv = to_ace_timevalue();
	}

	inline ACE_Time_Value to_ace_timevalue() const
	{
		return ACE_Time_Value(sec,ns/1000);
	}


} Time_Stamp,TIMESTAMP32;


class CPF_CLASS CTimeStamp32
{
public:
	/// Constant "0".
	static const CTimeStamp32 zero;
	static const CTimeStamp32 maxtime;


// ������������
public:
	inline CTimeStamp32(void)
	{
		m_ts.sec = 0;
		m_ts.ns = 0;
	}

	inline CTimeStamp32(ACE_UINT32 nMilliSec)
	{
		m_ts.sec = nMilliSec/1000;
		m_ts.ns = (nMilliSec%1000)*NSS_PER_MS;
	}

	inline CTimeStamp32(const CTimeStamp32 &tt)
	{
		*this = tt;
	}

	inline CTimeStamp32(ACE_INT32 sec, ACE_INT32 ns)
	{
		m_ts.sec = sec;
		m_ts.ns = ns;
	}

	inline CTimeStamp32(const Time_Stamp& ts)
	{
		m_ts = ts;
	}

	inline CTimeStamp32(const struct timeval& tv)
	{
		m_ts.from_timeval(tv);
	}

	inline CTimeStamp32(const ACE_Time_Value& tv)
	{
		m_ts.from_ace_timevalue(tv);
	}

// ����������
public:
	inline CTimeStamp32 &operator = (const CTimeStamp32 &tt)
	{
		m_ts.sec = tt.m_ts.sec;
		m_ts.ns = tt.m_ts.ns;
		return *this;
	}

	inline CTimeStamp32 operator + (const CTimeStamp32 &tt) const
	{
		CTimeStamp32 temp = *this;

		return (temp += tt);
	}

	inline CTimeStamp32 operator - (const CTimeStamp32 &tt) const
	{
		CTimeStamp32 temp = *this;

		return (temp -= tt);
	}

	inline CTimeStamp32 &operator += (const CTimeStamp32 &tt)
	{
		this->m_ts.ns += tt.m_ts.ns;
		this->m_ts.sec += tt.m_ts.sec;

		if (this->m_ts.ns >= NSS_PER_SECOND)
		{
			this->m_ts.ns -= NSS_PER_SECOND;
			++this->m_ts.sec;
		}

		return *this;
	}

	inline CTimeStamp32 &operator -= (const CTimeStamp32 &tt)
	{
		if (this->m_ts.ns >= tt.m_ts.ns)
		{
			this->m_ts.sec -= tt.m_ts.sec ;
			this->m_ts.ns -= tt.m_ts.ns ;
		}
		else
		{
			this->m_ts.sec = this->m_ts.sec - tt.m_ts.sec - 1;
			this->m_ts.ns = this->m_ts.ns + NSS_PER_SECOND - tt.m_ts.ns ;
		}
		return *this;
	}

	inline bool operator == (const CTimeStamp32 &tt) const
	{	return ((this->m_ts.sec==tt.m_ts.sec) && (this->m_ts.ns==tt.m_ts.ns));	}
	inline bool operator != (const CTimeStamp32 &tt) const
	{	return !(((this->m_ts.sec==tt.m_ts.sec) && (this->m_ts.ns==tt.m_ts.ns)));	}

	inline int operator > (const CTimeStamp32& tt) const
	{	return (this->Compare(tt) > 0);	}
	inline int operator >= (const CTimeStamp32& tt) const
	{	return (this->Compare(tt) >= 0);	}
	inline int operator < (const CTimeStamp32& tt) const
	{	return (this->Compare(tt) < 0);	}
	inline int operator <= (const CTimeStamp32& tt) const
	{	return (this->Compare(tt) <= 0);	}


// ��̬�ӿڣ��ȼ���ԭ����api
public:
	// 0 - ��ȣ�1 - pts1>pts2, -1 - pts1<pts2
	inline int Compare(const CTimeStamp32 &ts) const
	{
		if (this->m_ts.sec > ts.m_ts.sec)
			return +1;
		else if (this->m_ts.sec < ts.m_ts.sec)
			return -1;
		else
		{
			if (this->m_ts.ns > ts.m_ts.ns)
				return +1;
			else if(this->m_ts.ns < ts.m_ts.ns)
				return -1;
		}

		return 0;
	}

	inline CTimeStamp32 Div(ACE_INT32 n) const
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

	// ��ʽ���������֧�����룬ʹ��ҪС��
	// ��ʾh:m:s.ns, ����ֵ�п���Ϊ��
	inline const ACE_TCHAR* Format() const
	{	return Format(0);	}

	// ��ʽ���������֧�����룬ʹ��ҪС��
	// ��ʾY/M/D-h:m:s.ns, ����ֵ�п���Ϊ��
	inline const ACE_TCHAR* FormatEx() const
	{	return Format(1);	}

	//type = 0,��ʾh:m:s.ns
	//type = 1,��ʾY/M/D-h:m:s.n
	//type = 2,��ʾh:m:s
	//type = 3,��ʾY/M/D-h:m:s
	//type = 4,��ʾY_M_D
	//type = 5,��ʾY_M_D-h_m_s
	//type = 6,,��ʾY-M-D h:m:s
	inline const ACE_TCHAR* Format(int type) const
	{
		return CPF::FormatTime(type,m_ts.sec,m_ts.ns);
	}

	// ȡ�����д��һ��,���һ��ָ��Ϊ��,���ز�Ϊ�յ�,��������Ϊ��
	inline static const CTimeStamp32& Max(const CTimeStamp32& ts1, const CTimeStamp32& ts2)
	{		
		if( ts1.Compare(ts2) >= 0 )
			return ts1;
		else
			return ts2;
	}
	// ȡ������С��һ��,���һ��ָ��Ϊ��,���ز�Ϊ�յ�,��������Ϊ��
	inline static const CTimeStamp32& Min(const CTimeStamp32& ts1, const CTimeStamp32& ts2)
	{
		if( ts1.Compare(ts2) <= 0 )
			return ts1;
		else
			return ts2;
	}

	// �����������
	inline int SecBetween(const CTimeStamp32 &preTime) const
	{
		return 	(int)(m_ts.sec - preTime.m_ts.sec);
	}
	inline int MilliSecBetween(const CTimeStamp32 &preTime) const  // �����
	{
		int nDist1, nDist2;

		nDist1 = (ACE_INT32)(m_ts.sec - preTime.m_ts.sec) * 1000;
		nDist2 = (ACE_INT32)(m_ts.ns - preTime.m_ts.ns) / NSS_PER_MS;

		return (nDist1 + nDist2);
	}

	//���Ӽ���ms
	inline void IncMs(int ms)
	{
		*this += CTimeStamp32(ms);
	}
	
	// �ж��Ƿ�ʱ
	inline static bool IsTimeOut(const CTimeStamp32 &endTS, const CTimeStamp32 &beginTS, const CTimeStamp32 &pto)
	{
		ACE_ASSERT( endTS.Compare(beginTS) >= 0 );

		return ((endTS - beginTS).Compare(pto) >=0);
	}

	// �ж��Ƿ�ʱ
	inline static bool IsTimeOut(const CTimeStamp32 &endTS, const CTimeStamp32 &beginTS, int sec)
	{
		ACE_ASSERT( endTS.Compare(beginTS) >= 0 );

		return ( (endTS - beginTS).GetSEC() >= sec );
	}


// �ӿڲ������֣�
public:
	inline void SetValue(ACE_INT32 sec, ACE_INT32 ns)
	{
		m_ts.sec = sec;
		m_ts.ns = ns;
	}

	inline void GetValue(ACE_INT32 &sec, ACE_INT32 &ns) const
	{
		sec = m_ts.sec ;
		ns = m_ts.ns;
	}

	inline ACE_INT32 GetSEC() const 
	{
		return m_ts.sec; 
	}

	inline ACE_INT32 GetNS() const 
	{ 
		return m_ts.ns; 
	}

	// ת��������
	inline ACE_UINT64 GetMS() const
	{
		return ((ACE_UINT64)m_ts.sec)*1000 + m_ts.ns/NSS_PER_MS;
	}

public:

	Time_Stamp	m_ts;

	enum{
		//1������NSS_PER_SECOND��ns	
		NSS_PER_SECOND = 1000000000,

		//1ms��NSS_PER_MS��ns	
		NSS_PER_MS = 1000000,
	};
};

#endif // __TIMESTAMP32_H__20060412__
