#ifndef _CAPCONFIG_SUNXG_2003_5_27
#define _CAPCONFIG_SUNXG_2003_5_27

//--------------------------------------------------------------------------------
// 文件名：		CapConfig.h
// 创建者：		
// 创建时间：	2003.5.27
// 内容描述：	定义捕获设置,包括捕获到内存和文件
// 修改记录：
// 修改时间	修改者		修改内容
// …
//--------------------------------------------------------------------------------

#include "Str.h"

class CCapConfig
{
public:
	CCapConfig()
	{
		bCapToMem = FALSE;
		bUseFilter = FALSE;
		bSimpleFilter = TRUE;
		bMultiFile = TRUE;
	}
public:
	CStr strFilePath;
	BOOL bMultiFile;
	int nFileSizeMb;		// 如果捕获到内存,此为内存大小
	BOOL bUseFilter;
	BOOL bCPTOnly;			// 只生成CPT文件，不生成.NST文件
	BOOL bSimpleFilter;		// 是否简单过滤
	BOOL bCapToMem;			// 是否捕获到内存
	CStr strFilterName;	// 过滤器名称
};				


#endif
