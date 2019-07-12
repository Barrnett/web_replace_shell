// CapAndPlayBack.cpp : Defines the entry point for the DLL application.
//

#include "InnerCommon.h"
#include "cpf/path_tools.h"


BOOL IsValidCPTPathName(LPCSTR szFileName)
{
	// ���в����� /
#ifdef OS_WINDOWS
	if(NULL != strchr(szFileName,'/'))
	{
		return FALSE;
	}
#endif
	
	if( !CPF::IsFullPathName(szFileName) )// ��������·��
	{
		// ������ \ ��ͷ
		if(szFileName[0] == FILENAME_SEPERATOR)
		{
			return FALSE;
		}
		
		// ���в����� .,.. ��Ϊ���·��
		char buf[MAX_PATH];
		strcpy(buf,szFileName);
		char * ps = strrchr(buf,FILENAME_SEPERATOR);
		if(ps != NULL) // ��ȥ���ļ�����Ŀ¼
		{
			if(ps != buf)
				* ps = 0;
			if(NULL != strchr(buf,'.'))
			{
				return FALSE;
			}
		}
		return TRUE;		
	} // if ���·��
	
	// ����·��
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
	
	if( !CPF::IsFullPathName(strFileName) )// ��������·��
	{
		// �ϳɾ���·��
		char dir[MAX_PATH];
		
		CPF::GetModuleFullFileName(dir,sizeof(dir),strFileName);
		
		strFileName = dir;
	} // if ���·��
	
	return TRUE;
}

// �����ļ���Ψһ�����ڴ湲������Ҫ�����в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
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

// �����ļ���Ψһ�����ļ��ڴ�ӳ������Ҫ�����в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
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
	
    nPartSum  = (nPartSum >> 16) + (nPartSum & 0xFFFF); //��λ����
    nPartSum += (nPartSum >> 16);  //���ܵĽ�λ����
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

