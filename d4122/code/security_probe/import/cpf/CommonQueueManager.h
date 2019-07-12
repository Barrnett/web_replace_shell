#ifndef	__COMMON_QUEUE_MANAGER_EXPORT_H__
#define __COMMON_QUEUE_MANAGER_EXPORT_H__
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#pragma once

#include "cpf/ostypedef.h"

#ifdef _WINDOWS
//{
//zzp_add���´���
#ifdef ETHERIO_EXPORTS
#	define	COMMONQUEUEMANAGER_EXPORTS
#endif

#ifdef CPF_DLL_EXPORTS
#	define	COMMONQUEUEMANAGER_EXPORTS
#endif

//zzp_add���ϴ��� 
	
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



// �±���linux����ʹ��
#ifdef OS_LINUX

// �±���linux��������ʹ��
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

// ������״̬����

// �����Ͷ���
//#define QUEUE_PACKET_INVALID	(DWORD)-1
// ����ʱ�����ڴ��еİ����Ͷ���
#define PACKET_TYPE_INVALID			0		// �ް�
#define PACKET_TYPE_PADDING			-1		// ��β������
#define PACKET_TYPE_TIMED			-2		// ��ʱ��
#define PACKET_TYPE_MEM_ERROR		-3		// �ڴ����
#define PACKET_TYPE_PKT_ERROR		-4		// ��ͷ����

#pragma pack(push)
#pragma pack(1)

// ����ͷ���ṹ
typedef struct _SHARE_BUFFER_HEAD_
{
	UINT_PTR	pWritePosition;	// ��¼������һ��Ҫд����֡��λ�ã�����ڻ�������ʼλ�õ�ƫ����
	UINT_PTR	pReadPosition;	// ��¼������һ��Ҫ������֡��λ�ã�����ڻ�������ʼλ�õ�ƫ����
	UINT_PTR	dwBufferLength;	// �������ܴ�С������SHARE_BUFFER_HEAD����ʹ�����ݲ���

	UINT_PTR	iReadPacketCount;	//��ǰ�������У������˶��ٸ�����
	UINT_PTR	iWritePacketCount;	//��ǰ�������У�������ٸ�����
	UINT_PTR	pTmpReadPosition;	//��ʱ��ָ��
	UINT_PTR	iTmpReadPacketCount;//��ʱ�����˶��ٸ���
}SHARE_BUFFER_HEAD, *PSHARE_BUFFER_HEAD;

//���ͻص��Ĵ���
typedef struct tagCQM_TMP_EX_MGR
{
	UINT_PTR head;
	UINT_PTR cur;

	UINT_PTR *	mgr_table;
	UINT_PTR	table_size;

}CQM_TMP_EX_MGR;

// ���нṹ
typedef struct tagCommonQueueManager
{
	PUCHAR				m_pQueueBaseAddress;		// ���������л���ַ
	SHARE_BUFFER_HEAD*	m_pShareBufferHead;		// ������ͷ���ṹָ���ָ��
	DWORD		m_dwBufferLength;
	DWORD		m_dwLastAllocateLength;
	int			m_nLastReadPktLen;
	int			m_nReserved;

	CQM_TMP_EX_MGR cmq_tmp_ex;

}TAG_COMMOM_QUEUE_MANAGER, *PTAG_COMMOM_QUEUE_MANAGER;

#pragma pack(pop)


///////////////////////////////////////////////////////////
//
//	��ʼ��һ�����л�����
//
//	���:
//		pQueue - ���нṹָ��
//		pQueueBaseAddresspQueue - ������ָ��Ļ�������ַ
//		dwBufferLength -������ָ��Ļ������ĳ��ȵ�ַ
//
//  ����:
COMMONQUEUEMANAGER_API VOID  QueueInitialize_new(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN PUCHAR pQueueBaseAddress, IN DWORD dwBufferLength,IN int temp_read_param);


//�����Ѿ���ʼ�����ˣ�����ֻҪ��ʼ���ṹ���У�����Ҫ��ʼ�������ڴ��еĶ�
COMMONQUEUEMANAGER_API VOID  QueueInitialize_flow(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN PUCHAR pQueueBaseAddress,IN int temp_read_param);

COMMONQUEUEMANAGER_API VOID  ZeroInitQueue(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);

COMMONQUEUEMANAGER_API VOID  QueueUnInitialize(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);

