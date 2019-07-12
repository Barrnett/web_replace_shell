#ifndef _CPTOPSTRUCT_H_SUNXG_2003_5_21
#define _CPTOPSTRUCT_H_SUNXG_2003_5_21
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


#include "CapFileHead.h"

#define MAX_ONE_CPT_INDEX	200000	// 按每包长为0估算,头长22字节,1000包建立一个索引,在4G长的文件中足够使用

#pragma pack(1)

typedef struct 
{
	BOOL		m_bWriting;					// 该共享缓存是否由CCPTSingleWrite创建

	char		m_strFilePathName[MAX_PATH];	// 带路径存盘文件名
	
	DWORD		m_dwPacketCount;			// 本文件中已经记录的包数
	

	DWORD		m_dwMapSize;				// 映射文件大小
	DWORD		m_dwContentEnd;				// 实际内容的尾部
	
	CAPFILE_HEAD m_filehead;				// 文件头
	
	DWORD		m_dwIndexCount;				// 索引个数
	DWORD		m_adwPktIndex[MAX_ONE_CPT_INDEX];//约781K索引数组
	
} CPT_OP_STRUCT;

#pragma pack()

// 如果连续记录，获取下一个文件名的回调函数，返回值应该不含路径，最长FILENAME_LEN
typedef LPCSTR (* GET_NEXT_FILE_NAME_FUNC) (void * pParam);



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
#endif//_CPTOPSTRUCT_H_SUNXG_2003_5_21


