//////////////////////////////////////////////////////////////////////////
//CommonQueueManager.cpp

#ifdef OS_WINDOWS
#	ifndef _WINDOWS
#		include "precomp.h"
#	endif
#endif

#ifdef OS_LINUX
#	include <linux/types.h>
#	include <linux/kernel.h>
#endif

#include "cpf/CommonQueueManager.h"


#pragma pack(push)
#pragma pack(1)

//为了实现4字节对齐，为每一个包定义一个包头
typedef struct _COMQUEUE__PACKET_HEADER_
{
	DWORD dwAdjSize;	//每一个包调整后的大小,包括COMQUEUE_PACKET_HEADER这个头的大小
	DWORD dwOrgSize;	//每一个包的原始大小,PACKET_TYPE_PADDING 表明这个包是结尾的填充包

}COMQUEUE_PACKET_HEADER, *PCOMQUEUE_PACKET_HEADER;

#pragma pack(pop)



/*
	队列说明：
		1、读写指针都是想对于缓冲区指针的相对偏移，不是绝对地址
		2、读写指针初始状态和最小值为sizeof(SHARE_BUFFER_HEAD)，而不是0
		3、读写指针最大值应小于缓冲区长度，而不能等于此长度值
		4、在申请写缓冲区的时候，总要保留一个空包头的位置。因此缓冲区有数据时，写指针永远不能追上读指针
		5、读指针=写指针时，缓冲区为空
*/

#define _DEF_RESET_RW_POINTER(_pointer)	\
	(_pointer) = sizeof(SHARE_BUFFER_HEAD)

VOID  ZeroInitQueue(IN TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	memset(pQueue,0x00,sizeof(TAG_COMMOM_QUEUE_MANAGER));
}

VOID  QueueUnInitialize(IN TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	pQueue->cmq_tmp_ex.head = pQueue->cmq_tmp_ex.cur = 0;

	if( pQueue->cmq_tmp_ex.mgr_table )
	{
#ifdef _WIN_NT_DRV_ENV_
		NdisFreeMemory(pQueue->cmq_tmp_ex.mgr_table,0,0);
#else
		free(pQueue->cmq_tmp_ex.mgr_table);
#endif

		pQueue->cmq_tmp_ex.mgr_table = NULL;
	}

	memset(pQueue,0x00,sizeof(TAG_COMMOM_QUEUE_MANAGER));

	return;
}

VOID QueueInitialize_new(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN PUCHAR pQueueBaseAddress, IN DWORD dwBufferLength,IN int temp_read_param)
{
	ASSERT(pQueue);
	ASSERT(pQueueBaseAddress);
	ASSERT(dwBufferLength);

	pQueue->m_pQueueBaseAddress = pQueueBaseAddress;
	pQueue->m_pShareBufferHead = (SHARE_BUFFER_HEAD*)pQueueBaseAddress;
	pQueue->m_dwBufferLength = dwBufferLength;

	pQueue->m_dwLastAllocateLength = 0;
	pQueue->m_nLastReadPktLen = 0;
	pQueue->m_nReserved = 0;

	pQueue->cmq_tmp_ex.head = pQueue->cmq_tmp_ex.cur = 0;

	pQueue->cmq_tmp_ex.table_size = temp_read_param;

	if( pQueue->cmq_tmp_ex.table_size )
	{
#ifdef _WIN_NT_DRV_ENV_
		 NdisAllocateMemoryWithTag(&pQueue->cmq_tmp_ex.mgr_table,temp_read_param*sizeof(UINT_PTR),'oiE');
#else
		 pQueue->cmq_tmp_ex.mgr_table = (UINT_PTR *)malloc(temp_read_param*sizeof(UINT_PTR));
#endif
	}

	QueueClearQueue(pQueue);
}

VOID  QueueInitialize_flow(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN PUCHAR pQueueBaseAddress,IN int temp_read_param)
{
	pQueue->m_pQueueBaseAddress = pQueueBaseAddress;
	pQueue->m_pShareBufferHead = (SHARE_BUFFER_HEAD*)pQueueBaseAddress;
	pQueue->m_dwBufferLength = (DWORD)pQueue->m_pShareBufferHead->dwBufferLength;
	
	pQueue->m_dwLastAllocateLength = 0;
	pQueue->m_nLastReadPktLen = 0;
	pQueue->m_nReserved = 0;

	pQueue->cmq_tmp_ex.head = pQueue->cmq_tmp_ex.cur = 0;

	pQueue->cmq_tmp_ex.table_size = temp_read_param;

	if( pQueue->cmq_tmp_ex.table_size )
	{
#ifdef _WIN_NT_DRV_ENV_
		NdisAllocateMemoryWithTag(&pQueue->cmq_tmp_ex.mgr_table,temp_read_param*sizeof(UINT_PTR),'oiE');
#else
		pQueue->cmq_tmp_ex.mgr_table = (UINT_PTR *)malloc(temp_read_param*sizeof(UINT_PTR));
#endif
	}

	return;
}


