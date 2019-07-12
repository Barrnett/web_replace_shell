//////////////////////////////////////////////////////////////////////////
//TIME_QUERY_COND.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS TIME_QUERY_COND
{
public:
	//��ѯ�����ͣ�HOUR_TABLE=Сʱ��DAY_TABLE=���MONTH_TABLE=�±�
	typedef enum{

		HOUR_TABLE = 0,
		DAY_TABLE = 1,
		MONTH_TABLE = 2

	}QUERY_TIME_TABLE_TYPE;


	typedef enum{

		self_def = 0,//�Զ���
		last_day, //���һ��
		today ,
		yesterday,
		this_week,
		last_week,
		this_month,
		last_month,
		this_year,
		last_year,

	}TIME_SEL_TYPE;

public:
	TIME_QUERY_COND()
	{
		time_type = TIME_QUERY_COND::last_day;
		begin_time = end_time = 0;
		table_type = TIME_QUERY_COND::DAY_TABLE;
	}

	TIME_QUERY_COND(TIME_SEL_TYPE ttype,
		time_t begin,time_t end,
		QUERY_TIME_TABLE_TYPE type)
		:time_type(ttype),begin_time(begin),end_time(end),table_type(type)
	{
	}

	TIME_QUERY_COND(const TIME_QUERY_COND& cond)
	{
		*this = cond;
	}

	TIME_QUERY_COND& operator = (const TIME_QUERY_COND& cond)
	{
		if( this != &cond )
		{
			time_type = cond.time_type;
			begin_time = cond.begin_time;
			end_time = cond.end_time;
			table_type = cond.table_type;
		}

		return *this;		
	}

	friend int operator == (const TIME_QUERY_COND& cond1,const TIME_QUERY_COND& cond2)
	{
		return (cond1.begin_time == cond2.begin_time
			&& cond1.end_time == cond2.end_time
			&& cond1.table_type == cond2.table_type
			&& cond1.time_type == cond2.time_type);
	}

	friend int operator != (const TIME_QUERY_COND& cond1,const TIME_QUERY_COND& cond2)
	{
		return (!(cond1==cond2));
	}

	//����ʱ�����ͣ����죬���죬���ܣ����ܵȣ��õ���ʼ��ʱ��
	//���cur_time=0,��ʾʹ�õ�ǰʱ��
	static void GetInitTime(TIME_SEL_TYPE time_type,
		time_t& lBeg, time_t& lEnd,
		QUERY_TIME_TABLE_TYPE& table_type,
		time_t cur_time=0);

	static void GetInitTime(TIME_SEL_TYPE time_type,
		TIME_QUERY_COND& cond,
		time_t cur_time=0);

	//�Ż���ѯ�����ǰ�����β�ѯ��end_time���ȵ�ǰʱ��Сdiff(��)������Ҫ�ٴβ�ѯ
	//��Ϊ�������µ�����
	//���cur_time=0,��ʾʹ�õ�ǰʱ��
	//����true����ʾ�����Ż����������򷵻�false
	BOOL OptimizeQueryTime(const TIME_QUERY_COND& cond,time_t diff,time_t cur_time=0);

public:
	TIME_SEL_TYPE				time_type;
	time_t						begin_time;
	time_t						end_time;


	QUERY_TIME_TABLE_TYPE		table_type;


public:
	//����ʱ������հ����ݡ�
	//bFirst:��һ�������Ƿ�Ҫ����
	//bLast:���һ�������Ƿ�Ҫ����
	//last_time;����u���һ���ڵ��ʱ��
	static int gen_emptydata_time(int table_type,
		time_t end_time,time_t cur_time,
		BOOL bFirst,
		BOOL bLast,
		OUT std::vector<time_t>* vt_empty_time,
		OUT time_t * last_time = NULL);

};