///////////////////////////////////////////////////////////
//
//	�����������
//
//	���:
//		pQueue - ���нṹָ��
//
//  ����:
COMMONQUEUEMANAGER_API VOID  QueueClearQueue(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);
COMMONQUEUEMANAGER_API VOID  QueueClearShareBuffer(IN SHARE_BUFFER_HEAD *pBuffer, DWORD dwLen);

// ��������������ֵΪ�����ȣ���ָ���ò�������
///////////////////////////////////////////////////////////
//
//	�Ӷ��л��������ȡһ�����ݰ�,ָʾ���ݰ��׵�ַ,���������ݸ���
//
//	���:
//		pQueue - ���нṹָ��
//		pPacketPointer - �������ݰ����׵�ַ
//
//  ����:
//		> 0 - ���ݱ��ĳ���
//		0 - û�ж������ݰ�
//		< 0 - ����״̬
COMMONQUEUEMANAGER_API int QueueReadCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer);//, DWORD &dwAPacketLength);

///////////////////////////////////////////////////////////
//
//	�Ӷ��л�������������ǰ���ݰ�,������ͷ��ָ�����
//
//	���:	
//		pQueue - ���нṹָ��
//
//  ����:	��
COMMONQUEUEMANAGER_API VOID  QueueSkipCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);


//�����QueueReadCurrentPacket��QueueReadCurrentPacket
COMMONQUEUEMANAGER_API int QueueReadNextPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer);//, DWORD &dwAPacketLength);

///////////////////////////////////////////////////////////
//
//	�Ӷ��л�����������һ�黺�������������Ƿ���������Ҫ
//
//	���:	
//		pQueue - ���нṹָ��
//		dwWriteLength - ����Ļ���������
//
//  ����:	������ָ��
//		NULL -- ����ʧ��
COMMONQUEUEMANAGER_API PVOID QueueWriteAllocateBuffer(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN DWORD dwWriteLength);	// ���뻺��������������ֵΪ��������ַ

///////////////////////////////////////////////////////////
//
//	�Ӷ��л�������ռ��һ�黺����
//
//	���:	
//		pQueue - ���нṹָ��
//		dwWriteLength - ʹ�õĻ���������
//
//  ����:
//		
COMMONQUEUEMANAGER_API BOOL  QueueWriteDataFinished(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN DWORD dwWriteLength);		//����д������ɣ�Ӧ���ƶ�����

///////////////////////////////////////////////////////////
//
//	���ض���������ʹ���˵Ŀռ�
//
//	���:	
//		pQueue - ���нṹָ��
//		dwTotalLength - ���ػ������ܳ���
//
//  ����:	��ʹ���˵��ֽ���
COMMONQUEUEMANAGER_API DWORD QueueQueryUsedBytes(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT DWORD *dwTotalLength);	// ��������ʹ���˵Ŀռ�


///////////////////////////////////////////////////////////
//
//	���ض���������δʹ�õĿռ�
//
//	���:	
//		pQueue - ���нṹָ��
//		dwTotalLength - ���ػ������ܳ���
//
//  ����:	δʹ�õ��ֽ���
COMMONQUEUEMANAGER_API DWORD QueueQueryUnUsedBytes(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT DWORD *dwTotalLength);


///////////////////////////////////////////////////////////
//
//	���ض��������Ѵ��ڵ����ݰ�����
//
//	���:	
//		pQueue - ���нṹָ��
//
//  ����ֵ:	ʣ������ݰ�����
COMMONQUEUEMANAGER_API DWORD QueueQueryPacketCount(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue);

//��ʱ����
//��ʱ�������������ʱ��ָ����ͷ��ǰ���˳��������ֱ��ʹ��
COMMONQUEUEMANAGER_API int QueueReadCurrentPacket_Tmp(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer);
COMMONQUEUEMANAGER_API VOID  QueueSkipCurrentPacket_Tmp(IN TAG_COMMOM_QUEUE_MANAGER *pQueue);

COMMONQUEUEMANAGER_API int QueueQueryPacketCount_Tmp(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue);


COMMONQUEUEMANAGER_API int QueueReadCurrentPacket_Tmp_Ex(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer,OUT UINT_PTR * index);
COMMONQUEUEMANAGER_API VOID  QueueSkipCurrentPacket_Ex(IN TAG_COMMOM_QUEUE_MANAGER *pQueue,UINT_PTR index);

//ͳ���Ѿ����ͣ�����û�з�����ɵ�����
COMMONQUEUEMANAGER_API int QueueQuerySendedButNotCompletedPkts(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue);

#ifdef __cplusplus
}
#endif
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
#endif // __COMMON_QUEUE_MANAGER_EXPORT_H__