VOID QueueClearQueue(IN TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	ASSERT(pQueue->m_dwBufferLength);

	pQueue->cmq_tmp_ex.head = pQueue->cmq_tmp_ex.cur = 0;

	if( pQueue->cmq_tmp_ex.mgr_table )
	{
		memset(
			pQueue->cmq_tmp_ex.mgr_table,0x00,
			sizeof(pQueue->cmq_tmp_ex.mgr_table[0])*pQueue->cmq_tmp_ex.table_size
			);
	}

	QueueClearShareBuffer(pQueue->m_pShareBufferHead, pQueue->m_dwBufferLength);
}

VOID QueueClearShareBuffer(IN SHARE_BUFFER_HEAD* pShareBufferHead, DWORD dwBufferLength )
{
	_DEF_RESET_RW_POINTER(pShareBufferHead->pWritePosition);
	_DEF_RESET_RW_POINTER(pShareBufferHead->pReadPosition);
	pShareBufferHead->dwBufferLength = dwBufferLength;
	pShareBufferHead->iReadPacketCount = 0;
	pShareBufferHead->iWritePacketCount = 0;

	_DEF_RESET_RW_POINTER(pShareBufferHead->pTmpReadPosition);
	pShareBufferHead->iTmpReadPacketCount = 0;
	
}

///////////////////////////////////////////////////////////
//
//	从队列缓冲区里读取一个数据包,指示数据包首地址,不进行数据复制
//
//	入口:
//		pQueue - 队列结构指针
//		pPacketPointer - 返回数据包的首地址
//
//  出口:
//		not zero - 数据报的长度
//		0 - 没有读到数据包
inline int Inner_QueueReadCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer)//, DWORD &dwAPacketLength);
{
	COMQUEUE_PACKET_HEADER*	pPacketHeader = NULL;

	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	
	// 缓冲区失效
	if ( !pQueue->m_pShareBufferHead->pReadPosition )
		return PACKET_TYPE_MEM_ERROR;

	if( pQueue->m_pShareBufferHead->iWritePacketCount <= pQueue->m_pShareBufferHead->iReadPacketCount )
	{
		return PACKET_TYPE_INVALID;
	}

	if (pQueue->m_pShareBufferHead->pWritePosition == pQueue->m_pShareBufferHead->pReadPosition)
	{
		return PACKET_TYPE_INVALID;
	}

	pPacketHeader = (COMQUEUE_PACKET_HEADER*)((UCHAR*)pQueue->m_pShareBufferHead + pQueue->m_pShareBufferHead->pReadPosition);

	// 表明是无效包，即结尾的一小部分
	if ((DWORD)PACKET_TYPE_PADDING == pPacketHeader->dwOrgSize)
	{
		// 回绕，重新读，这个填充包不算在任何计数内
		_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pReadPosition);

		return Inner_QueueReadCurrentPacket(pQueue, pPacketPointer);
	}
	else
	{
		DWORD dwAdjSize = 0;

		// 检查数据的合法性
		dwAdjSize = INT_PTR_ALIGN(pPacketHeader->dwOrgSize + sizeof(COMQUEUE_PACKET_HEADER));

		if (pPacketHeader->dwAdjSize != dwAdjSize)
		{
			SHARE_BUFFER_HEAD* pShareBufferHead = pQueue->m_pShareBufferHead;

			pShareBufferHead->iReadPacketCount = pShareBufferHead->iWritePacketCount;
			pShareBufferHead->pReadPosition = pShareBufferHead->pWritePosition;

			return PACKET_TYPE_PKT_ERROR;

		}

		*pPacketPointer = (PUCHAR)((UCHAR*)pPacketHeader + sizeof(COMQUEUE_PACKET_HEADER));

		return pPacketHeader->dwOrgSize;
	}
}

int QueueReadCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer)
{
	pQueue->m_nLastReadPktLen = Inner_QueueReadCurrentPacket(pQueue,pPacketPointer);

	return pQueue->m_nLastReadPktLen;
}

int QueueReadNextPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer)
{
	if( pQueue->m_nLastReadPktLen > 0 )
	{
		QueueSkipCurrentPacket(pQueue);

		pQueue->m_nLastReadPktLen = 0;
	}

	return QueueReadCurrentPacket(pQueue,pPacketPointer);
}

///////////////////////////////////////////////////////////
//
//	从队列缓冲区里跳过当前数据包,将队列头部指针后移
//
//	入口:	
//		pQueue - 队列结构指针
//
//  出口:	无
VOID  QueueSkipCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	UINT_PTR dwReadPos; 

	COMQUEUE_PACKET_HEADER*	pPacketHeader = NULL;
	
	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	
	pPacketHeader = (COMQUEUE_PACKET_HEADER*)( (UCHAR*)pQueue->m_pShareBufferHead + pQueue->m_pShareBufferHead->pReadPosition);

	// 表明是无效包，即结尾的一小部分
	if ((DWORD)PACKET_TYPE_PADDING == pPacketHeader->dwOrgSize)
	{
		// 回绕，重新读，这个填充包不算在任何计数内
		_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pReadPosition);

		QueueSkipCurrentPacket(pQueue);

		return;
	}
	else
	{
		dwReadPos = pQueue->m_pShareBufferHead->pReadPosition + (UINT_PTR)pPacketHeader->dwAdjSize;

		// 应该不会在skip时出现到缓冲区尾部的情况
		// 因为结尾的填充包都会在Read函数中被跳过去了
		if (dwReadPos >= pQueue->m_pShareBufferHead->dwBufferLength)
		{
			ASSERT(FALSE);
			_DEF_RESET_RW_POINTER(dwReadPos);
		}

		pQueue->m_pShareBufferHead->pReadPosition = dwReadPos;

		{
			++pQueue->m_pShareBufferHead->iReadPacketCount;

			ASSERT(pQueue->m_pShareBufferHead->iWritePacketCount>=pQueue->m_pShareBufferHead->iReadPacketCount);
		}
	}

	return;

}


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
PVOID	QueueWriteAllocateBuffer(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN DWORD dwWriteLength)	// 申请缓冲区操作，返回值为缓冲区地址
{
	COMQUEUE_PACKET_HEADER*	pPacketHeader = NULL;
	DWORD	dwActualRequestLength = 0;
	UINT_PTR	dwTailSpace, dwHeadSpace;
	UINT_PTR	pWritePosition, pReadPosition;

	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	ASSERT(dwWriteLength);
	
	// 纪录申请的缓冲区大小
	pQueue->m_dwLastAllocateLength = dwWriteLength;

	if (0 == dwWriteLength)
	{
		return NULL;
	}

	// 缓冲区失效
	if ( !pQueue->m_pShareBufferHead->pWritePosition)
	{
		pQueue->m_dwLastAllocateLength = 0;
		return NULL;
	}


	// 将需要的缓冲区大小32位对齐,需要加上当前包头和预留无效包头的长度
	// 因为预留无效包的存在，所以写指针不会追上读指针
	dwActualRequestLength = INT_PTR_ALIGN(dwWriteLength + 2*sizeof(COMQUEUE_PACKET_HEADER));

	pWritePosition = pQueue->m_pShareBufferHead->pWritePosition;
	pReadPosition = pQueue->m_pShareBufferHead->pReadPosition;

	// 写指针大于读指针
	if (pWritePosition >= pReadPosition)
	{
		dwTailSpace = pQueue->m_pShareBufferHead->dwBufferLength - pWritePosition;
		// 剩余空间满足需求
		if (dwTailSpace > dwActualRequestLength)	// 不用=判断，可防止指针指到缓冲区尾
		{
			//                      指针             +     偏移
			return (PVOID)((UCHAR*)pQueue->m_pShareBufferHead + pWritePosition + sizeof(COMQUEUE_PACKET_HEADER));
		}
		
		// 判断头部剩余空间是否满足需求
		dwHeadSpace = pReadPosition - sizeof(SHARE_BUFFER_HEAD);
		if (dwHeadSpace >= dwActualRequestLength)	// 这里可以用=判断
		{
			// 将尾部剩余部分填充为无效包
			pPacketHeader = (PCOMQUEUE_PACKET_HEADER)((UCHAR*)pQueue->m_pShareBufferHead + pWritePosition);
			pPacketHeader->dwAdjSize = (DWORD)dwTailSpace;	// 对齐长度为剩余所有的长度
			pPacketHeader->dwOrgSize = (DWORD)PACKET_TYPE_PADDING;	// 原始长度指明这个包无效

			// 修改写指针到缓冲区头部
			_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pWritePosition);

			//                      指针             +     偏移
			return (PVOID)((UCHAR*)pQueue->m_pShareBufferHead + sizeof(SHARE_BUFFER_HEAD) + sizeof(COMQUEUE_PACKET_HEADER));
		}

	}
	// 读指针大于写指针,则只剩下中间一块空间
	else
	{
		dwTailSpace = pReadPosition - pWritePosition;
		if (dwTailSpace >= dwActualRequestLength) 	// 这里可以用=判断
			return (PVOID)((UCHAR*)pQueue->m_pShareBufferHead + pWritePosition + sizeof(COMQUEUE_PACKET_HEADER));
	}

	// 没有足够空间
	pQueue->m_dwLastAllocateLength = 0;
	return NULL;
}


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
BOOL QueueWriteDataFinished(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN DWORD dwWriteLength)	//表明写操作完成，应当移动队列
{
	PCOMQUEUE_PACKET_HEADER	pPacketHeader = NULL;
	DWORD	dwActualRequestLength = 0;
	UINT_PTR	dwNewPosi;

	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);

	if( dwWriteLength == 0 )
	{
		dwWriteLength = pQueue->m_dwLastAllocateLength;
	}
	
	if (pQueue->m_dwLastAllocateLength<dwWriteLength
		|| pQueue->m_dwLastAllocateLength == 0 )
	{
		ASSERT(FALSE);
		return TRUE;
	}

	dwActualRequestLength = INT_PTR_ALIGN(dwWriteLength + sizeof(COMQUEUE_PACKET_HEADER));

	// 处理包头
	pPacketHeader = (COMQUEUE_PACKET_HEADER *)((UCHAR*)pQueue->m_pShareBufferHead + pQueue->m_pShareBufferHead->pWritePosition);
	pPacketHeader->dwOrgSize = dwWriteLength;
	pPacketHeader->dwAdjSize = dwActualRequestLength;

	// 处理缓冲区头
	dwNewPosi = pQueue->m_pShareBufferHead->pWritePosition + dwActualRequestLength;
	// 理论上讲，不会出现>=的情况，因为每次allcate的时候都提前预留了一个head的空间
	if (dwNewPosi >= pQueue->m_pShareBufferHead->dwBufferLength)
	{
		ASSERT(FALSE);
		dwNewPosi = sizeof(SHARE_BUFFER_HEAD);
	}

	pQueue->m_pShareBufferHead->pWritePosition = dwNewPosi;

	{
		++pQueue->m_pShareBufferHead->iWritePacketCount;

		ASSERT(pQueue->m_pShareBufferHead->iWritePacketCount>=pQueue->m_pShareBufferHead->iReadPacketCount);
	}


	return TRUE;
}

