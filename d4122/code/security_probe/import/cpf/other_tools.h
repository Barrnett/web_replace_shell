#ifndef _OTHER_TOOLS_2006_05_08
#define _OTHER_TOOLS_2006_05_08
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"
#include "cpf/Octets.h"

typedef enum{

	CMP_OP_INVALID=-1,
	CMP_OP_GT=0,//>
	CMP_OP_GE=1,//>=
	CMP_OP_EQ=2,//==
	CMP_OP_NEQ=3,//!=
	CMP_OP_LT=4,//<
	CMP_OP_LE=5,//<=
	CMP_OP_BT=6,//between
	CMP_OP_NBT=7,//not between

}CMP_OP_TYPE;

template<typename T>
BOOL do_cmp_operation(const T& nValue,CMP_OP_TYPE nOperator,const T& nPort1,const T& nPort2)
{
	switch(nOperator) 
	{
	case CMP_OP_GT:
		return nValue>nPort1;
		break;
	case CMP_OP_GE:
		return nValue>=nPort1;

		break;
	case CMP_OP_EQ:
		return nValue==nPort1;
		break;
	case CMP_OP_NEQ:
		return nValue!=nPort1;
		break;
	case CMP_OP_LT:
		return nValue<nPort1;

		break;
	case CMP_OP_LE:
		return nValue<=nPort1;
		break;
	case CMP_OP_BT:
		return nValue>=nPort1 && nValue<=nPort2;
		break;
	case CMP_OP_NBT:
		return nValue<nPort1 || nValue>nPort2;
		break;

	default:
		break;
	}

	return 0;
}


namespace CPF
{
	CPF_CLASS
		char* itoa(int num);

	CPF_CLASS
		ACE_TCHAR * linux_ultoa(ULONG num, ACE_TCHAR * str, int radix);

	CPF_CLASS
		char * str_upr(char * pData);
	CPF_CLASS
		char * str_upr(char * pData,size_t nchars);
	CPF_CLASS
		char * str_upr(std::string& string_data);


	CPF_CLASS
		wchar_t * str_upr(wchar_t * pData);
	CPF_CLASS
		wchar_t * str_upr(wchar_t * pData,size_t nwchars);
	CPF_CLASS
		wchar_t * str_upr(std::wstring& wstring_data);

	CPF_CLASS
		char * str_lwr(char * pData);
	CPF_CLASS
		char * str_lwr(char * pData,size_t nchars);
	CPF_CLASS
		char * str_lwr(std::string& string_data);

	CPF_CLASS
		wchar_t * str_lwr(wchar_t * pData);
	CPF_CLASS
		wchar_t * str_lwr(wchar_t * pData,size_t nwchars);
	CPF_CLASS
		wchar_t * str_lwr(std::wstring& wstring_data);

	//charset==NULL表示去掉空格
	CPF_CLASS
		void LTrim(std::string& data,const char* charset=NULL);

	//charset==NULL表示去掉空格
	CPF_CLASS
		void RTrim(std::string& data,const char* charset=NULL);

	//charset==NULL表示去掉空格
	CPF_CLASS
		void Trim(std::string& data,const char* charset=NULL);	

	CPF_CLASS
		BOOL GetLocalMachineIP(std::vector<ACE_UINT32>& vt_netorderip);

	//windows下从 a 变成 a.dll
	//linux下从a 变成liba.so
	//返回longname
	CPF_CLASS
		ACE_TCHAR * get_dll_name(ACE_TCHAR* longname,const ACE_TCHAR * shortname);

	CPF_CLASS 
		//将一个16进制的值，变成字符串。其中每一个字节变成两个字符
		//outstring需要的空间大小是inlen*2
		void string_value_to_a(const char * pMD5Value,int inlen,ACE_TCHAR * outstring,int upper );

	CPF_CLASS
		//和string_value_to_a相反
		//poutvalue需要的空间大小是stringlen/2
		void string_a_to_value(const ACE_TCHAR* instring,int stringlen,unsigned char * poutvalue);


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
	CPF_CLASS
	const ACE_TCHAR* FormatTime(int type,int sec,int ns=0);

	CPF_CLASS
	const ACE_TCHAR* FormatTime(int type,time_t the_time);

	CPF_CLASS 
	const ACE_TCHAR* get_curtime_string(int type);


	//Fri, 28 Jul 2006 13:10:09 +0800，变化成time_t
	CPF_CLASS
	BOOL StrToTime(const char* strTime,time_t& rtntime);

	CPF_CLASS
	BOOL StrToTime(const wchar_t* strTime,time_t& rtntime);

	//和FormatTime是相反的函数。从字符串得到数据
	CPF_CLASS
	BOOL StrToTime(int type,const char* strTime,int& sec,int& ns);

	CPF_CLASS
	BOOL StrToTime(int type,const wchar_t* strTime,int& sec,int& ns);

	CPF_CLASS
	const BYTE * memstr(const BYTE * ptext,size_t textlen,const BYTE * pkey,size_t keylen);

	CPF_CLASS
	//相等返回0
	//if( (*pdat1 & *mask) != (*pdata2 & *mask) )
	int mask_memcmp(const char * pdat1,const char * pdata2,const char * mask,int len);

	CPF_CLASS
		//相等返回0
		//if( *pat != (*pdata & *mask) )
	int mask_patcmp(const char * pat,const char * data,const char * mask,int len);

	CPF_CLASS
		//dwsec：单位为秒
		//输出格式：type=1:%dDay-%02d:%02d:%02d
		//2=%d-%02d:%02d:%02d
		//3=%d:%02d:%02d:%02d
		std::string GetTimeLastInfo(ACE_UINT32 dwsec,int type=1);


