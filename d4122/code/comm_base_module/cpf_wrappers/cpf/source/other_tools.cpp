#include "cpf/other_tools.h"
#include "cpf/strtools.h"
#include "string.h"
#include "cpf/path_tools.h"
#include <fstream>
#include "cpf/CPF_MD5.h"
#include "ace/Process.h"
#include "cpf/ostypedef.h"
#include "cpf/my_event.h"

#ifdef OS_WINDOWS
#	include "shellapi.h"
#endif

char* CPF::itoa(int val)
{
	static char buf[32];
	const unsigned int radix = 10;
	char* p;
	unsigned int a; //every digit
	int len;
	char* b; //start of the digit char
	char temp;
	unsigned int u;

	p = buf;
	if (val < 0)
	{
		*p++ = '-';
		val = 0 - val;
	}

	u = (unsigned int)val;
	b = p;
	do
	{
		a = u % radix;
		u /= radix;
		*p++ = a + '0';
	} while (u > 0);

	len = (int)(p - buf);
	*p-- = 0;
	//swap

	do
	{
		temp = *p;
		*p = *b;
		*b = temp;
		--p;
		++b;
	} while (b < p);

	return buf;
}




ACE_TCHAR * CPF::linux_ultoa(ULONG num, ACE_TCHAR * str, int radix)
{
	ACE_ASSERT(radix >= 2 && radix <= 36);

	*str = ACE_TEXT('\0');

	if (0 == num)
	{
		ACE_OS::strcpy(str, ACE_TEXT("0"));
	}

	while (num) 
	{
		ULONG i = num % (ULONG)radix;
		num = num / (ULONG)radix;

		ACE_TCHAR ch = 0;

		if (i >= 0 && i <= 9)
		{
			ch = static_cast<ACE_TCHAR>(i | 0x30);
		}

		if ( i > 9 && i < 36)
		{
			ch = static_cast<ACE_TCHAR>(i + 55);
		}

		ACE_OS::memmove(str+1, str, (ACE_OS::strlen(str)+1)*sizeof(ACE_TCHAR));
		*str = ch;
	}

	return str;	
}

char * CPF::str_upr(char * pData)
{
	return CPF::str_upr(pData,ACE_OS::strlen(pData));
}
char * CPF::str_upr(std::string& string_data)
{
	return CPF::str_upr((char *)string_data.c_str(),string_data.size());
}

char * CPF::str_upr(char * pData,size_t nchars)
{
	for(size_t i = 0; i < nchars; i++ )
	{
		pData[i] = static_cast<char>(ACE_OS::ace_toupper(pData[i]));
	}

	return pData;
}

wchar_t * CPF::str_upr(wchar_t * pData)
{
	return CPF::str_upr(pData,ACE_OS::strlen(pData));
}
wchar_t * CPF::str_upr(std::wstring& wstring_data)
{
	return CPF::str_upr((wchar_t *)wstring_data.c_str(),wstring_data.size());
}

wchar_t * CPF::str_upr(wchar_t * pData,size_t nwchars)
{
	for(size_t i = 0; i < nwchars; i++ )
	{
		pData[i] = static_cast<wchar_t>(ACE_OS::ace_towupper(pData[i]));
	}

	return pData;
}

char * CPF::str_lwr(char * pData)
{
	return CPF::str_lwr(pData,ACE_OS::strlen(pData));
}
char * CPF::str_lwr(std::string& string_data)
{
	return CPF::str_lwr((char *)string_data.c_str(),string_data.size());
}

char * CPF::str_lwr(char * pData,size_t nchars)
{
	for(size_t i = 0; i < nchars; i++ )
	{
		pData[i] = static_cast<char>(ACE_OS::ace_tolower(pData[i]));
	}

	return pData;
}

wchar_t * CPF::str_lwr(wchar_t * pData)
{
	return CPF::str_lwr(pData,ACE_OS::strlen(pData));
}
wchar_t * CPF::str_lwr(std::wstring& wstring_data)
{
	return CPF::str_lwr((wchar_t *)wstring_data.c_str(),wstring_data.size());
}

wchar_t * CPF::str_lwr(wchar_t * pData,size_t nwchars)
{
	for(size_t i = 0; i < nwchars; i++ )
	{
		pData[i] = static_cast<wchar_t>(ACE_OS::ace_towlower(pData[i]));
	}

	return pData;
}

const ACE_TCHAR* CPF::get_curtime_string(int type)
{
	ACE_Time_Value curtime = ACE_OS::gettimeofday();

	return CPF::FormatTime(type,(int)curtime.sec(),curtime.usec()*1000);	
}



//windows下从 a 变成 a.dll
//linux下从a 变成liba.so

ACE_TCHAR * CPF::get_dll_name(ACE_TCHAR* longname,const ACE_TCHAR * shortname)
{
#ifdef OS_WINDOWS
	const char suf[] = ACE_TEXT(".dll");	
	longname[0] = 0;
#else
	const char suf[] = ACE_TEXT(".so");
	strcpy(longname,ACE_TEXT("lib"));
#endif

	ACE_OS::strcat(longname,shortname);
	ACE_OS::strcat(longname,suf);

	return longname;
}

void CPF::string_value_to_a(const char * pMD5Value,int len,ACE_TCHAR * outstring,int upper )
{
	int off = 0;

	for( int i = 0; i < len; ++i )
	{
		if( upper )
			ACE_OS::sprintf(outstring + off,"%02X",pMD5Value[i]);
		else
			ACE_OS::sprintf(outstring + off,"%02x",pMD5Value[i]);

		off += 2;
	}
}

void CPF::string_a_to_value(const ACE_TCHAR* instring,int stringlen,unsigned char * poutvalue)
{
	int off = 0;

	while( off < stringlen )
	{
		unsigned int tmpvalue = 0;

		::sscanf(instring+off,"%02X",&tmpvalue);
	
		*poutvalue++ = (unsigned char)tmpvalue;

		off += 2;
	}
}


