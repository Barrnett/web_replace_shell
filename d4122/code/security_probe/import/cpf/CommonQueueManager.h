#ifndef	__COMMON_QUEUE_MANAGER_EXPORT_H__
#define __COMMON_QUEUE_MANAGER_EXPORT_H__
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#pragma once

#include "cpf/ostypedef.h"

#ifdef _WINDOWS
//{
//zzp_add以下代码
#ifdef ETHERIO_EXPORTS
#	define	COMMONQUEUEMANAGER_EXPORTS
#endif

#ifdef CPF_DLL_EXPORTS
#	define	COMMONQUEUEMANAGER_EXPORTS
#endif

//zzp_add以上代码 
	
#ifdef COMMONQUEUEMANAGER_EXPORTS
	#ifdef	__cplusplus
		#define COMMONQUEUEMANAGER_API extern "C" __declspec(dllexport)
	#else
		#define COMMONQUEUEMANAGER_API __declspec(dllexport)
	#endif
#else	// ZETHERNIC_EXPORTS
	#ifdef	__cplusplus
	#define COMMONQUEUEMANAGER_API extern "C" __declspec(dllimport)
	#else
	#define COMMONQUEUEMANAGER_API __declspec(dllimport)
	#endif
#endif	// ZETHERNIC_EXPORTS

#ifdef COMMONQUEUEMANAGER_EXPORTS
	#include <WINSOCK2.H>
	#include <windows.h>
	#include <assert.h>
	#ifdef _DEBUG
		#define ASSERT(x)	assert(x)
	#else
		#define ASSERT(x)	
	#endif
#endif//_DEBUG#endif	// ZETHERNIC_EXPORTS

#ifdef WIN32
	#include <WINSOCK2.H>
	#include "windows.h"
#else
	#include "ntddk.h"
	#include "ndis.h"
	#include "windef.h"
#endif	// #ifdef WIN32

#endif // _WINDOWS



// 下边是linux编译使用
#ifdef OS_LINUX

// 下边是linux驱动编译使用
#	ifdef OS_LINUX_DLL
#		include <string.h>
#		include <linux/stddef.h>
#	endif // OS_LINUX_DLL

#define IN
#define OUT

#define FALSE 0
#define TRUE  1

///packet family macro --  tmp

#define PF_PACKET_1 37
#define PF_PACKET_2 38
#define PF_PACKET_3 39
#define PF_PACKET_4 40
#define PF_PACKET_5 41

#define iPF_PACKET PF_PACKET_1

#define PACKED  __attribute__ ((packed))

// Put platform dependent declaration here
// For example, linux type definition
typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;
typedef unsigned long long	UINT64;
typedef int				INT32;
typedef long long 			INT64;

typedef unsigned char *		PUINT8;
typedef unsigned short *		PUINT16;
typedef unsigned int *			PUINT32;
typedef unsigned long long *	PUINT64;
typedef int	*				PINT32;
typedef long long * 			PINT64;

typedef signed char			CHAR;
typedef signed short		SHORT;
typedef signed int			INT;
typedef signed long			LONG;


typedef unsigned char		BYTE;
typedef unsigned short		USHORT;
typedef unsigned int		UINT;
typedef unsigned long		ULONG;
#if !( (defined OS_LINUX) && (defined _X64)  )
typedef unsigned long long	ULONGLONG;
typedef signed long long	LONGLONG;
#endif


typedef unsigned char		BOOLEAN;
typedef void				VOID;

typedef VOID *				PVOID;
typedef CHAR *				PCHAR;
typedef USHORT *			PUSHORT;
typedef LONG *				PLONG;
typedef ULONG *				PULONG;
typedef UINT *				PUINT;

typedef unsigned int 			DWORD;
typedef int             			BOOL;

typedef unsigned int	NDIS_MEDIA_STATE;

//#ifndef LARGE_INTEGER
//typedef union _LARGE_INTEGER {
//	struct {
//		DWORD	LowPart;
//		DWORD	HighPart;
//	} u;
//	LONGLONG QuadPart;
//} LARGE_INTEGER;
//#endif

#ifndef ASSERT
#	ifdef _DEBUG
#		include "assert.h"
#		define ASSERT(_x_)	assert(_x_)
#	else
#		define ASSERT(_x_)
#	endif
#endif

