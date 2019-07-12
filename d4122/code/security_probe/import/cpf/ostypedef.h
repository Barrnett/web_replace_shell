#ifndef OS_TYPEDEF_H_ZHUZP_2006_04_08
#define OS_TYPEDEF_H_ZHUZP_2006_04_08
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


#ifdef WIN32
#if !defined(_WIN32_WINNT)
	#define _WIN32_WINNT 0x400
#endif
#endif

/* Checking the operating system */

#ifndef OS_LINUX
#ifdef __linux
#define OS_LINUX __linux
#endif
#endif

#ifndef OS_WINDOWS
#if defined(_WIN32) || defined(WIN32)
#define OS_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#if !defined(OS_LINUX) && !defined(OS_WINDOWS)
#error Unknown operating system.
#endif


#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include "ace/Basic_Types.h"
#include "ace/SString.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/ace_wchar.h"

#if defined(OS_WINDOWS)
#define MPLAT_Export_Flag __declspec (dllexport)
#define MPLAT_Import_Flag __declspec (dllimport)
#else
#define MPLAT_Export_Flag
#define MPLAT_Import_Flag
#endif

#ifdef __cplusplus
#define MPLAT_API_Export_Flag	extern "C" MPLAT_Export_Flag
#define MPLAT_API_Import_Flag	extern "C" MPLAT_Import_Flag
#else
#define MPLAT_API_Export_Flag	MPLAT_Export_Flag
#define MPLAT_API_Import_Flag	MPLAT_Import_Flag
#endif

#define MPLAT_CLASS_Export_Flag	MPLAT_Export_Flag
#define MPLAT_CLASS_Import_Flag	MPLAT_Import_Flag

#define MPLAT_Export	MPLAT_Export_Flag
#define MPLAT_Import	MPLAT_Import_Flag

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

typedef ACE_Byte			BYTE;
typedef unsigned int		UINT;
typedef unsigned long		ULONG;
typedef int                 BOOL;


typedef const char *		LPACECSTR;
typedef char *				LPACESTR;
typedef BYTE *				LPBYTE;

typedef const ACE_TCHAR *	LPACECTSTR;
typedef ACE_TCHAR *			LPACETSTR;

#ifndef OS_WINDOWS
#include "stdint.h"
typedef unsigned short  		USHORT, *PUSHORT;
typedef unsigned short			WORD;
typedef unsigned int			DWORD;
typedef long					LONG;
typedef unsigned long			ULONG;
typedef void *					PVOID;
typedef void *					LPVOID;
typedef char *					LPSTR;
typedef const char *			LPCSTR;
typedef const ACE_TCHAR *		LPCTSTR;
typedef ACE_UINT64				ULONGLONG;
typedef ACE_INT64				LONGLONG;
typedef uintptr_t			UINT_PTR;
typedef intptr_t			INT_PTR;
typedef uintptr_t			ULONG_PTR;
typedef intptr_t			LONG_PTR;
#endif//OS_WINDOWS


typedef ACE_UINT8	u_int8_t;
typedef ACE_UINT16	u_int16_t;
typedef ACE_UINT32	u_int32_t;
typedef ACE_UINT64	u_int64_t;


#if defined(OS_LINUX)
#define FILE_MAP_READ        0x01 
#define FILE_MAP_WRITE       0x02 
#define FILE_MAP_ALL_ACCESS  0x03 
#define INVALID_HANDLE_VALUE NULL
#endif

typedef std::basic_string<ACE_TCHAR> STD_TString;

typedef std::vector<std::string>	CStdStringArray;
typedef std::vector<ACE_CString>	CACEStringArray;
typedef std::vector<std::wstring>	CStdWStringArray;
typedef std::vector<ACE_WString>	CACEWStringArray;
typedef std::vector<STD_TString>	CStdTStringArray;
typedef std::vector<ACE_TString>	CACETStringArray;


#define LINUX_FILENAME_SEPERATOR       ACE_TEXT('/')
#define LINUX_FILENAME_SEPERATOR_STR   ACE_TEXT("/")
#define WIN_FILENAME_SEPERATOR	       ACE_TEXT('\\')
#define WIN_FILENAME_SEPERATOR_STR     ACE_TEXT("\\")

#define LINUX_LIB_FILE_SUFFIX_STR		ACE_TEXT(".so")
#define WIN_LIB_FILE_SUFFIX_STR			ACE_TEXT(".dll")