	CPF_CLASS
		std::string GetFriendNumString(ACE_UINT64 num);
	CPF_CLASS
		std::string GetFriendNumString(ACE_INT64 num);

	CPF_CLASS
	inline std::string GetFriendNumString(ACE_UINT32 num)
	{
		return GetFriendNumString((ACE_UINT64)num);
	}

	CPF_CLASS
	inline std::string GetFriendNumString(ACE_INT32 num)
	{
		return GetFriendNumString((ACE_INT64)num);
	}
	
	CPF_CLASS
	inline std::string GetFriendNumString(ACE_UINT16 num)
	{
		return GetFriendNumString((ACE_UINT64)num);
	}

	CPF_CLASS
	inline std::string GetFriendNumString(ACE_INT16 num)
	{
		return GetFriendNumString((ACE_INT64)num);
	}

	class CPF_CLASS Http_DetachLine
	{
	public:
		typedef std::pair<const char *,int>	ONE_LINE_DATA;

	public:
		//输入所有的数据,如果数据头部全部到齐(\r\n\r\n),返回1,否则返回0
		//如果全部到齐,返回的头部以外的数据指针和数据长度(data,datalen)
		//vt_line:返回的每行的数据指针和字符个数
		static int DetachLine(IN const char * indata,IN int inlen,
			OUT const char *& data,OUT int& datalen,
			OUT std::vector<ONE_LINE_DATA>& vt_line);

		inline static int DetachLine(IN OUT const char *& inoutdata,IN OUT int& inoutlen,
			OUT std::vector<ONE_LINE_DATA>& vt_line)
		{
			return DetachLine(inoutdata,inoutlen,inoutdata,inoutlen,vt_line);
		}


		//返回当前行的头部指针.len = 当前行的长度.
		//count碰到了几个\r或者\n
		//nextoken,表示下一次从哪个字符开始继续扫描
		static const char * strtokCRLF ( IN const char * input_string, 
			OUT int &len ,OUT int &count,
			OUT const char *& nextoken );

	};

	//使用一个事件来表示进程是否重复
	//看event_name的进程是否只有一份,如果返回false，表示有重复。返回TRUE表示没有重复
	
	CPF_CLASS 
		bool TestProcMutex(const char *event_name);

	CPF_CLASS 
		//在服务中创建事件,对于linux（实际上是直接调用TestProcMutex）
		bool TestProcMutex_For_Server(const char *event_name);

	 CPF_CLASS
		 //解析"1-12"这样的字符串，得到两个无符号的整数
		 //如果正确就返回true。否则返回false

		 BOOL ParseUintRange(const char* source, size_t numtchars,
		 unsigned int& begin_int,unsigned int& end_int,char sep=('-'));

	 CPF_CLASS
	 unsigned int calc_hashnr(const BYTE *key,unsigned int length);

	 //将一个字符串，进行md5后，然后将md5的值转换成字符方式
	 //注意返回值是一个静态变量的字符串数组
	 CPF_CLASS
	 const char * MD5Passwd(const char * passwd,int len);
	
	 CPF_CLASS
	 inline const char * MD5Passwd(const char * passwd)
	 {
		return MD5Passwd(passwd,(int)strlen(passwd));
	 }


	 //将数字的数组，转换成字符串
	 //比如数组中有'3','5','18',得到是"3,5,18"
	 CPF_CLASS
		 std::string MakeUpIntSetString(const std::vector<int>& vt_data,char sep=',');

	 CPF_CLASS
		 std::string MakeUpIntSetString(const std::vector<unsigned int>& vt_data,char sep=',');

	 CPF_CLASS
		 std::string MakeUpIntSetString(const std::vector<ACE_UINT64>& vt_data,char sep=',');

	 CPF_CLASS
		 std::string MakeUpStringSetString(const std::vector<std::string>& vt_data,char sep=',');

	 CPF_CLASS//just for windows
	 int RunBat(const char * szAppName, const char *szCmd, const char *szWorkingDirectory);

	 CPF_CLASS
	 int RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory);

	 CPF_CLASS
	 int RunProcessAndWaitDone(const char * szAppName, const char *szCmd, const char *szWorkingDirectory);

	 //删除类似的日志文件flux_control_2009_10_04-08_00_00.log
	 //其中prev="flux_control_",suf="log"
	 CPF_CLASS
	 int DeleteTimerLogFile(const char * path,const char * prev,const char * suf,int timeout_sec);

	 typedef enum tagWin32SysType{
		 Nothing = 0,
		 Windows32s=1,
		 WindowsNT3,
		 Windows95,
		 Windows98,
		 WindowsME,
		 WindowsNT4,
		 Windows2000,
		 WindowsXP,
		 Windows2003,
		 Windows2008,
		 Windows2008R2,
		 WindowsVista,
		 Windows7,
		 
	 }Win32SysType;

	 CPF_CLASS
	 Win32SysType GetWindowOs(char * desc,int& os_bit);

	 CPF_CLASS
	 BOOL ReadCryXmlFileToBuf(const char * dat_filename, CCPFOctets& outOctets);

	 CPF_CLASS
	 BOOL ReadXmlFileToBuf(const char * xml_filename, CCPFOctets& outOctets);

}


#if defined(OS_WINDOWS)

#define ULTOA( num, str, radix )	ultoa(num,str,radix)

#elif defined(OS_LINUX)

//#define ITOA( num, str, radix )		linux_itoa(num,str,radix)
#define ULTOA( num, str, radix )		CPF::linux_ultoa(num,str,radix)

#else

#error Unknow operating system.

#endif // #if defined(OS_WINDOWS)


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//#define _OTHER_TOOLS_2006_05_08


