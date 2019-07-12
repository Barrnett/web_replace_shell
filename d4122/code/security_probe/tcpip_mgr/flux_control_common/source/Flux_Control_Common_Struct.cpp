//////////////////////////////////////////////////////////////////////////
//Flux_Control_Common_Struct.cpp

#include "StdAfx.h"
#include "Flux_Control_Common_Struct.h"
#include "cpf/Data_Stream.h"

void PutStream_TimeQueryCond(char *& pdata,const TIME_QUERY_COND& cond)
{
	CData_Stream_LE::PutUint32(pdata,(ACE_UINT32)cond.begin_time);
	CData_Stream_LE::PutUint32(pdata,(ACE_UINT32)cond.end_time);
	CData_Stream_LE::PutUint32(pdata,(ACE_UINT32)cond.table_type);

	return;

}

void GetStream_TimeQueryCond(const char *& pdata,TIME_QUERY_COND& cond)
{
	cond.begin_time = (time_t)CData_Stream_LE::GetUint32(pdata);
	cond.end_time = (time_t)CData_Stream_LE::GetUint32(pdata);
	cond.table_type = (TIME_QUERY_COND::QUERY_TIME_TABLE_TYPE)(int)CData_Stream_LE::GetUint32(pdata);

	return;

}

int GetProbeToDBTimes(TIME_QUERY_COND::QUERY_TIME_TABLE_TYPE table_type)
{
	switch(table_type)
	{
	case TIME_QUERY_COND::HOUR_TABLE:
		return 12;
		break;

	case TIME_QUERY_COND::DAY_TABLE:
		return 12*24;
		break;

	case TIME_QUERY_COND::MONTH_TABLE:
		return 12*24*30;//�ȴ����
		break;

	default:
		ACE_ASSERT(FALSE);
		return 1;
		break;
	}

	ACE_ASSERT(FALSE);
	return 1;
}

//�õ�ÿ�ֱ�ļ��ʱ�䣬����Сʱ����3600�룬�����24*3600
int GetHistInterval_TimeQueryCond(const TIME_QUERY_COND& cond)
{
	time_t diff = cond.end_time - cond.begin_time;

	if( (int)diff <= 0 )
	{
		ACE_ASSERT(false);
		diff = 1;
	}

	return (int)diff;
}

int GetHistInterval_TableType(TIME_QUERY_COND::QUERY_TIME_TABLE_TYPE table_type)
{
	switch(table_type)
	{
	case TIME_QUERY_COND::HOUR_TABLE:
		return 3600;
		break;

	case TIME_QUERY_COND::DAY_TABLE:
		return 3600*24;
		break;

	case TIME_QUERY_COND::MONTH_TABLE:
		return 3600*24*30;//�ȴ����
		break;

	default:
		ACE_ASSERT(FALSE);
		return 3600;
		break;
	}

	ACE_ASSERT(FALSE);
	return 3600;
}

std::string GetIfDataTypeString(int nIfDataType)
{
	std::string strInfo;

	switch(nIfDataType)
	{
	case IF_DATA_TYPE_AFTER_TRANS:
		strInfo = "ת��������";
		break;

	case IF_DATA_TYPE_BEFORE_TRANS:
		strInfo = "ת��ǰ����";
		break;

	case IF_DATA_TYPE_INNER:
		strInfo = "����������";
		break;

	case IF_DATA_TYPE_OUTTER:
		strInfo = "����������";
		break;

	default:
		ACE_ASSERT(FALSE);
		strInfo = "";
		break;
	}

	return strInfo;
}