typedef unsigned char		UCHAR;
typedef UCHAR * 			PUCHAR;

#ifdef _i386
typedef uintptr_t                       UINT_PTR;
typedef intptr_t                        INT_PTR;
typedef uintptr_t                       ULONG_PTR;
typedef intptr_t                        LONG_PTR;
#else
typedef unsigned long		UINT_PTR;
typedef long				INT_PTR;
#endif
typedef void*				HANDLE;

#define INVALID_HANDLE_VALUE -1

#endif	//#ifdef OS_WINDOWS

#ifndef COMMONQUEUEMANAGER_API 
#	define COMMONQUEUEMANAGER_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define INT_PTR_ALIGN(len) (((len)+sizeof(UINT_PTR)-1) & ~(sizeof(UINT_PTR)-1))

// 缓冲区状态定义

// 包类型定义
//#define QUEUE_PACKET_INVALID	(DWORD)-1
// 接收时共享内存中的包类型定义
#define PACKET_TYPE_INVALID			0		// 无包
#define PACKET_TYPE_PADDING			-1		// 结尾的填充包
#define PACKET_TYPE_TIMED			-2		// 定时包
#define PACKET_TYPE_MEM_ERROR		-3		// 内存出错
#define PACKET_TYPE_PKT_ERROR		-4		// 包头出错

#pragma pack(push)
#pragma pack(1)

// 队列头部结构
typedef struct _SHARE_BUFFER_HEAD_
{
	UINT_PTR	pWritePosition;	// 纪录的是下一个要写数据帧的位置，相对于缓冲区起始位置的偏移量
	UINT_PTR	pReadPosition;	// 纪录的是下一个要读数据帧的位置，相对于缓冲区起始位置的偏移量
	UINT_PTR	dwBufferLength;	// 缓冲区总大小，包括SHARE_BUFFER_HEAD本身和存放数据部分

	UINT_PTR	iReadPacketCount;	//当前缓冲区中，读走了多少个包了
	UINT_PTR	iWritePacketCount;	//当前缓冲区中，放入多少个包了
	UINT_PTR	pTmpReadPosition;	//临时读指针
	UINT_PTR	iTmpReadPacketCount;//临时读走了多少个包
}SHARE_BUFFER_HEAD, *PSHARE_BUFFER_HEAD;

//发送回调的处理
typedef struct tagCQM_TMP_EX_MGR
{
	UINT_PTR head;
	UINT_PTR cur;

	UINT_PTR *	mgr_table;
	UINT_PTR	table_size;

}CQM_TMP_EX_MGR;

// 队列结构
typedef struct tagCommonQueueManager
{
	PUCHAR				m_pQueueBaseAddress;		// 缓冲区队列基地址
	SHARE_BUFFER_HEAD*	m_pShareBufferHead;		// 缓冲区头部结构指针的指针
	DWORD		m_dwBufferLength;
	DWORD		m_dwLastAllocateLength;
	int			m_nLastReadPktLen;
	int			m_nReserved;

	CQM_TMP_EX_MGR cmq_tmp_ex;

}TAG_COMMOM_QUEUE_MANAGER, *PTAG_COMMOM_QUEUE_MANAGER;

#pragma pack(pop)


///////////////////////////////////////////////////////////
//
//	初始化一个队列缓冲区
//
//	入口:
//		pQueue - 队列结构指针
//		pQueueBaseAddresspQueue - 队列所指向的缓冲区地址
//		dwBufferLength -队列所指向的缓冲区的长度地址
//
//  出口:
COMMONQUEUEMANAGER_API VOID  QueueInitialize_new(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN PUCHAR pQueueBaseAddress, IN DWORD dwBufferLength,IN int temp_read_param);


//别人已经初始化好了，这里只要初始化结构就行，不需要初始化共享内存中的东
COMMONQUEUEMANAGER_API VOID  QueueInitialize_flow(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN PUCHAR pQueueBaseAddress,IN int temp_read_param);

COMMONQUEUEMANAGER_API VOID  ZeroInitQueue(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);

COMMONQUEUEMANAGER_API VOID  QueueUnInitialize(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);