///////////////////////////////////////////////////////////
//
//	返回队列里面已使用了的空间
//
//	入口:	
//		pQueue - 队列结构指针
//		dwTotalLength - 返回缓冲区总长度
//
//  出口:	已使用了的字节数
DWORD	QueueQueryUsedBytes(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT DWORD *dwTotalLength)	// 队列里面使用了的空间
{
	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);

	if(dwTotalLength)
	{
		*dwTotalLength = (DWORD)(pQueue->m_pShareBufferHead->dwBufferLength-sizeof(SHARE_BUFFER_HEAD) );
	}

	if ( pQueue->m_pShareBufferHead->pWritePosition == pQueue->m_pShareBufferHead->pReadPosition)
	{
		return 0;
	}
	else if (pQueue->m_pShareBufferHead->pWritePosition > pQueue->m_pShareBufferHead->pReadPosition)
	{
		return (DWORD)(pQueue->m_pShareBufferHead->pWritePosition - pQueue->m_pShareBufferHead->pReadPosition);
	}
	else
	{
		return (DWORD) 
			(	pQueue->m_pShareBufferHead->pWritePosition - sizeof(SHARE_BUFFER_HEAD) +
				pQueue->m_pShareBufferHead->dwBufferLength - pQueue->m_pShareBufferHead->pReadPosition
			);
	}
	
}