//type = 0,显示h:m:s.ns
//type = 1,显示Y/M/D-h:m:s.n
//type = 2,显示h:m:s
//type = 3,显示Y/M/D-h:m:s
//type = 4,显示Y_M_D
//type = 5,显示Y_M_D-h_m_s
//type = 6,显示Y-M-D h:m:s
//type = 7,显示Y-M-D
//type = 8,显示Y-M-D h
//type = 9,显示Y-M-D h:m
const ACE_TCHAR* CPF::FormatTime(int type,int sec,int ns)
{
	static ACE_TCHAR buffer[100];

	time_t tmpTime = sec;

	tm* pTm = ACE_OS::localtime((time_t *)&tmpTime);
	if ( NULL == pTm )
	{
		tmpTime = 0;
		pTm = ACE_OS::localtime((time_t *)&tmpTime);
	}

	switch(type)
	{
	case 0:
		ACE_OS::sprintf(buffer, "%02d:%02d:%02d.%09u", 
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec, ns);
		break;

	case 1:
		ACE_OS::sprintf(buffer, "%d/%02d/%02d-%02d:%02d:%02d.%09u", 
			pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday,
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec,ns);
		break;

	case 2:
		ACE_OS::sprintf(buffer, "%02d:%02d:%02d", 
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
		break;

	case 3:
		ACE_OS::sprintf(buffer, "%d/%02d/%02d-%02d:%02d:%02d", 
			pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday,
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
		break;

	case 4:
		ACE_OS::sprintf(buffer, "%02d_%02d_%02d", 
			pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday);
		break;

	case 5:
		ACE_OS::sprintf(buffer, "%d_%02d_%02d-%02d_%02d_%02d", 
			pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday,
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
		break;

	case 6:
		ACE_OS::sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d", 
			pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday,
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
		break;

	case 7:
		ACE_OS::sprintf(buffer, "%d-%02d-%02d", 
			pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday);
		break;

	case 8:
		ACE_OS::sprintf(buffer, "%d-%02d-%02d %02d", 
			pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday,
			pTm->tm_hour);
		break;

	case 9:
		ACE_OS::sprintf(buffer, "%d-%02d-%02d %02d:%02d", 
			pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday,
			pTm->tm_hour,pTm->tm_min);
		break;

	default:
		*(int *)buffer = 0;
		break;

	}

	return buffer;
}


const ACE_TCHAR* CPF::FormatTime(int type,time_t the_time)
{
	return FormatTime(type,(int)the_time,0);
}


//和FormatTime是相反的函数。从字符串得到数据
BOOL CPF::StrToTime(int type,const char* strTime,int& sec,int& ns)
{ 
	if( !strTime || !strTime[0] )
	{
		return false;
	}

	struct tm the_tm;

	memset(&the_tm,0x00,sizeof(the_tm));

	switch(type)
	{
	case 0:
		{
			time_t cur_time;
			time(&cur_time);

			struct tm * pTm = localtime(&cur_time);

			memcpy(&the_tm,pTm,sizeof(the_tm));
			the_tm.tm_mon += 1;
			the_tm.tm_year += 1900;
			
			sscanf(strTime,"%02d:%02d:%02d.%09u",
				&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec, &ns);
		}
		break;

	case 1:
		sscanf(strTime,"%d/%02d/%02d-%02d:%02d:%02d.%09u", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday,
			&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec,&ns);
		break;

	case 2:
		{
			time_t cur_time;
			time(&cur_time);

			struct tm * pTm = localtime(&cur_time);

			memcpy(&the_tm,pTm,sizeof(the_tm));
			the_tm.tm_mon += 1;
			the_tm.tm_year += 1900;

			sscanf(strTime,"%02d:%02d:%02d", 
				&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec);
		}
		break;

	case 3:
		sscanf(strTime, "%d/%02d/%02d-%02d:%02d:%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday,
			&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec);
		break;

	case 4:
		sscanf(strTime, "%d_%02d_%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday);
		break;

	case 5:
		sscanf(strTime, "%d_%02d_%02d-%02d_%02d_%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday,
			&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec);
		break;

	case 6:
		sscanf(strTime, "%d-%02d-%02d %02d:%02d:%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday,
			&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec);
		break;

	case 7:
		sscanf(strTime, "%d-%02d-%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday);
		break;

	default:
		return false;
		break;

	}

	the_tm.tm_mon -= 1;
	the_tm.tm_year -= 1900;

	time_t rtntime = mktime(&the_tm);

	sec = (int)rtntime;

	return true;
}

BOOL CPF::StrToTime(int type,const wchar_t* strTime,int& sec,int& ns)
{ 
	struct tm the_tm;

	memset(&the_tm,0x00,sizeof(the_tm));

	switch(type)
	{
	case 0:
		{
			time_t cur_time;
			time(&cur_time);

			struct tm * pTm = localtime(&cur_time);

			memcpy(&the_tm,pTm,sizeof(the_tm));
			the_tm.tm_mon += 1;
			the_tm.tm_year += 1900;

			swscanf(strTime,L"%02d:%02d:%02d.%09u",
				&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec, &ns);
		}
		break;

	case 1:
		swscanf(strTime,L"%d/%02d/%02d-%02d:%02d:%02d.%09u", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday,
			&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec,&ns);
		break;

	case 2:
		{
			time_t cur_time;
			time(&cur_time);

			struct tm * pTm = localtime(&cur_time);

			memcpy(&the_tm,pTm,sizeof(the_tm));
			the_tm.tm_mon += 1;
			the_tm.tm_year += 1900;

			swscanf(strTime,L"%02d:%02d:%02d", 
				&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec);
		}
		break;

	case 3:
		swscanf(strTime, L"%d/%02d/%02d-%02d:%02d:%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday,
			&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec);
		break;

	case 4:
		swscanf(strTime, L"%02d_%02d_%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday);
		break;

	case 5:
		swscanf(strTime, L"%d_%02d_%02d-%02d_%02d_%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday,
			&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec);
		break;

	case 6:
		swscanf(strTime, L"%d-%02d-%02d %02d:%02d:%02d", 
			&the_tm.tm_year, &the_tm.tm_mon, &the_tm.tm_mday,
			&the_tm.tm_hour,&the_tm.tm_min,&the_tm.tm_sec);
		break;

	default:
		return false;
		break;

	}

	the_tm.tm_mon -= 1;
	the_tm.tm_year -= 1900;

	time_t rtntime = mktime(&the_tm);

	sec = (int)rtntime;

	return true;
}

//BOOL CPF::StrToTime(const char* strTime,time_t& rtntime)
//{
//	strTime = strstr(strTime,",");//去掉星期
//
//	if( !strTime )
//		return false;
//
//	strTime += 2 ;		//去掉“，”
//
//	//day
//	int nday = -1;
//	std::string strday;
//	CPF::GetWord<char,std::string>(0,strTime,strday,' ');
//	nday = ACE_OS::atoi(strday.c_str());
//
//	//month
//	std::string strMonth;
//	int nMonth =  -1;
//	CPF::GetWord<char,std::string>(1,strTime,strMonth,' ');
//
//	if (ACE_OS::strcasecmp(strMonth.c_str(),"Jan") == 0)
//	{
//		nMonth = 0;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Feb") == 0)
//	{
//		nMonth = 1;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Mar") == 0)
//	{
//		nMonth = 2;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Apr") == 0)
//	{
//		nMonth = 3;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"May") == 0)
//	{
//		nMonth = 4;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Jun") == 0)
//	{
//		nMonth = 5;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Jul") == 0)
//	{
//		nMonth = 6;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Aug") == 0)
//	{
//		nMonth = 7;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Sep") == 0)
//	{
//		nMonth = 8;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Oct") == 0)
//	{
//		nMonth = 9;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Nov") == 0)
//	{
//		nMonth = 10;
//	}
//	else if  (ACE_OS::strcasecmp(strMonth.c_str(),"Dec") == 0)
//	{
//		nMonth = 11;
//	}
//	else
//	{
//		return false;
//	}
//
//	//year
//	int nyear = -1;
//	std::string stryear;
//	CPF::GetWord<char,std::string>(2,strTime,stryear,' ');
//	nyear = ACE_OS::atoi(stryear.c_str());
//
//	//hour-min-sec
//	std::string strhms;
//	std::string strhour;
//	std::string strmin;
//	std::string strsec;
//	int nhour = -1;
//	int nMin = -1;
//	int nSec = -1;
//	CPF::GetWord<char,std::string>(3,strTime,strhms,' ');
//	CPF::GetWord<char,std::string>(0,strhms.c_str(),strhour,':');
//	CPF::GetWord<char,std::string>(1,strhms.c_str(),strmin,':');
//	CPF::GetWord<char,std::string>(2,strhms.c_str(),strsec,':');
//
//	nhour = ACE_OS::atoi(strhour.c_str());
//	nMin = ACE_OS::atoi(strmin.c_str());
//	nSec = ACE_OS::atoi(strsec.c_str());
//
//	tm retm;
//
//	retm.tm_sec = nSec;
//	retm.tm_min = nMin;
//	retm.tm_hour = nhour;
//	retm.tm_mday = nday;
//	retm.tm_mon = nMonth;
//	retm.tm_year = nyear-1900;
//
//	rtntime = mktime(&retm);
//
//	return true;
//}

BOOL CPF::StrToTime(const char* strTime,time_t& rtntime)
{
	strTime = strstr(strTime,",");//去掉星期

	if( !strTime )
		return false;

	strTime += 2 ;		//去掉“，”

	char * strnewtime = ACE_OS::strdup(strTime);

	do{

		//day
		const char * strday = CPF::separate_token(strnewtime,' ',true);

		if( !strday )
			break;

		int nday = ACE_OS::atoi(strday);

		//month
		int nMonth =  -1;
		const char * strMonth = CPF::separate_token(strnewtime,' ',true);

		if( !strMonth )
			break;

		if (ACE_OS::strcasecmp(strMonth,"Jan") == 0)
		{
			nMonth = 0;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Feb") == 0)
		{
			nMonth = 1;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Mar") == 0)
		{
			nMonth = 2;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Apr") == 0)
		{
			nMonth = 3;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"May") == 0)
		{
			nMonth = 4;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Jun") == 0)
		{
			nMonth = 5;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Jul") == 0)
		{
			nMonth = 6;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Aug") == 0)
		{
			nMonth = 7;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Sep") == 0)
		{
			nMonth = 8;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Oct") == 0)
		{
			nMonth = 9;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Nov") == 0)
		{
			nMonth = 10;
		}
		else if  (ACE_OS::strcasecmp(strMonth,"Dec") == 0)
		{
			nMonth = 11;
		}
		else
		{
			break;
		}

		//year
		const char * stryear = CPF::separate_token(strnewtime,' ',true);

		if( !stryear )
			break;

		int nyear = ACE_OS::atoi(stryear);

		//hour-min-sec
		char * strhms = CPF::separate_token(strnewtime,' ',true);

		if( !strhms )
			break;

		const char * strhour = CPF::separate_token(strhms,':',true);

		if( !strhour )
			break;

		const char * strmin = CPF::separate_token(strhms,':',true);

		if( !strmin )
			break;

		const char * strsec = CPF::separate_token(strhms,':',true);

		if( !strsec )
			break;

		int nhour = ACE_OS::atoi(strhour);
		int nMin = ACE_OS::atoi(strmin);
		int nSec = ACE_OS::atoi(strsec);

		tm retm;

		retm.tm_sec = nSec;
		retm.tm_min = nMin;
		retm.tm_hour = nhour;
		retm.tm_mday = nday;
		retm.tm_mon = nMonth;
		retm.tm_year = nyear-1900;

		rtntime = mktime(&retm);

		free((void *)strnewtime);

		return true;
		
	}while(0);

	free((void *)strnewtime);

	return false;
}

BOOL CPF::StrToTime(const wchar_t* strTime,time_t& rtntime)
{
	return false;
}

std::string CPF::GetTimeLastInfo(ACE_UINT32 dwsec,int type)
{
	DWORD dwtemp = dwsec;

	int nHour = dwtemp/3600;
	dwtemp %= 3600;

	int nDay = nHour/24;
	nHour %= 24;

	int nMin = dwtemp/60;
	dwtemp %= 60;

	int nSec = dwtemp;

	char buf[256];

	if( 1 == type )
	{
		sprintf(buf,"%dDay-%02d:%02d:%02d",nDay,nHour,nMin,nSec);
	}
	else if( 2 == type )
	{
		sprintf(buf,"%d-%02d:%02d:%02d",nDay,nHour,nMin,nSec);
	}
	else if( 3 == type )
	{
		sprintf(buf,"%d:%02d:%02d:%02d",nDay,nHour,nMin,nSec);
	}
	else
	{
		sprintf(buf,"%dDay-%02d:%02d:%02d",nDay,nHour,nMin,nSec);
	}

	return std::string(buf);
}


std::string CPF::GetFriendNumString(ACE_UINT64 num)
{
	char buf[64]={0};

	int offset = 63;

	do
	{
		unsigned int temp = (unsigned int)(num%1000);

		num /= 1000;

		char chbk = buf[offset];

		if( num > 0 )
		{
			sprintf(buf+offset-4,",%03u",temp);
			buf[offset] = chbk;

			offset -= 4;
		}
		else
		{
			if( temp > 99 )
			{
				sprintf(buf+offset-3,"%u",temp);
				buf[offset] = chbk;

				offset -= 3;
			}
			else if( temp > 9 )
			{
				sprintf(buf+offset-2,"%u",temp);
				buf[offset] = chbk;

				offset -= 2;

			}
			else
			{
				sprintf(buf+offset-1,"%u",temp);
				buf[offset] = chbk;

				offset -= 1;
				sprintf(buf,"%u",temp);

			}
		}

	}while( num > 0 );

	return std::string(buf+offset);
}


std::string CPF::GetFriendNumString(ACE_INT64 num)
{
	if( num >= 0 )
	{
		return GetFriendNumString((ACE_UINT64)num);
	}
	else
	{
		num = -1 * num;

		std::string strnum = GetFriendNumString((ACE_UINT64)num);

		strnum = "-"+strnum;

		return strnum;
	}

	return "";
}

BOOL CPF::ParseUintRange(const char* source, size_t numtchars,
				  unsigned int& begin_int,unsigned int& end_int,char sep)
{
	std::vector<std::string>	vt_string;

	int nums = CPF::GetWords(source,numtchars,vt_string,sep,true);

	if( nums != 1 && nums != 2 )
	{
		return false;
	}

	{
		//去掉前后的空格
		const char * pbegin = vt_string[0].c_str();

		while( *pbegin == ' ' )
		{
			++pbegin;
		}

		char * pend = (char *)vt_string[0].c_str() + vt_string[0].size()-1;

		while( *pend == ' ' )
		{
			*pend = 0;
			--pend;
		}

		if( !CPF::cpf_isdigitstring(pbegin,10) )
			return false;

		begin_int = ACE_OS::atoi(pbegin);
	}

	if( nums == 1 )
	{
		end_int = begin_int;
	}
	else
	{
		//去掉前后的空格
		const char * pbegin = vt_string[1].c_str();

		while( *pbegin == ' ' )
		{
			++pbegin;
		}

		char * pend = (char *)vt_string[1].c_str() + vt_string[1].size()-1;

		while( *pend == ' ' )
		{
			*pend = 0;
			--pend;
		}

		if( !CPF::cpf_isdigitstring(pbegin,10) )
			return false;

		end_int = ACE_OS::atoi(pbegin);
	}

	return true;
}

bool CPF::TestProcMutex(const char *event_name)
{
	if(event_name && strlen(event_name) > 0 )
	{
		my_event_t hEvent;

		ACE_OS::last_error(0);

		if( MY_EVENT::event_init(&hEvent,0,0,0,event_name) != 0 )
		{
			return true;
		}

#ifdef OS_WINDOWS
		if (ACE_OS::last_error() == ERROR_ALREADY_EXISTS)
#else
		if (ACE_OS::last_error() == -1)
#endif
		{
			MY_EVENT::event_destroy(&hEvent);
			return false;		
		}

		//注意这里不能调用：ACE_OS::event_destroy(&hEvent);
		//如果调用了，就不能启动互斥的作用了。

		return true;
	}
	else
	{
		ACE_ASSERT(false);
		return true;
	}

}

CPF_CLASS 
//在服务中创建事件,对于linux（实际上是直接调用TestProcMutex）
bool CPF::TestProcMutex_For_Server(const char *event_name)
{
	if(event_name && strlen(event_name) > 0 )
	{
		my_event_t hEvent;

		ACE_OS::last_error(0);

		if( MY_EVENT::event_init_for_server(&hEvent,0,0,0,event_name) != 0 )
		{
			return true;
		}

#ifdef OS_WINDOWS
		if (ACE_OS::last_error() == ERROR_ALREADY_EXISTS)
#else
		if (ACE_OS::last_error() == -1)
#endif
		{
			MY_EVENT::event_destroy(&hEvent);
			return false;		
		}

		//注意这里不能调用：ACE_OS::event_destroy(&hEvent);
		//如果调用了，就不能启动互斥的作用了。

		return true;
	}
	else
	{
		ACE_ASSERT(false);
		return true;
	}
}


int CPF::Http_DetachLine::DetachLine(IN const char * alldata,IN int inlen,
									 OUT const char *& data,OUT int& datalen,
									 OUT std::vector<ONE_LINE_DATA>& vt_line)
{
	if(inlen==0)
	{
		return 0;
	}

	vt_line.reserve(16);

	const char *curhead = alldata;

	while( 1 )
	{
		int count = 0;
		int len = 0;

		const char *next;

		/* Establish string and get the first token: */
		const char * token = strtokCRLF( curhead, len ,count, next);

		if(count>=3)
		{
			datalen = (int)((alldata+inlen)-next);
			data = (char *)next;
			return 1;
		}

		if( token == NULL )
		{
			break;
		}

		curhead = next;

		vt_line.push_back(std::make_pair(token,len));

	} 

	return 0;
}

const char * CPF::Http_DetachLine::strtokCRLF ( IN const char * input_string, 
		OUT int &len ,OUT int &count,
		OUT const char *& nextoken )
{
	static const unsigned char map[256] = {
		/*00*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x00,0x0D,0x00,0x00,
		/*10*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*20*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*30*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*40*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*50*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*60*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*70*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*80*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*90*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*A0*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*B0*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*C0*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*D0*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*E0*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		/*F0*/		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};



	len = 0;
	count = 0;

	const unsigned char * curstr = (unsigned char *)input_string;

	/* Find beginning of token (skip over leading delimiters). Note that
	* there is no token iff this loop sets str to point to the terminal
	* null (*str == '\0') */
	while ( map[*curstr] )
	{
		curstr++;
		count ++;
	}

	if(count>=4)
	{
		nextoken = (const char *)curstr;
		return NULL;
	}

	const char * output_string = (const char *)curstr;


	/* Find the end of the token. If it is not the end of the string,
	* put a null there. */
	for ( ; *curstr ; curstr++ )
	{
		if ( map[*curstr] )
		{
			break;
		}

		len ++;
	}

	/* Update nextoken (or the corresponding field in the per-thread data
	* structure */
	nextoken = (const char *)curstr;

	/* Determine if a token has been found. */
	if ( output_string == (const char *)curstr )
		return NULL;
	else
		return output_string;
}


#ifdef OS_WINDOWS
#include "Iphlpapi.h"
#pragma comment( lib, "Iphlpapi.lib")

BOOL GetLocalMachineIP_windows(std::vector<ACE_UINT32> &vt_selfIP)
{
	DWORD dwSize = 0;

	DWORD dwRtn = GetAdaptersInfo(NULL,&dwSize);

	PIP_ADAPTER_INFO pIP_ADAPTER_INFO = NULL;

	if( dwRtn == ERROR_BUFFER_OVERFLOW )
	{
		pIP_ADAPTER_INFO = (PIP_ADAPTER_INFO)new BYTE[dwSize];

		if( !pIP_ADAPTER_INFO )
			return FALSE;

		dwRtn = GetAdaptersInfo(pIP_ADAPTER_INFO,&dwSize);
	}

	if( dwRtn != NO_ERROR )
	{
		delete []((BYTE *)pIP_ADAPTER_INFO);
		return FALSE;
	}

	if( !pIP_ADAPTER_INFO )
	{
		return false;
	}

	PIP_ADAPTER_INFO pInfo = pIP_ADAPTER_INFO;

	while( pInfo )
	{
		if( pInfo->AddressLength != 6 )
		{
			pInfo = pInfo->Next;
			continue;
		}

		IP_ADDR_STRING * pIpAddress = &pInfo->IpAddressList;

		while( pIpAddress )
		{
			DWORD dwIP = inet_addr((char *)pIpAddress->IpAddress.String);
			DWORD dwMask = inet_addr((char *)pIpAddress->IpMask.String);

			if( dwIP && dwMask )
			{
				vt_selfIP.push_back(dwIP);
			}

			pIpAddress = pIpAddress->Next;
		}

		pIpAddress = &pInfo->GatewayList;

		while( pIpAddress )
		{
			DWORD dwGateWayIP = inet_addr((char *)pIpAddress->IpAddress.String );

			if(dwGateWayIP )
			{			

			}

			pIpAddress = pIpAddress->Next;
		}	

		pInfo = pInfo->Next;	
	}

	delete []((BYTE *)pIP_ADAPTER_INFO);


	return true;	
}
#else
BOOL GetLocalMachineIP_linux(std::vector<ACE_UINT32> &vt_selfIP)
{
	return false;
}
#endif//OS_WINDOWS


BOOL CPF::GetLocalMachineIP(std::vector<ACE_UINT32>& vt_netorderip)
{
#ifdef OS_WINDOWS
	return GetLocalMachineIP_windows(vt_netorderip);
#else
	return GetLocalMachineIP_linux(vt_netorderip);
#endif
}


const BYTE * CPF::memstr(const BYTE * ptext,size_t textlen,const BYTE * pkey,size_t keylen)
{
	const BYTE * ptemp = ptext;
	size_t curlen = textlen;

	int firstc = (int)pkey[0];

	while(curlen >= keylen )
	{
		BYTE * p = (BYTE *)memchr(ptemp,firstc,curlen-keylen+1);

		if( p )
		{
			if( memcmp(p+1,pkey+1,keylen-1) == 0 )
			{
				return p;
			}

			ptemp = p+1;

			curlen = textlen-(ptemp-ptext);
		}
		else
		{
			break;
		}			
	}

	return NULL;
}

int CPF::mask_memcmp(const char * pdat1,const char * pdata2,const char * mask,int len)
{
	for(int i = 0; i < len; ++i)
	{
		if( (*pdat1 & *mask) != (*pdata2 & *mask) )
		{
			return 1;
		}

		++pdat1;
		++pdata2;
		++mask;		
	}

	return 0;
}

int CPF::mask_patcmp(const char * pat,const char * pdata,const char * mask,int len)
{
	for(int i = 0; i < len; ++i)
	{
		if( (*pat) != (*pdata & *mask) )
		{
			return 1;
		}

		++pat;
		++pdata;
		++mask;		
	}

	return 0;
}


std::string CPF::MakeUpIntSetString(const std::vector<int>& vt_data,char sep)
{
	std::string out_string;

	for(size_t i = 0; i < vt_data.size(); ++i)
	{
		char id_string[32];

		if( i == 0 )
		{
			sprintf(id_string,"%d",vt_data[i]);

			out_string += id_string;
		}
		else
		{
			sprintf(id_string,"%c%d",sep,vt_data[i]);
			out_string += id_string;
		}		
	}

	return out_string;
}


std::string CPF::MakeUpIntSetString(const std::vector<unsigned int>& vt_data,char sep)
{
	std::string out_string;

	for(size_t i = 0; i < vt_data.size(); ++i)
	{
		char id_string[32];

		if( i == 0 )
		{
			sprintf(id_string,"%u",vt_data[i]);

			out_string += id_string;
		}
		else
		{
			sprintf(id_string,"%c%u",sep,vt_data[i]);
			out_string += id_string;
		}		
	}

	return out_string;
}

CPF_CLASS
std::string CPF::MakeUpIntSetString(const std::vector<ACE_UINT64>& vt_data,char sep)
{
	std::string out_string;

	for(size_t i = 0; i < vt_data.size(); ++i)
	{
		char id_string[32];

		if( i == 0 )
		{
			sprintf(id_string,"%lld",vt_data[i]);

			out_string += id_string;
		}
		else
		{
			sprintf(id_string,"%c%lld",sep,vt_data[i]);
			out_string += id_string;
		}		
	}

	return out_string;
}


std::string CPF::MakeUpStringSetString(const std::vector<std::string>& vt_data,char sep)
{
	std::string out_string;

	for(size_t i = 0; i < vt_data.size(); ++i)
	{
		if( i == 0 )
		{
			out_string += vt_data[i];
		}
		else
		{
			out_string += sep;
			out_string += vt_data[i];
		}		
	}

	return out_string;
}

unsigned int CPF::calc_hashnr(const BYTE *key,unsigned int length)
{  
	register unsigned int nr=1, nr2=4;  

	while (length--)  
	{  
		nr^= (((nr & 63)+nr2)*((unsigned int) (BYTE) *key++))+ (nr << 8);  
		nr2+=3;  
	}  
	return((unsigned int) nr);  
}  


void CPF::LTrim(std::string& data,const char* charset)
{
	const char * pdata = data.c_str();

	int good = 0;
	if (charset) {
		while (strchr (charset, *(pdata + good)) != NULL)
			good++;
	}
	else {
		while (*(pdata + good) == ' ')
			good++;
	}

	if (good > 0)
	{
		data.erase(0,good);
	}

	return;
}

void CPF::RTrim(std::string& data,const char* charset)
{
	const char * pdata = data.c_str();

	int good = (int)data.length();
	int old_size = good;

	if (good == 0)
		return;

	if (charset) {
		while (good > 0  &&  strchr (charset, *(pdata+good-1)) != NULL)
			--good;
	}
	else {
		while (good > 0  &&  *(pdata+good-1) == ' ')
			--good;
	}

	data.erase(good,old_size-good);

	return;
}

void CPF::Trim(std::string& data,const char* charset)
{
	LTrim(data,charset);
	RTrim(data,charset);

	return;
}

const char * CPF::MD5Passwd(const char * passwd,int len)
{
	unsigned char new_passwd_bin[16] = {0};

	CPF::CMD5::MD5Direct((unsigned char *)passwd,
		len,new_passwd_bin);

	static char new_passwd_text[64] = {0};

	CPF::bin2hexstr_littleendian(new_passwd_bin,sizeof(new_passwd_bin),
		(char*)new_passwd_text);

	CPF::str_lwr(new_passwd_text);

	return new_passwd_text;

}

int CPF::RunProcessAndWaitDone(const char * szAppName, const char *szCmd, const char *szWorkingDirectory)
{
	int process_id = RunProcess(szAppName,szCmd,szWorkingDirectory);

	if( process_id == -1 )
		return -1;

	while( 1 )
	{
		if( ACE::process_active(process_id) )
		{//等待程序结束

			ACE_OS::sleep(1);

			continue;
		}
		else
		{
			break;
		}
	}	

	return 0;
}



int CPF::RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory)
{
	if( !szAppName && !szCmd )
		return -1;

	ACE_Process rProcess;
	ACE_Process_Options rOpt;

	rOpt.handle_inheritance(false);

	if( szWorkingDirectory && szWorkingDirectory[0] )
	{
		rOpt.working_directory(szWorkingDirectory);
	}

#ifdef OS_WINDOWS
	rOpt.creation_flags( CREATE_NEW_CONSOLE );
#endif

	if( szAppName && szAppName[0] && szCmd && szCmd[0] )
		rOpt.command_line("%s %s",szAppName,szCmd);
	else if( szAppName && szAppName[0] )
		rOpt.command_line("%s",szAppName);
	else if( szCmd && szCmd[0] )
		rOpt.command_line("%s",szCmd);

	return rProcess.spawn( rOpt );
}

int CPF::RunBat(const char * szAppName, const char *szCmd, const char *szWorkingDirectory)
{
	if( (!szAppName || !szAppName[0]) 
		&& (!szCmd || !szCmd[0]) )
	{
		return -1;
	}

#ifdef OS_WINDOWS
	if( CPF::IsFullPathName(szAppName) )
	{
		const char * path_name = CPF::GetPathNameFromFullName(szAppName);

		ShellExecute(NULL,NULL,szAppName,NULL,path_name,SW_SHOWNORMAL);
	}
	else
	{
		ShellExecute(NULL,NULL,szAppName,NULL,NULL,SW_SHOWNORMAL);
	}
#else
	char pszTmp[1024];
	char strOldPath[128];

	// 记录当前工作路径
	ACE_OS::getcwd((char*)strOldPath, 128);

	// 加运行权限
	sprintf(pszTmp, "chmod +x %s", szAppName);
	system(pszTmp);

	// 路径切换
	if( CPF::IsFullPathName(szAppName) )
	{
		const char * path_name = CPF::GetPathNameFromFullName(szAppName);
		sprintf(pszTmp, "cd %s", (NULL==szWorkingDirectory) ? path_name : szWorkingDirectory);
		system(pszTmp);
		sprintf(pszTmp, "%s %s", szAppName, szCmd);
	}
	else
		sprintf(pszTmp, "./%s %s", szAppName, szCmd);

	// 执行
	system(pszTmp);

	ACE_OS::chdir(strOldPath);	// 返回开始的目录

#endif

	return 0;
}

BOOL JudgeToDeleteThisFile(const char * path,const char * file_name,const char * str_time,int timeout_sec)
{
	int sec = 0;
	int ns = 0;

	CPF::StrToTime(5,str_time,sec,ns);

	if( ACE_OS::gettimeofday().sec()-sec > timeout_sec )
	{
		ACE_OS::unlink(CPF::JoinPathToPath(path,file_name));

		return true;
	}
	else
	{
		return false;
	}

}


int CPF::DeleteTimerLogFile(const char * path,const char * prev,const char * suf,int timeout_sec)
{
	int count = 0;

	//扫描源目录的所有文件
	ACE_DIRENT **namelist = NULL ;

	int nEntityNum = ACE_OS::scandir(
		path,
		&namelist,
		NULL,
		NULL);

	//如果目录为空，返回
	if(nEntityNum<=0) 
	{
		return count;
	}

	for(int nFileIndex=0;nFileIndex<nEntityNum;nFileIndex++)
	{
		if( strcmp(namelist[nFileIndex]->d_name,".") == 0 )
			continue;
		if( strcmp(namelist[nFileIndex]->d_name ,"..") == 0 )
			continue;

		if( namelist[nFileIndex]->d_name != strstr(namelist[nFileIndex]->d_name,prev) )
			continue;

		const char * time_begin_pos = namelist[nFileIndex]->d_name+strlen(prev);

		std::string str_time;

		if( suf )
		{
			const char * pdata = strrchr(namelist[nFileIndex]->d_name,'.');

			if( !pdata )
				continue;

			if( stricmp(pdata+1,suf) != 0 )
				continue;

			str_time.assign(time_begin_pos,pdata-time_begin_pos);
		}
		else
		{
			str_time = time_begin_pos;
		}

		if( JudgeToDeleteThisFile(path,namelist[nFileIndex]->d_name,str_time.c_str(),timeout_sec) )
		{
			++count;
		}
	}		


	//释放空间
	CPF::freedirent(namelist,nEntityNum);

	return count;
}


#ifdef OS_WINDOWS

#include  <windows.h> 
#include  <winuser.h>
#include  <tchar.h> 
#include  <stdio.h> 
#include  <strsafe.h> 

#pragma comment(lib, "User32.lib") 
#define BUFSIZE 256 

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO); 
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD); 

#define VER_SUITE_WH_SERVER                 0x00008000
#define SM_SERVERR2							89

CPF::Win32SysType CPF::GetWindowOs( char * pszOS,int& out_os_bit) 
{ 
	CPF::Win32SysType os_type = CPF::Nothing;

	OSVERSIONINFOEX osvi; 
	SYSTEM_INFO si; 
	
	PGNSI pGNSI; 
	//PGPI pGPI; 
	
	BOOL bOsVersionInfoEx; 
	
	//DWORD dwType; 
	
	ZeroMemory(&si, sizeof(SYSTEM_INFO)); 
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX)); 
	
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	
	out_os_bit = sizeof(void *)*8;

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) ) 
	{  
		return os_type; 
	}
	
	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise. 
	
	pGNSI = (PGNSI) GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")),  "GetNativeSystemInfo"); 
	
	if(NULL != pGNSI) 
		pGNSI(&si); 
	else 
		GetSystemInfo(&si);

	if ( !(VER_PLATFORM_WIN32_NT==osvi.dwPlatformId &&  osvi.dwMajorVersion > 4) ) 
	{
		StringCchCopy(pszOS, BUFSIZE, TEXT("This sample does not support this version of Windows.\n ")); 

		return os_type;
	}
	
	StringCchCopy(pszOS, BUFSIZE, TEXT("Microsoft ")); 
	
	// Test for the specific product. 
	if ( osvi.dwMajorVersion == 6 ) 
	{
		if( osvi.dwMinorVersion == 0 ) 
		{ 
			if( osvi.wProductType == VER_NT_WORKSTATION ) 
			{
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows Vista ")); 
				os_type = CPF::WindowsVista;
			}
			else 
			{
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 " )); 
				os_type = CPF::Windows2008;
			}
		} 
		if ( osvi.dwMinorVersion == 1 ) 
		{ 
			if( osvi.wProductType == VER_NT_WORKSTATION ) 
			{
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows 7 ")); 
				os_type = Windows7;
			}
			else 
			{
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 R2 " )); 
				os_type = Windows2008R2;
			}
		} 
		
	} //end if ( osvi.dwMajorVersion == 6 ) 

	if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 ) 
	{ 
		os_type = CPF::Windows2003;

		if( GetSystemMetrics(SM_SERVERR2) ) 
			StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Server 2003 R2, ")); 
		else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER ) 
			StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Storage Server 2003")); 
		else if ( osvi.wSuiteMask & VER_SUITE_WH_SERVER ) 
			StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Home Server")); 
		else if( osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64) 
		{ 
			os_type = CPF::WindowsXP;
			//os_bit = 64;
			StringCchCat(pszOS, BUFSIZE, TEXT( "Windows XP Professional x64 Edition")); 
		} 
		else 
		{
			StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2003, ")); 
		}
			
		// Test for the server type. 
		if ( osvi.wProductType != VER_NT_WORKSTATION ) 
		{ 
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 ) 
			{ 
				//os_bit = 64;
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER ) 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition for Itanium-based Systems" )); 
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE ) 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition for Itanium-based Systems" )); 
			} 
			else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 ) 
			{ 
				//os_bit = 64;
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER ) 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter x64 Edition" )); 
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE ) 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise x64 Edition" )); 
				else 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Standard x64 Edition" )); 
			} 
			else 
			{ 
				//os_bit = 32;
				if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER ) 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Compute Cluster Edition" )); 
				else if( osvi.wSuiteMask & VER_SUITE_DATACENTER ) 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition" )); 
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE ) 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition" )); 
				else if ( osvi.wSuiteMask & VER_SUITE_BLADE ) 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Web Edition" )); 
				else 
					StringCchCat(pszOS, BUFSIZE, TEXT( "Standard Edition" )); 
			} 
		} 
	} 
	if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 ) 
	{ 
		os_type = CPF::WindowsXP;

		StringCchCat(pszOS, BUFSIZE, TEXT("Windows XP ")); 
		
		if( osvi.wSuiteMask & VER_SUITE_PERSONAL ) 
			StringCchCat(pszOS, BUFSIZE, TEXT( "Home Edition" )); 
		else 
			StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" )); 
	} 		
	if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 ) 
	{ 
		os_type = CPF::Windows2000;

		StringCchCat(pszOS, BUFSIZE, TEXT("Windows 2000 ")); 
		
		if ( osvi.wProductType == VER_NT_WORKSTATION ) 
		{ 
			StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" )); 
		} 
		else  
		{ 
			if( osvi.wSuiteMask & VER_SUITE_DATACENTER ) 
				StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Server" )); 
			else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					StringCchCat(pszOS, BUFSIZE, TEXT( "Advanced Server" )); 
			else StringCchCat(pszOS, BUFSIZE, TEXT( "Server" )); 
		} 
	} 
	// Include service pack (if any) and build number. 
	
	if( _tcslen(osvi.szCSDVersion) > 0 ) 
	{ 
		StringCchCat(pszOS, BUFSIZE, TEXT(" ") ); 
		
		StringCchCat(pszOS, BUFSIZE, osvi.szCSDVersion); 
	} 
	
	TCHAR buf[80]; 
	
	StringCchPrintf( buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber); 
	
	StringCchCat(pszOS, BUFSIZE, buf); 
	
	if ( osvi.dwMajorVersion >= 6 ) 
	{ 
		if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 ) 
		{
			//os_bit = 64;
			StringCchCat(pszOS, BUFSIZE, TEXT( ", 64-bit" )); 
		}
		else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL ) 
		{
			//os_bit = 32;
			StringCchCat(pszOS, BUFSIZE, TEXT(", 32-bit")); 
		}
	} 

	return os_type;  
	
} 

