//////////////////////////////////////////////////////////////////////////
//TIME_QUERY_COND.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS TIME_QUERY_COND
{
public:
	//查询表类型：HOUR_TABLE=小时表，DAY_TABLE=天表，MONTH_TABLE=月表
	typedef enum{

		HOUR_TABLE = 0,
		DAY_TABLE = 1,
		MONTH_TABLE = 2

	}QUERY_TIME_TABLE_TYPE;


	typedef enum{

		self_def = 0,//自定义
		last_day, //最近一天
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

	//按照时间类型：今天，昨天，本周，上周等，得到初始的时间
	//如果cur_time=0,表示使用当前时间
	static void GetInitTime(TIME_SEL_TYPE time_type,
		time_t& lBeg, time_t& lEnd,
		QUERY_TIME_TABLE_TYPE& table_type,
		time_t cur_time=0);

	static void GetInitTime(TIME_SEL_TYPE time_type,
		TIME_QUERY_COND& cond,
		time_t cur_time=0);

	//优化查询：如果前后两次查询的end_time都比当前时间小diff(秒)，则不需要再次查询
	//因为不会有新的数据
	//如果cur_time=0,表示使用当前时间
	//返回true，表示附和优化条件，否则返回false
	BOOL OptimizeQueryTime(const TIME_QUERY_COND& cond,time_t diff,time_t cur_time=0);

public:
	TIME_SEL_TYPE				time_type;
	time_t						begin_time;
	time_t						end_time;


	QUERY_TIME_TABLE_TYPE		table_type;


public:
	//根据时间差，插入空白数据。
	//bFirst:第一个数据是否要插入
	//bLast:最后一个数据是否要插入
	//last_time;返回u最后一个节点的时间
	static int gen_emptydata_time(int table_type,
		time_t end_time,time_t cur_time,
		BOOL bFirst,
		BOOL bLast,
		OUT std::vector<time_t>* vt_empty_time,
		OUT time_t * last_time = NULL);

};