#ifdef OS_WINDOWS
#define LIB_FILE_SUFFIX_STR			WIN_LIB_FILE_SUFFIX_STR	
#define FILENAME_SEPERATOR			WIN_FILENAME_SEPERATOR
#define FILENAME_SEPERATOR_STR		WIN_FILENAME_SEPERATOR_STR
#define IS_FILENAME_SEPERATOR(x)	((x)==WIN_FILENAME_SEPERATOR||(x)==LINUX_FILENAME_SEPERATOR)
#else
#define LIB_FILE_SUFFIX_STR			LINUX_LIB_FILE_SUFFIX_STR
#define FILENAME_SEPERATOR			LINUX_FILENAME_SEPERATOR
#define FILENAME_SEPERATOR_STR		LINUX_FILENAME_SEPERATOR_STR
#define IS_FILENAME_SEPERATOR(x)	((x)==LINUX_FILENAME_SEPERATOR)
#define MAX_PATH					260
#endif

#ifdef FALSE
#undef FALSE
#define FALSE 0
#else
#define FALSE 0
#endif

#ifdef TRUE
#undef TRUE
#define TRUE 1
#else
#define TRUE 1
#endif

//ACE_DEBUG的参数化
#define MY_ACE_DEBUG(loginstance,X) \
	do { if( (loginstance) ){\
	int __ace_error = ACE_Log_Msg::last_error_adapter (); \
	(loginstance)->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
	(loginstance)->log X; }\
	} while (0)


//////////////////////////////////////////////////////////////////////////
//字节序转化
//////////////////////////////////////////////////////////////////////////

#define ACE_SWAP_INT64(L)((ACE_SWAP_LONG((L) & 0xFFFFFFFF)<<32)\
	|ACE_SWAP_LONG(((L)>>32) & 0xFFFFFFFF))

# if defined (ACE_LITTLE_ENDIAN)
#define ACE_NTOHI64(x) ACE_SWAP_INT64(x)
#else
#define ACE_NTOHI64(x) (x)
#endif

#define ACE_HTONI64(x)	ACE_NTOHI64(x)

# if defined (ACE_LITTLE_ENDIAN)
#define CPF_LTOHL(x)	(x)
#define CPF_LTOHS(x)	(x)
#define CPF_LTOHI64(x)	(x)
# else
#define CPF_LTOHL(x)	ACE_NTOHL(x)
#define CPF_LTOHS(x)	ACE_NTOHS(x)
#define CPF_LTOHI64(x)	ACE_SWAP_INT64(x)
# endif /* ACE_LITTLE_ENDIAN */


#define CPF_HTOLL(x)	CPF_LTOHL(x)
#define CPF_HTOLS(x)	CPF_LTOHS(x)
#define CPF_HTOLI64(x)	CPF_LTOHI64(x)

#define CPF_HTONL(x)	ACE_HTONL(x)
#define CPF_HTONS(x)	ACE_HTONS(x)
#define CPF_HTONI64(x)	ACE_HTONI64(x)

# if defined (ACE_LITTLE_ENDIAN)

#define NBUF_TO_UINT(buf) (ACE_NTOHL(*(ACE_UINT32 *)(buf)))
#define NBUF_TO_WORD(buf) (ACE_NTOHS(*(ACE_UINT16 *)(buf)))
#define NBUF_TO_UI64(buf) (ACE_NTOHI64(*(ACE_UINT64 *)(buf)))

#define LBUF_TO_UINT(buf) ((*(ACE_UINT32 *)(buf)))
#define LBUF_TO_WORD(buf) ((*(ACE_UINT16 *)(buf)))
#define LBUF_TO_UI64(buf) ((*(ACE_UINT64 *)(buf)))

#else

#define NBUF_TO_UINT(buf) ((*(ACE_UINT32 *)(buf)))
#define NBUF_TO_WORD(buf) ((*(ACE_UINT16 *)(buf)))
#define NBUF_TO_UI64(buf) ((*(ACE_UINT64 *)(buf)))

#define LBUF_TO_UINT(buf) (ACE_NTOHL(*(ACE_UINT32 *)(buf)))
#define LBUF_TO_WORD(buf) (ACE_NTOHS(*(ACE_UINT16 *)(buf)))
#define LBUF_TO_UI64(buf) (ACE_NTOHI64(*(ACE_UINT64 *)(buf)))

#endif


#ifndef OS_WINDOWS
#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD)(a) & 0xff)) | ((WORD)((BYTE)((DWORD)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD)(w) >> 8))
#endif//OS_WINDOWS

#define MAKEDWORD(a, b)      MAKELONG(a,b)

