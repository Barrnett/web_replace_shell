#ifndef _CAPOPSTRUCT_H_SUNXG_2003_5_21
#define _CAPOPSTRUCT_H_SUNXG_2003_5_21

#include "CapFileHead.h"
#include "cpf/MyStr.h"

#define MAX_SINGLE_CPT	10000		// 按每包长为0估算,头长22字节,1000包建立一个索引,在4G长的文件中足够使用

#pragma pack(1)


typedef struct 
{
	ACE_UINT64 ullPktFirstPos;		// 此文件中的记录第一个帧全局位置
	ACE_UINT64 ullPktEndPos;			// 此文件中的记录最后全局位置
	ACE_UINT64 ullFirstFrame;		// 此文件中的记录第一个帧全局帧编号
	DWORD dwFrameCount;				// 此文件中记录的帧的个数
	char szFileName[MAX_PATH];		// 不带路径文件名
}SINGLE_FILE_INFO; 	//单个文件表项

typedef struct 
{
	BOOL		m_bWriting;					// 该共享缓存是否由CCPTSingleWrite创建
	
	char		m_strOrgFilePathName[MAX_PATH];	// 带路径原始文件名
	
	int			m_nFileCount;					// 文件个数
	ACE_UINT64   m_ullTotalFrame;				// 包个数
    ACE_UINT64   m_ullTotalSize;				    // 所有文件大小
    SINGLE_FILE_INFO m_Files[MAX_SINGLE_CPT];	// 文件列表
	
} CAP_OP_STRUCT;

#pragma pack()

// 如果连续记录，获取下一个文件名的回调函数，返回值应该不含路径，最长FILENAME_LEN
typedef LPCSTR (* GET_NEXT_FILE_NAME_FUNC) (void * pParam);

// 创建文件的一一对应的共享内存共享名，要求其中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
void CAPMakeShareMemName(LPCSTR szFileName,CStr& strSMName);

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#endif