///////////////////////////////////////////////////////////
//
//	返回队列里面已未使用的空间
//
//	入口:	
//		pQueue - 队列结构指针
//		dwTotalLength - 返回缓冲区总长度
//
//  出口:	未使用的字节数
DWORD	QueueQueryUnUsedBytes(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT DWORD *dwTotalLength)	// 队列里面使用了的空间
{
	DWORD dwUsed = QueueQueryUsedBytes(pQueue, dwTotalLength);
	
	return (DWORD)(pQueue->m_pShareBufferHead->dwBufferLength - sizeof(SHARE_BUFFER_HEAD)- dwUsed);
}



///////////////////////////////////////////////////////////
//
//	返回队列里面已存在的数据包个数
//
//	入口:	
//		pQueue - 队列结构指针
//
//  返回值:	剩余的数据包个数
DWORD QueueQueryPacketCount(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);

	ASSERT(pQueue->m_pShareBufferHead->iWritePacketCount>=pQueue->m_pShareBufferHead->iReadPacketCount);

	return (DWORD)(pQueue->m_pShareBufferHead->iWritePacketCount
		- pQueue->m_pShareBufferHead->iReadPacketCount);
}


//临时操作
int QueueReadCurrentPacket_Tmp(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer)
{
	PCOMQUEUE_PACKET_HEADER	pPacketHeader = NULL;

	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	
	if( pQueue->m_pShareBufferHead->iWritePacketCount <= pQueue->m_pShareBufferHead->iTmpReadPacketCount )
	{
		return PACKET_TYPE_INVALID;
	}
		
	if (pQueue->m_pShareBufferHead->pWritePosition == pQueue->m_pShareBufferHead->pTmpReadPosition)
	{
		return PACKET_TYPE_INVALID;
	}

	pPacketHeader = (PCOMQUEUE_PACKET_HEADER)((char*)pQueue->m_pShareBufferHead + pQueue->m_pShareBufferHead->pTmpReadPosition);

	// 表明是无效包，即结尾的一小部分
	if ((DWORD)PACKET_TYPE_PADDING == pPacketHeader->dwOrgSize)
	{
		// 回绕，重新读，这个填充包不算在任何计数内
		_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pTmpReadPosition);
		return QueueReadCurrentPacket_Tmp(pQueue, pPacketPointer);
	}
	else
	{
		DWORD dwAdjSize = 0;

		// 检查数据的合法性
		dwAdjSize = INT_PTR_ALIGN(pPacketHeader->dwOrgSize + sizeof(COMQUEUE_PACKET_HEADER));

		if (pPacketHeader->dwAdjSize != dwAdjSize)
		{
			SHARE_BUFFER_HEAD* pShareBufferHead = pQueue->m_pShareBufferHead;

			pShareBufferHead->iTmpReadPacketCount = pShareBufferHead->iWritePacketCount;
			pShareBufferHead->pTmpReadPosition = pShareBufferHead->pWritePosition;

			pShareBufferHead->iReadPacketCount = pShareBufferHead->iWritePacketCount;
			pShareBufferHead->pReadPosition = pShareBufferHead->pWritePosition;

			return PACKET_TYPE_PKT_ERROR;
		}

		*pPacketPointer = (PUCHAR)((PUCHAR)pPacketHeader + (ULONG_PTR)sizeof(COMQUEUE_PACKET_HEADER));

		return pPacketHeader->dwOrgSize;
	}
}

VOID  QueueSkipCurrentPacket_Tmp(IN TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	UINT_PTR dwReadPos; 

	PCOMQUEUE_PACKET_HEADER	pPacketHeader = NULL;
	
	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	
	pPacketHeader = (PCOMQUEUE_PACKET_HEADER)((PUCHAR)pQueue->m_pShareBufferHead + pQueue->m_pShareBufferHead->pTmpReadPosition);

	// 表明是无效包，即结尾的一小部分
	if ((DWORD)PACKET_TYPE_PADDING == pPacketHeader->dwOrgSize)
	{
		ASSERT(FALSE);

		// 回绕，重新读，这个填充包不算在任何计数内
		_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pTmpReadPosition);

		//切记，切记，
		//注意这里不能调用QueueSkipCurrentPacket_Tmp(pQueue);
		//切记，切记，

		return;
		
	}
	else
	{
		dwReadPos = pQueue->m_pShareBufferHead->pTmpReadPosition + (UINT_PTR)pPacketHeader->dwAdjSize;

		// 应该不会在skip时出现到缓冲区尾部的情况
		// 因为结尾的填充包都会在Read函数中被跳过去了
		if (dwReadPos >= pQueue->m_pShareBufferHead->dwBufferLength)
		{
			ASSERT(FALSE);
			_DEF_RESET_RW_POINTER(dwReadPos);
		}

		pQueue->m_pShareBufferHead->pTmpReadPosition = dwReadPos;

		{
			++pQueue->m_pShareBufferHead->iTmpReadPacketCount;

			ASSERT(pQueue->m_pShareBufferHead->iWritePacketCount>=pQueue->m_pShareBufferHead->iTmpReadPacketCount);
		}
	}


	return;

}