#define MAKELONGLONG(a, b)      ((ACE_INT64)(((ACE_UINT32)((ACE_UINT64)(a) & 0xffffffff)) | ((ACE_UINT64)((ACE_UINT32)((ACE_UINT64)(b) & 0xffffffff))) << 32))

#define HIDWORD(v)  			((DWORD)((v)>>32))
#define LODWORD(v)  			((DWORD)(v))
#define HILONG(ll)				((long)HIDWORD(ll))
#define LOLONG(ll)				LODWORD(ll)

#ifndef mymax
#define mymax(a,b) (((a) > (b)) ? (a) : (b)) 
#endif

#ifndef mymin
#define mymin(a,b) (((a) < (b)) ? (a) : (b)) 
#endif


#define SEQ_CMP(a,b)	 ((int)((a)-(b)))

#define SEQ_LT(a,b)      ( SEQ_CMP(a,b) < 0 )
#define SEQ_LEQ(a,b)     ( SEQ_CMP(a,b) <= 0 )
#define SEQ_GT(a,b)      ( SEQ_CMP(a,b) > 0 )
#define SEQ_GEQ(a,b)     ( SEQ_CMP(a,b) >= 0 )

#define TABLE_MODEL_CMP_VALUE(a,b)	(((a)>(b))?1:(((a)==(b))?0:-1))


//客户端到服务器之间的方向定义
typedef enum{
	DIR_CS_UNKNOWN=-1, //未知
	DIR_S2C = 0,//服务器到客户
	DIR_C2S = 1//客户到服务器
	
}DIR_CS;

#define GET_DIR(cmpvalue)	( (cmpvalue)==1?DIR_C2S:((cmpvalue)==-1?DIR_S2C:DIR_CS_UNKNOWN) )

#define IS_CLIENT_TO_SERVER(dir)	(DIR_C2S==(dir))
#define IS_SERVER_TO_CLIENT(dir)	(DIR_S2C==(dir))

//内网到外网之间的方向定义
typedef enum{
	DIR_IO_UNKNOWN=-1, //未知
	DIR_O2I = 0,//外部到内部
	DIR_I2O = 1//内部到外部
}DIR_IO;

#define IS_INNER_TO_OUTTER(dir)		(DIR_I2O==(dir))
#define IS_OUTTER_TO_INNER(dir)		(DIR_O2I==(dir))

#define ONE_K_UNIT	(1000)
#define ONE_M_UNIT	(1000*1000)
#define ONE_G_UNIT	(1000*1000*1000)

#ifdef OS_WINDOWS
#define GetTickCount_us()	(((ACE_UINT64)GetTickCount())*1000)
#endif

// 20101029 add by someone
#ifdef OS_LINUX
#define	INVALID_SOCKET			-1
#define SOCKET_ERROR			-1
#define closesocket				close
#define GetCurrentProcessId		getpid
#define stricmp					strcasecmp
#define MAXDWORD    0xffffffff
typedef void   *HANDLE;
#define NO_ERROR 	0L	// dderror
//#define CopyFile(oname, nname, flag) ((link(oname, nname) == -1) ? FALSE : TRUE)
inline BOOL CopyFile(
			  LPCTSTR lpExistingFileName, // name of an existing file
			  LPCTSTR lpNewFileName,      // name of new file
			  BOOL bFailIfExists          // operation if file exists
			  )
{
        if (bFailIfExists)
        {
                FILE* fp = fopen(lpNewFileName, "r");
                if (fp)
                {
                        fclose(fp);
                        return FALSE;
                }
        }
        char str_cmd[MAX_PATH];
        sprintf(str_cmd, "cp -R %s %s", lpExistingFileName, lpNewFileName);
        system(str_cmd);
        return TRUE;
}


typedef struct _SYSTEMTIME {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#include <sys/times.h>

//CLOCK_MONOTONIC: 系统运行时间，从系统启动这一刻计时，不受系统时间修改的影响
inline ACE_UINT32 GetTickCount_s()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return ts.tv_sec;
}

inline ACE_UINT32 GetTickCount()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

//#define GetTickCount()			( ( DWORD )( times( NULL ) * ( 1000 / sysconf( _SC_CLK_TCK ) ) ) )


inline ACE_UINT64 GetTickCount_us()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (((ACE_UINT64)ts.tv_sec) * 1000 *1000 + ts.tv_nsec / 1000);
}

#endif//#ifdef OS_LINUX

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#endif//OS_TYPEDEF_H_ZHUZP_2006_04_08

