//////////////////////////////////////////////////////////////////////////
//CPF_Time_Seg.cpp

#include "cpf/CPF_Time_Seg.h"

CPF_Time_Seg::CPF_Time_Seg(void)
{
	m_begin_hour = 0;
	m_begin_min = 0;

	m_end_hour = 0;
	m_end_min = 0;

}

CPF_Time_Seg::CPF_Time_Seg(int begin_hour,int begin_min,
			 int end_hour,int end_min)
{
	set(begin_hour,begin_min,end_hour,end_min);

	return;
}


CPF_Time_Seg::CPF_Time_Seg(const CPF_Time_Seg& other)
{
	*this = other;
}

CPF_Time_Seg::~CPF_Time_Seg(void)
{
}

int CPF_Time_Seg::cmp(int hour,int min) const
{
	if( CPF_Time_Seg::cmp(m_begin_hour,m_begin_min,hour,min) > 0 )
		return -1;

	if( CPF_Time_Seg::cmp(m_end_hour,m_end_min,hour,min) <= 0 )
		return 1;

	return 0;
}

//判断两个时间段是否有交叉
bool CPF_Time_Seg::intercross(const CPF_Time_Seg& other) const
{
	{
		if( 0 == other.cmp(m_begin_hour,m_begin_min) )
			return true;

		int temp_end_hour = this->m_end_hour;
		int temp_end_min = this->m_end_min;

		desc_one_min(temp_end_hour,temp_end_min);

		if( 0 == other.cmp(temp_end_hour,temp_end_min) )
			return true;

	}

	{
		if( 0 == this->cmp(other.m_begin_hour,other.m_begin_min) )
			return true;

		int temp_end_hour = other.m_end_hour;
		int temp_end_min = other.m_end_min;

		desc_one_min(temp_end_hour,temp_end_min);

		if( 0 == this->cmp(temp_end_hour,temp_end_min) )
			return true;

	}

	return false;
}



//////////////////////////////////////////////////////////////////////////
//CPF_Date_Seg
//////////////////////////////////////////////////////////////////////////



CPF_Date_Seg::CPF_Date_Seg(void)
{
	m_begin_month = 0;
	m_begin_date = 0;

	m_end_month = 0;
	m_end_date = 0;
}

CPF_Date_Seg::CPF_Date_Seg(int begin_month,int begin_date,
			 int end_month,int end_date)
{
	set(begin_month,begin_date,end_month,end_date);

	return;
}


CPF_Date_Seg::CPF_Date_Seg(const CPF_Date_Seg& other)
{
	*this = other;
}

CPF_Date_Seg::~CPF_Date_Seg(void)
{
}

int CPF_Date_Seg::cmp(int month,int date) const
{
	if( CPF_Time_Seg::cmp(m_begin_month,m_begin_date,month,date) > 0 )
		return -1;

	if( CPF_Time_Seg::cmp(m_end_month,m_end_date,month,date) < 0 )
		return 1;

	return 0;
}

//判断两个时间段是否有交叉
bool CPF_Date_Seg::intercross(const CPF_Date_Seg& other) const
{
	if( 0 == other.cmp(m_begin_month,m_begin_date) )
		return true;

	if( 0 == other.cmp(m_end_month,m_end_date) )
		return true;

	if( 0 == this->cmp(other.m_begin_month,other.m_begin_date) )
		return true;

	if( 0 == this->cmp(other.m_end_month,other.m_end_date) )
		return true;

	return false;
}
