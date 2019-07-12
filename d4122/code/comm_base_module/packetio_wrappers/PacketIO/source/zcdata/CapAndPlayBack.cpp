// CapAndPlayBack.cpp : Defines the entry point for the DLL application.
//

#include "InnerCommon.h"
#include "cpf/path_tools.h"


BOOL IsValidCPTPathName(LPCSTR szFileName)
{
	// 其中不能有 /
#ifdef OS_WINDOWS
	if(NULL != strchr(szFileName,'/'))
	{
		return FALSE;
	}
#endif
	
	if( !CPF::IsFullPathName(szFileName) )// 如果是相对路径
	{
		// 不能以 \ 开头
		if(szFileName[0] == FILENAME_SEPERATOR)
		{
			return FALSE;
		}
		
		// 其中不能有 .,.. 作为相对路径
		char buf[MAX_PATH];
		strcpy(buf,szFileName);
		char * ps = strrchr(buf,FILENAME_SEPERATOR);
		if(ps != NULL) // 除去了文件名的目录
		{
			if(ps != buf)
				* ps = 0;
			if(NULL != strchr(buf,'.'))
			{
				return FALSE;
			}
		}
		return TRUE;		
	} // if 相对路径
	
	// 绝对路径
	return TRUE;
}

BOOL TransToFullPath(CStr & strFileName)
{
#ifdef OS_WINDOWS
	for(int i = 0; i < strFileName.GetLength(); ++i)
	{
		if( strFileName[i] == LINUX_FILENAME_SEPERATOR )
		{
			strFileName[i] = WIN_FILENAME_SEPERATOR;
		}
	}
#endif

	if(!IsValidCPTPathName(strFileName))
	{
		return FALSE;
	}
	
	if( !CPF::IsFullPathName(strFileName) )// 如果是相对路径
	{
		// 合成绝对路径
		char dir[MAX_PATH];
		
		CPF::GetModuleFullFileName(dir,sizeof(dir),strFileName);
		
		strFileName = dir;
	} // if 相对路径
	
	return TRUE;
}

// 创建文件的唯一共享内存共享名，要求其中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
LPCSTR CPTMakeShareMemName(LPCSTR szFileName,CStr& strSMName)
{
	CStr strPathName = szFileName;
	
	if(!TransToFullPath(strPathName))
	{
		ACE_ASSERT(0);
		return NULL;
	}
	
	strPathName.MakeUpper();
	strSMName.Format("CPTOP_%s",(LPCSTR)strPathName);
	strSMName.Replace(FILENAME_SEPERATOR,'_');
	return (LPCSTR)strSMName;
}

// 创建文件的唯一命名文件内存映射名，要求其中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
LPCSTR CPTMakeMapHandleName(LPCSTR szFileName,CStr& strMapHandleName)
{
	CStr strPathName = szFileName;
	
	if(!TransToFullPath(strPathName))
	{
		ACE_ASSERT(0);
		return NULL;
	}
	
	strPathName.MakeUpper();
	
	strMapHandleName.Format("CPTHM_%s",(LPCSTR)strPathName);
	strMapHandleName.Replace(FILENAME_SEPERATOR,'_');
	return (LPCSTR)strMapHandleName;
}

WORD CPTFileHeadComputeCheckSum(BYTE* pPktChecked, int nLenChecked)
{
    WORD wBit16;
	UINT nPartSum = 0;
	
	int nLenEven = ((nLenChecked >> 1) << 1);
	for(int i = 0; i < nLenEven; i += 2)
	{
		wBit16 = pPktChecked[i];
		wBit16 = ((wBit16 << 8) | pPktChecked[i+1]);
		nPartSum += wBit16;
	}
	if (nLenEven != nLenChecked)
	{
		wBit16 = pPktChecked[nLenEven];
		wBit16 <<= 8;
		nPartSum += wBit16;
	}
	
    nPartSum  = (nPartSum >> 16) + (nPartSum & 0xFFFF); //进位补偿
    nPartSum += (nPartSum >> 16);  //可能的进位补偿
    return (WORD)(~nPartSum);
}

#ifdef OS_WINDOWS
int ftruncate64(FILE_ID hFile,off64_t off)
{	
	ULONG loLong = LOLONG(off);
	long  hiLong = HILONG(off);
	DWORD dwRet = SetFilePointer(hFile,loLong,&hiLong,FILE_BEGIN);
	if(dwRet != (DWORD)-1)
		return !SetEndOfFile(hFile);

	return -1;
}
#endif

