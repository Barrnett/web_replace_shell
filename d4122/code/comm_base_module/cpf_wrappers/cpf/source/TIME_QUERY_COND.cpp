//////////////////////////////////////////////////////////////////////////
//TIME_QUERY_COND.cpp

#include "cpf/TIME_QUERY_COND.h"

BOOL TIME_QUERY_COND::OptimizeQueryTime(const TIME_QUERY_COND& cond,time_t diff,time_t cur_time)
{
	if(cur_time ==0)
	{
		time(&cur_time);
	}

	if( cond.end_time < cur_time && this->end_time < cur_time )
	{
		if( cur_time - cond.end_time < diff )
			return false;

		if( cur_time - this->end_time < diff )
			return false;

		return true;
	}
	else
	{
		return false;
	}
}

void TIME_QUERY_COND::GetInitTime(TIME_SEL_TYPE time_type,
								  time_t& lBeg, time_t& lEnd,
								  TIME_QUERY_COND::QUERY_TIME_TABLE_TYPE& table_type,
								  time_t cur_time)
{
	if( cur_time == 0 )
	{
		time(&cur_time);
	}

	switch(time_type)
	{
	case TIME_QUERY_COND::last_day:	
		{
			lEnd = cur_time;
			lBeg = lEnd-3600*24;
			table_type = HOUR_TABLE;
		}
		break;

	case TIME_QUERY_COND::today:
		{
			tm* pTime = localtime(&cur_time);

			pTime->tm_hour = 0;
			pTime->tm_min = 0;
			pTime->tm_sec = 0;
			lBeg = mktime(pTime);

			lEnd = (lBeg+24*3600);		

			table_type = HOUR_TABLE;
		}
		break;

	case TIME_QUERY_COND::yesterday:
		{
			tm* pTime = localtime(&cur_time);
			pTime->tm_hour = 0;
			pTime->tm_min = 0;
			pTime->tm_sec = 0;
			lEnd = mktime(pTime);
			lBeg = lEnd-3600*24;
			table_type = HOUR_TABLE;
		}
		break;

	case TIME_QUERY_COND::this_week:
		{
			tm* pTime = localtime(&cur_time);
			
			pTime->tm_hour = 0;
			pTime->tm_min = 0;
			pTime->tm_sec = 0;
			lBeg = mktime(pTime);
			lBeg -= (pTime->tm_wday)*3600*24;
			table_type = DAY_TABLE;
			lEnd = lBeg + 7*3600*24;
		}
		break;

	case TIME_QUERY_COND::last_week:
		{
			tm* pTime = localtime(&cur_time);
			pTime->tm_hour = 0;
			pTime->tm_min = 0;
			pTime->tm_sec = 0;
			lEnd = mktime(pTime);
			lEnd -= (pTime->tm_wday)*3600*24;
			lBeg = lEnd-7*3600*24;
			table_type = DAY_TABLE;
		}
		break;

	case TIME_QUERY_COND::this_month:
		{
			tm* pTime = localtime(&cur_time);
			pTime->tm_mday = 1;
			pTime->tm_hour = 0;
			pTime->tm_min = 0;
			pTime->tm_sec = 0;
			lBeg = mktime(pTime);
			table_type = DAY_TABLE;

			if( pTime->tm_mon == 11 )
			{
				pTime->tm_mon = 0;
				pTime->tm_year += 1;
			}
			else
			{
				pTime->tm_mon += 1;
			}

			lEnd = mktime(pTime);
		}
		break;

	case TIME_QUERY_COND::last_month:
		{
			tm* pTime = localtime(&cur_time);
			pTime->tm_mday = 1;
			pTime->tm_hour = 0;
			pTime->tm_min = 0;
			pTime->tm_sec = 0;
			lEnd = mktime(pTime);

			pTime->tm_mon --;
			if(pTime->tm_mon < 0)
			{
				pTime->tm_mon = 11;
				pTime->tm_year --;
			}
			lBeg = mktime(pTime);

			table_type = DAY_TABLE;
		}
		break;

	case TIME_QUERY_COND::this_year:
		{
			tm* pTime = localtime(&cur_time);

			pTime->tm_mon = 0;
			pTime->tm_mday = 1;
			pTime->tm_hour = 0;
			pTime->tm_min = 0;
			pTime->tm_sec = 0;

			lBeg = mktime(pTime);

			pTime->tm_yday += 1;

			lEnd = mktime(pTime);

			table_type = MONTH_TABLE;
		}
		break;

	case TIME_QUERY_COND::last_year:
		{
			tm* pTime = localtime(&cur_time);

			pTime->tm_mon = 0;
			pTime->tm_mday = 1;
			pTime->tm_hour = 0;
			pTime->tm_min = 0;
			pTime->tm_sec = 0;

			lEnd = mktime(pTime);

			pTime->tm_year -= 1;

			lEnd = mktime(pTime);

			table_type = MONTH_TABLE;
		}
		break;

	default:
		ACE_ASSERT(FALSE);
		lBeg = lEnd = 0;
		break;
	}

	return;
}

