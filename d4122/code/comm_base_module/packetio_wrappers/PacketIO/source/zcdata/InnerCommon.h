////////////////////////////////////////////////////
//InnerCommon.h

#ifndef _INNER_COMMON_H_ZHUZP_2005_06_01
#define _INNER_COMMON_H_ZHUZP_2005_06_01
////////////////////////////////////////////////////
////////////////////////////////////////////////////

#include "cpf/MyStr.h"
#include "CapFileHead.h"

BOOL IsValidCPTPathName(LPCSTR szFileName);

// 创建文件的一一对应的共享内存共享名，要求其中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
LPCSTR CPTMakeShareMemName(LPCSTR szFileName,CStr& strSMName);
// 创建文件的唯一命名文件内存映射名，要求其中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
LPCSTR CPTMakeMapHandleName(LPCSTR szFileName,CStr& strSMName);
// 转换为全路径
BOOL TransToFullPath(CStr & strFileName);

WORD CPTFileHeadComputeCheckSum(BYTE* pPktChecked, int nLenChecked);

#ifdef OS_WINDOWS
int ftruncate64(FILE_ID hFile,off64_t off);
#endif

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#endif//_INNER_COMMON_H_ZHUZP_2005_06_01