///////////////////////////////////////////////////////////
//
//	清除队列内容
//
//	入口:
//		pQueue - 队列结构指针
//
//  出口:
COMMONQUEUEMANAGER_API VOID  QueueClearQueue(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);
COMMONQUEUEMANAGER_API VOID  QueueClearShareBuffer(IN SHARE_BUFFER_HEAD *pBuffer, DWORD dwLen);

// 读包操作，返回值为包长度，包指针用参数返回
///////////////////////////////////////////////////////////
//
//	从队列缓冲区里读取一个数据包,指示数据包首地址,不进行数据复制
//
//	入口:
//		pQueue - 队列结构指针
//		pPacketPointer - 返回数据包的首地址
//
//  出口:
//		> 0 - 数据报的长度
//		0 - 没有读到数据包
//		< 0 - 错误状态
COMMONQUEUEMANAGER_API int QueueReadCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer);//, DWORD &dwAPacketLength);

///////////////////////////////////////////////////////////
//
//	从队列缓冲区里跳过当前数据包,将队列头部指针后移
//
//	入口:	
//		pQueue - 队列结构指针
//
//  出口:	无
COMMONQUEUEMANAGER_API VOID  QueueSkipCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);


//结合了QueueReadCurrentPacket和QueueReadCurrentPacket
COMMONQUEUEMANAGER_API int QueueReadNextPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer);//, DWORD &dwAPacketLength);

///////////////////////////////////////////////////////////
//
//	从队列缓冲区里申请一块缓冲区，检查队列是否能满足需要
//
//	入口:	
//		pQueue - 队列结构指针
//		dwWriteLength - 申请的缓冲区长度
//
//  出口:	缓冲区指针
//		NULL -- 申请失败
COMMONQUEUEMANAGER_API PVOID QueueWriteAllocateBuffer(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN DWORD dwWriteLength);	// 申请缓冲区操作，返回值为缓冲区地址

///////////////////////////////////////////////////////////
//
//	从队列缓冲区里占用一块缓冲区
//
//	入口:	
//		pQueue - 队列结构指针
//		dwWriteLength - 使用的缓冲区长度
//
//  出口:
//		
COMMONQUEUEMANAGER_API BOOL  QueueWriteDataFinished(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN DWORD dwWriteLength);		//表明写操作完成，应当移动队列

///////////////////////////////////////////////////////////
//
//	返回队列里面已使用了的空间
//
//	入口:	
//		pQueue - 队列结构指针
//		dwTotalLength - 返回缓冲区总长度
//
//  出口:	已使用了的字节数
COMMONQUEUEMANAGER_API DWORD QueueQueryUsedBytes(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT DWORD *dwTotalLength);	// 队列里面使用了的空间


///////////////////////////////////////////////////////////
//
//	返回队列里面已未使用的空间
//
//	入口:	
//		pQueue - 队列结构指针
//		dwTotalLength - 返回缓冲区总长度
//
//  出口:	未使用的字节数
COMMONQUEUEMANAGER_API DWORD QueueQueryUnUsedBytes(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT DWORD *dwTotalLength);


///////////////////////////////////////////////////////////
//
//	返回队列里面已存在的数据包个数
//
//	入口:	
//		pQueue - 队列结构指针
//
//  返回值:	剩余的数据包个数
COMMONQUEUEMANAGER_API DWORD QueueQueryPacketCount(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue);

//临时操作
//临时读操作，如果临时读指针的释放是按照顺序来的则直接使用
COMMONQUEUEMANAGER_API int QueueReadCurrentPacket_Tmp(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer);
COMMONQUEUEMANAGER_API VOID  QueueSkipCurrentPacket_Tmp(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);

COMMONQUEUEMANAGER_API int QueueQueryPacketCount_Tmp(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue);


COMMONQUEUEMANAGER_API int QueueReadCurrentPacket_Tmp_Ex(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer,OUT UINT_PTR * index);
COMMONQUEUEMANAGER_API VOID  QueueSkipCurrentPacket_Ex(IN TAG_COMMOM_QUEUE_MANAGER *pQueue,UINT_PTR index);

//统计已经发送，但是没有发送完成的数量
COMMONQUEUEMANAGER_API int QueueQuerySendedButNotCompletedPkts(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue);

#ifdef __cplusplus
}
#endif
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
#endif // __COMMON_QUEUE_MANAGER_EXPORT_H__