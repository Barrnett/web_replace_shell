////////////////////////////////////////////////////
//InnerCommon.h

#ifndef _INNER_COMMON_H_ZHUZP_2005_06_01
#define _INNER_COMMON_H_ZHUZP_2005_06_01
////////////////////////////////////////////////////
////////////////////////////////////////////////////

#include "cpf/MyStr.h"
#include "CapFileHead.h"

BOOL IsValidCPTPathName(LPCSTR szFileName);

// �����ļ���һһ��Ӧ�Ĺ����ڴ湲������Ҫ�����в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
LPCSTR CPTMakeShareMemName(LPCSTR szFileName,CStr& strSMName);
// �����ļ���Ψһ�����ļ��ڴ�ӳ������Ҫ�����в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
LPCSTR CPTMakeMapHandleName(LPCSTR szFileName,CStr& strSMName);
// ת��Ϊȫ·��
BOOL TransToFullPath(CStr & strFileName);

WORD CPTFileHeadComputeCheckSum(BYTE* pPktChecked, int nLenChecked);

#ifdef OS_WINDOWS
int ftruncate64(FILE_ID hFile,off64_t off);
#endif

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#endif//_INNER_COMMON_H_ZHUZP_2005_06_01