#endif//os_windows


static FILE* MyTiXmlFOpen( const char* filename, const char* mode )
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
	FILE* fp = 0;
	errno_t err = fopen_s( &fp, filename, mode );
	if ( !err && fp )
		return fp;
	return 0;
#else
	return fopen( filename, mode );
#endif
}

BOOL CPF::ReadXmlFileToBuf(const char * xml_filename, CCPFOctets& outOctets)
{
	// reading in binary mode so that tinyxml can normalize the EOL
	FILE* file = MyTiXmlFOpen( xml_filename, "rb" );	

	if( !file )
	{
		return false;
	}

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	long length = 0;
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );

	// Strange case, but good to handle up front.
	if ( length <= 0 )
	{
		return false;
	}

	char* buf = new char[ length+1 ];
	buf[0] = 0;

	if ( fread( buf, length, 1, file ) != 1 ) 
	{
		delete [] buf;
		return false;
	}

	buf[length] = 0;

	outOctets.Reset();
	outOctets.Append((const BYTE*)buf, length+1);
	//	ReadOneNewPatXml(buf,length);

	delete []buf;

	fclose(file);

	return true;
}


BOOL CPF::ReadCryXmlFileToBuf(const char * dat_filename, CCPFOctets& outOctets)
{
	// reading in binary mode so that tinyxml can normalize the EOL
	FILE* file = MyTiXmlFOpen( dat_filename, "rb" );	

	if( !file )
	{
		return false;
	}

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	long length = 0;
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );

	// Strange case, but good to handle up front.
	if ( length <= 0 )
	{
		return false;
	}

	char* buf = new char[ length+1 ];
	buf[0] = 0;

	if ( fread( buf, length, 1, file ) != 1 ) 
	{
		delete [] buf;
		return false;
	}

	{
		buf[0]-=19;
		buf[1]+=37;

		for(int i = 2; i < length; ++i)
		{
			if( i % 2 == 0 )
				buf[i] ^= buf[1];
			else
				buf[i] ^= buf[0];
		}
	}

	buf[length] = 0;


	outOctets.Reset();
	outOctets.Append((const BYTE*)(buf+2), length-1);
	//	ReadOneNewPatXml(buf+2,length-2);

	delete []buf;

	fclose(file);

	return true;
}