void TIME_QUERY_COND::GetInitTime(TIME_QUERY_COND::TIME_SEL_TYPE time_type,
						TIME_QUERY_COND& cond,
						time_t cur_time)
{
	cond.time_type = time_type;

	GetInitTime(time_type,
		cond.begin_time,
		cond.end_time,
		cond.table_type,
		cur_time);

	return;
}


//消息表和天表的时间间隔是固定的
static time_t GetStepTime(int table_type)
{
	time_t step_time = 0;

	if( 0 == table_type )
	{
		step_time = (3600);
	}
	else if( 1 == table_type )
	{
		step_time = (24*3600);
	}
	else
	{
		ACE_ASSERT(FALSE);
	}

	return step_time;
}

//根据时间差，插入空白数据。
int TIME_QUERY_COND::gen_emptydata_time(int table_type,
								   time_t end_time,time_t cur_time,
								   BOOL bFirst,
								   BOOL bLast,
								   std::vector<time_t>* vt_empty_time,
								   time_t * las_time)
{
	if( vt_empty_time )
	{
		vt_empty_time->clear();
	}

	if( end_time <= cur_time )
	{
		return 0;
	}

	int count = 0;

	if(table_type == 0||table_type==1)
	{
		time_t step_time = GetStepTime(table_type);

		cur_time /= step_time;
		cur_time *= step_time;

		end_time /= step_time;
		end_time *= step_time;

		count = (int)((end_time-cur_time)/step_time);

		if( bFirst )
		{
			++count;
		}

		if( !bLast )
		{
			--count;
		}

		if(count <= 0 )
		{
			return 0;
		}

		if( vt_empty_time )
		{
			vt_empty_time->reserve(count);
		}

		for(int i = 0; i < count; ++i)
		{
			if( !bFirst )
			{
				cur_time += step_time;
			}

			if( vt_empty_time )
			{
				vt_empty_time->push_back(cur_time);
			}

			if( bFirst )
			{
				cur_time += step_time;
			}

		}
	}
	else if( table_type == 2 )
	{//月表
		struct tm tm_end;

		memcpy(&tm_end,ACE_OS::localtime(&end_time),sizeof(tm_end));

		struct tm tm_cur;

		memcpy(&tm_cur,ACE_OS::localtime(&cur_time),sizeof(tm_cur));

		if( bFirst )
		{
			++count;

			if( vt_empty_time )
				vt_empty_time->push_back(cur_time);
		}

		while(1)
		{
			if(tm_end.tm_year > tm_cur.tm_year)
			{
				if( tm_cur.tm_mon < 11 )
				{
					tm_cur.tm_mon++;
				}
				else
				{
					tm_cur.tm_mon = 0;
					tm_cur.tm_year++;
				}

				cur_time = mktime(&tm_cur);

				++count;

				if( vt_empty_time )
					vt_empty_time->push_back(cur_time);
			}
			else 
			{
				if( bLast )
				{
					if( tm_cur.tm_mon < tm_end.tm_mon )
					{
						tm_cur.tm_mon++;

						cur_time = mktime(&tm_cur);

						++count;

						if( vt_empty_time )
							vt_empty_time->push_back(cur_time);
					}
					else
					{
						break;
					}
				}
				else
				{
					if( tm_cur.tm_mon < tm_end.tm_mon - 1 )
					{
						tm_cur.tm_mon++;

						cur_time = mktime(&tm_cur);

						++count;

						if( vt_empty_time )
							vt_empty_time->push_back(cur_time);
					}
					else
					{
						break;
					}
				}

			}
		}
	}

	if( las_time )
	{
		*las_time = cur_time;
	}

	return count;
}