int QueueReadCurrentPacket_Tmp_Ex(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer,OUT UINT_PTR * index)
{
	if( pQueue->cmq_tmp_ex.mgr_table )
	{
		int pkt_len = 0;

		if( (pQueue->cmq_tmp_ex.head == pQueue->cmq_tmp_ex.cur+1) || 
			(pQueue->cmq_tmp_ex.head == 0 && (pQueue->cmq_tmp_ex.cur == pQueue->cmq_tmp_ex.table_size-1))
			)
		{//满了，不能缓存读的数据位置

			return -1;
		}

		pkt_len = QueueReadCurrentPacket_Tmp(pQueue,pPacketPointer);

		if( pkt_len == 0 )
		{
			return 0;
		}

		*index = pQueue->cmq_tmp_ex.cur;

		{//使用临时变量，主要是为了保障对cur操作的原子性
			UINT_PTR temp_cur = pQueue->cmq_tmp_ex.cur;

			pQueue->cmq_tmp_ex.mgr_table[temp_cur] = 0;

			++temp_cur;

			if( temp_cur == pQueue->cmq_tmp_ex.table_size )
			{
				temp_cur = 0;
			}

			pQueue->cmq_tmp_ex.cur = temp_cur;
		}

		return pkt_len;
	}
	else
	{
		return QueueReadCurrentPacket_Tmp(pQueue,pPacketPointer);
	}
	
	return 0;

}

VOID  QueueSkipCurrentPacket_Ex(IN TAG_COMMOM_QUEUE_MANAGER *pQueue,UINT_PTR index)
{	
	if( pQueue->cmq_tmp_ex.mgr_table )
	{
		pQueue->cmq_tmp_ex.mgr_table[index] = 1;

		if( index == pQueue->cmq_tmp_ex.head )
		{//使用临时变量，主要是为了保障对cur操作的原子性

			UINT_PTR temp_head = pQueue->cmq_tmp_ex.head;

			while( temp_head != pQueue->cmq_tmp_ex.cur )
			{
				if( pQueue->cmq_tmp_ex.mgr_table[temp_head] == 1 )
				{
					QueueSkipCurrentPacket(pQueue);

					pQueue->cmq_tmp_ex.mgr_table[temp_head] = 0;
					
					++temp_head;
					
					if( temp_head == pQueue->cmq_tmp_ex.table_size )
					{
						temp_head = 0;
					}
				}
				else
				{
					break;
				}
			}

			pQueue->cmq_tmp_ex.head = temp_head;
		}
	}
	else
	{
		QueueSkipCurrentPacket(pQueue);
	}

	return;
}


int QueueQueryPacketCount_Tmp(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);

	ASSERT(pQueue->m_pShareBufferHead->iWritePacketCount>=pQueue->m_pShareBufferHead->iTmpReadPacketCount);

	return (int)(pQueue->m_pShareBufferHead->iWritePacketCount
		- pQueue->m_pShareBufferHead->iTmpReadPacketCount);
}

int QueueQuerySendedButNotCompletedPkts(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	if( pQueue->cmq_tmp_ex.table_size )
	{
		if( pQueue->cmq_tmp_ex.cur >= pQueue->cmq_tmp_ex.head )
		{
			return (int)(pQueue->cmq_tmp_ex.cur-pQueue->cmq_tmp_ex.head);
		}
		else
		{
			return (int)(pQueue->cmq_tmp_ex.table_size - pQueue->cmq_tmp_ex.head + pQueue->cmq_tmp_ex.cur);
		}
	}
	else
	{
		return 0;
	}
}
