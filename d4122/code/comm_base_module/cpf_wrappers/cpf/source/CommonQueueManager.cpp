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

//Ϊ��ʵ��4�ֽڶ��룬Ϊÿһ��������һ����ͷ
typedef struct _COMQUEUE__PACKET_HEADER_
{
	DWORD dwAdjSize;	//ÿһ����������Ĵ�С,����COMQUEUE_PACKET_HEADER���ͷ�Ĵ�С
	DWORD dwOrgSize;	//ÿһ������ԭʼ��С,PACKET_TYPE_PADDING ����������ǽ�β������

}COMQUEUE_PACKET_HEADER, *PCOMQUEUE_PACKET_HEADER;

#pragma pack(pop)



/*
	����˵����
		1����дָ�붼������ڻ�����ָ������ƫ�ƣ����Ǿ��Ե�ַ
		2����дָ���ʼ״̬����СֵΪsizeof(SHARE_BUFFER_HEAD)��������0
		3����дָ�����ֵӦС�ڻ��������ȣ������ܵ��ڴ˳���ֵ
		4��������д��������ʱ����Ҫ����һ���հ�ͷ��λ�á���˻�����������ʱ��дָ����Զ����׷�϶�ָ��
		5����ָ��=дָ��ʱ��������Ϊ��
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
//	�Ӷ��л��������ȡһ�����ݰ�,ָʾ���ݰ��׵�ַ,���������ݸ���
//
//	���:
//		pQueue - ���нṹָ��
//		pPacketPointer - �������ݰ����׵�ַ
//
//  ����:
//		not zero - ���ݱ��ĳ���
//		0 - û�ж������ݰ�
inline int Inner_QueueReadCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT PUCHAR *pPacketPointer)//, DWORD &dwAPacketLength);
{
	COMQUEUE_PACKET_HEADER*	pPacketHeader = NULL;

	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	
	// ������ʧЧ
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

	// ��������Ч��������β��һС����
	if ((DWORD)PACKET_TYPE_PADDING == pPacketHeader->dwOrgSize)
	{
		// ���ƣ����¶�����������������κμ�����
		_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pReadPosition);

		return Inner_QueueReadCurrentPacket(pQueue, pPacketPointer);
	}
	else
	{
		DWORD dwAdjSize = 0;

		// ������ݵĺϷ���
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
//	�Ӷ��л�������������ǰ���ݰ�,������ͷ��ָ�����
//
//	���:	
//		pQueue - ���нṹָ��
//
//  ����:	��
VOID  QueueSkipCurrentPacket(IN TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	UINT_PTR dwReadPos; 

	COMQUEUE_PACKET_HEADER*	pPacketHeader = NULL;
	
	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	
	pPacketHeader = (COMQUEUE_PACKET_HEADER*)( (UCHAR*)pQueue->m_pShareBufferHead + pQueue->m_pShareBufferHead->pReadPosition);

	// ��������Ч��������β��һС����
	if ((DWORD)PACKET_TYPE_PADDING == pPacketHeader->dwOrgSize)
	{
		// ���ƣ����¶�����������������κμ�����
		_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pReadPosition);

		QueueSkipCurrentPacket(pQueue);

		return;
	}
	else
	{
		dwReadPos = pQueue->m_pShareBufferHead->pReadPosition + (UINT_PTR)pPacketHeader->dwAdjSize;

		// Ӧ�ò�����skipʱ���ֵ�������β�������
		// ��Ϊ��β������������Read�����б�����ȥ��
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
//	�Ӷ��л�����������һ�黺�������������Ƿ���������Ҫ
//
//	���:	
//		pQueue - ���нṹָ��
//		dwWriteLength - ����Ļ���������
//
//  ����:	������ָ��
//		NULL -- ����ʧ��
PVOID	QueueWriteAllocateBuffer(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN DWORD dwWriteLength)	// ���뻺��������������ֵΪ��������ַ
{
	COMQUEUE_PACKET_HEADER*	pPacketHeader = NULL;
	DWORD	dwActualRequestLength = 0;
	UINT_PTR	dwTailSpace, dwHeadSpace;
	UINT_PTR	pWritePosition, pReadPosition;

	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);
	ASSERT(dwWriteLength);
	
	// ��¼����Ļ�������С
	pQueue->m_dwLastAllocateLength = dwWriteLength;

	if (0 == dwWriteLength)
	{
		return NULL;
	}

	// ������ʧЧ
	if ( !pQueue->m_pShareBufferHead->pWritePosition)
	{
		pQueue->m_dwLastAllocateLength = 0;
		return NULL;
	}


	// ����Ҫ�Ļ�������С32λ����,��Ҫ���ϵ�ǰ��ͷ��Ԥ����Ч��ͷ�ĳ���
	// ��ΪԤ����Ч���Ĵ��ڣ�����дָ�벻��׷�϶�ָ��
	dwActualRequestLength = INT_PTR_ALIGN(dwWriteLength + 2*sizeof(COMQUEUE_PACKET_HEADER));

	pWritePosition = pQueue->m_pShareBufferHead->pWritePosition;
	pReadPosition = pQueue->m_pShareBufferHead->pReadPosition;

	// дָ����ڶ�ָ��
	if (pWritePosition >= pReadPosition)
	{
		dwTailSpace = pQueue->m_pShareBufferHead->dwBufferLength - pWritePosition;
		// ʣ��ռ���������
		if (dwTailSpace > dwActualRequestLength)	// ����=�жϣ��ɷ�ָֹ��ָ��������β
		{
			//                      ָ��             +     ƫ��
			return (PVOID)((UCHAR*)pQueue->m_pShareBufferHead + pWritePosition + sizeof(COMQUEUE_PACKET_HEADER));
		}
		
		// �ж�ͷ��ʣ��ռ��Ƿ���������
		dwHeadSpace = pReadPosition - sizeof(SHARE_BUFFER_HEAD);
		if (dwHeadSpace >= dwActualRequestLength)	// ���������=�ж�
		{
			// ��β��ʣ�ಿ�����Ϊ��Ч��
			pPacketHeader = (PCOMQUEUE_PACKET_HEADER)((UCHAR*)pQueue->m_pShareBufferHead + pWritePosition);
			pPacketHeader->dwAdjSize = (DWORD)dwTailSpace;	// ���볤��Ϊʣ�����еĳ���
			pPacketHeader->dwOrgSize = (DWORD)PACKET_TYPE_PADDING;	// ԭʼ����ָ���������Ч

			// �޸�дָ�뵽������ͷ��
			_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pWritePosition);

			//                      ָ��             +     ƫ��
			return (PVOID)((UCHAR*)pQueue->m_pShareBufferHead + sizeof(SHARE_BUFFER_HEAD) + sizeof(COMQUEUE_PACKET_HEADER));
		}

	}
	// ��ָ�����дָ��,��ֻʣ���м�һ��ռ�
	else
	{
		dwTailSpace = pReadPosition - pWritePosition;
		if (dwTailSpace >= dwActualRequestLength) 	// ���������=�ж�
			return (PVOID)((UCHAR*)pQueue->m_pShareBufferHead + pWritePosition + sizeof(COMQUEUE_PACKET_HEADER));
	}

	// û���㹻�ռ�
	pQueue->m_dwLastAllocateLength = 0;
	return NULL;
}


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
BOOL QueueWriteDataFinished(IN TAG_COMMOM_QUEUE_MANAGER *pQueue, IN DWORD dwWriteLength)	//����д������ɣ�Ӧ���ƶ�����
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

	// �����ͷ
	pPacketHeader = (COMQUEUE_PACKET_HEADER *)((UCHAR*)pQueue->m_pShareBufferHead + pQueue->m_pShareBufferHead->pWritePosition);
	pPacketHeader->dwOrgSize = dwWriteLength;
	pPacketHeader->dwAdjSize = dwActualRequestLength;

	// ��������ͷ
	dwNewPosi = pQueue->m_pShareBufferHead->pWritePosition + dwActualRequestLength;
	// �����Ͻ����������>=���������Ϊÿ��allcate��ʱ����ǰԤ����һ��head�Ŀռ�
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
//	���ض���������ʹ���˵Ŀռ�
//
//	���:	
//		pQueue - ���нṹָ��
//		dwTotalLength - ���ػ������ܳ���
//
//  ����:	��ʹ���˵��ֽ���
DWORD	QueueQueryUsedBytes(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT DWORD *dwTotalLength)	// ��������ʹ���˵Ŀռ�
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
//	���ض���������δʹ�õĿռ�
//
//	���:	
//		pQueue - ���нṹָ��
//		dwTotalLength - ���ػ������ܳ���
//
//  ����:	δʹ�õ��ֽ���
DWORD	QueueQueryUnUsedBytes(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue, OUT DWORD *dwTotalLength)	// ��������ʹ���˵Ŀռ�
{
	DWORD dwUsed = QueueQueryUsedBytes(pQueue, dwTotalLength);
	
	return (DWORD)(pQueue->m_pShareBufferHead->dwBufferLength - sizeof(SHARE_BUFFER_HEAD)- dwUsed);
}



///////////////////////////////////////////////////////////
//
//	���ض��������Ѵ��ڵ����ݰ�����
//
//	���:	
//		pQueue - ���нṹָ��
//
//  ����ֵ:	ʣ������ݰ�����
DWORD QueueQueryPacketCount(IN const TAG_COMMOM_QUEUE_MANAGER *pQueue)
{
	ASSERT(pQueue);
	ASSERT(pQueue->m_pShareBufferHead);

	ASSERT(pQueue->m_pShareBufferHead->iWritePacketCount>=pQueue->m_pShareBufferHead->iReadPacketCount);

	return (DWORD)(pQueue->m_pShareBufferHead->iWritePacketCount
		- pQueue->m_pShareBufferHead->iReadPacketCount);
}


//��ʱ����
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

	// ��������Ч��������β��һС����
	if ((DWORD)PACKET_TYPE_PADDING == pPacketHeader->dwOrgSize)
	{
		// ���ƣ����¶�����������������κμ�����
		_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pTmpReadPosition);
		return QueueReadCurrentPacket_Tmp(pQueue, pPacketPointer);
	}
	else
	{
		DWORD dwAdjSize = 0;

		// ������ݵĺϷ���
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

	// ��������Ч��������β��һС����
	if ((DWORD)PACKET_TYPE_PADDING == pPacketHeader->dwOrgSize)
	{
		ASSERT(FALSE);

		// ���ƣ����¶�����������������κμ�����
		_DEF_RESET_RW_POINTER(pQueue->m_pShareBufferHead->pTmpReadPosition);

		//�мǣ��мǣ�
		//ע�����ﲻ�ܵ���QueueSkipCurrentPacket_Tmp(pQueue);
		//�мǣ��мǣ�

		return;
		
	}
	else
	{
		dwReadPos = pQueue->m_pShareBufferHead->pTmpReadPosition + (UINT_PTR)pPacketHeader->dwAdjSize;

		// Ӧ�ò�����skipʱ���ֵ�������β�������
		// ��Ϊ��β������������Read�����б�����ȥ��
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
		{//���ˣ����ܻ����������λ��

			return -1;
		}

		pkt_len = QueueReadCurrentPacket_Tmp(pQueue,pPacketPointer);

		if( pkt_len == 0 )
		{
			return 0;
		}

		*index = pQueue->cmq_tmp_ex.cur;

		{//ʹ����ʱ��������Ҫ��Ϊ�˱��϶�cur������ԭ����
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
		{//ʹ����ʱ��������Ҫ��Ϊ�˱��϶�cur������ԭ����

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
