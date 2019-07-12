//////////////////////////////////////////////////////////////////////////
//CPF_Time_Seg.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CPF_Time_Seg
{
public:
	CPF_Time_Seg(void);

	CPF_Time_Seg(int begin_hour,int begin_min,
		int end_hour,int end_min);

	CPF_Time_Seg(const CPF_Time_Seg& other);

	~CPF_Time_Seg(void);

	int operator ==(const CPF_Time_Seg& other) const
	{
		return (m_begin_hour == other.m_begin_hour
			&& m_begin_min == other.m_begin_min
			&& m_end_hour == other.m_end_hour
			&& m_end_min == other.m_end_min );
	}

	CPF_Time_Seg& operator =(const CPF_Time_Seg& other)
	{
		if( this != &other )
		{
			this->set(other.m_begin_hour,other.m_begin_min,
				other.m_end_hour,other.m_end_min);
		}

		return *this;
	}


public:
	//如果包含，返回0
	//如果小于，返回-1
	//如果大于，返回+1
	int cmp(int hour,int min) const;

	//判断两个时间段是否有交叉
	bool intercross(const CPF_Time_Seg& other) const;

public:
	void set(int begin_hour,int begin_min,
		int end_hour,int end_min)
	{
		m_begin_hour = begin_hour;
		m_begin_min = begin_min;

		m_end_hour = end_hour;
		m_end_min = end_min;

		return;
	}

	void get(int& begin_hour,int& begin_min,
		int& end_hour,int& end_min)
	{
		begin_hour = m_begin_hour;
		begin_min = m_begin_min;

		end_hour = m_end_hour;
		end_min = m_end_min;

		return;
	}

	bool is_valid() const
	{
		return (CPF_Time_Seg::cmp(m_begin_hour,m_begin_min,m_end_hour,m_end_min) <=0);
	}

public:
	static void desc_one_min(int& hour,int& min)
	{
		if( min == 0 )
		{
			min = 59;
			--hour;
		}
		else
		{
			--min;
		}
	}
	
	static void inc_one_min(int& hour,int& min)
	{
		if( min == 59 )
		{
			min = 0;
			++hour;
		}
		else
		{
			++min;
		}
	}

public:
	//hour(0-23),min(0-59)
	//小时：分种，采用半闭区间，即[begin,end)
	int m_begin_hour;
	int m_begin_min;

	int m_end_hour;
	int m_end_min;

public:
	static int cmp(int begin_first_data,int begin_second_data,
		int end_first_data,int end_second_data)
	{
		if( begin_first_data < end_first_data )
			return -1;

		if( begin_first_data > end_first_data )
			return 1;

		if( begin_second_data < end_second_data )
			return -1;

		if( begin_second_data > end_second_data )
			return 1;

		return 0;
	}


};



class CPF_CLASS  CPF_Date_Seg
{
public:
	CPF_Date_Seg(void);

	CPF_Date_Seg(int begin_month,int begin_date,
		int end_month,int end_date);

	CPF_Date_Seg(const CPF_Date_Seg& other);

	~CPF_Date_Seg(void);

	int operator ==(const CPF_Date_Seg& other) const
	{

		return (m_begin_month == other.m_begin_month
			&& m_begin_date == other.m_begin_date
			&& m_end_month == other.m_end_month
			&& m_end_date == other.m_end_date );
	}

	CPF_Date_Seg& operator =(const CPF_Date_Seg& other)
	{
		if( this != &other )
		{
			this->set(other.m_begin_month,other.m_begin_date,
				other.m_end_month,other.m_end_date);
		}

		return *this;
	}


	//如果包含，返回0
	//如果小于，返回-1
	//如果大于，返回+1
	int cmp(int month,int date) const;

	//判断两个时间段是否有交叉
	bool intercross(const CPF_Date_Seg& other) const;

public:
	void set(int begin_month,int begin_date,
		int end_month,int end_date)
	{
		m_begin_month = begin_month;
		m_begin_date = begin_date;

		m_end_month = end_month;
		m_end_date = end_date;

		return;
	}

	void get(int& begin_month,int& begin_date,
		int& end_month,int& end_date)
	{
		begin_month = m_begin_month;
		begin_date = m_begin_date;

		end_month = m_end_month;
		end_date = m_end_date;

		return;
	}

	bool is_valid() const
	{
		return (CPF_Time_Seg::cmp(m_begin_month,m_begin_date,m_end_month,m_end_date) <=0);
	}

public:
	//month(0-11.January = 0),date(1-31)
	//小时：分种，采用全闭区间，即[begin,end]
	int m_begin_month;
	int m_begin_date;

	int m_end_month;
	int m_end_date;

};