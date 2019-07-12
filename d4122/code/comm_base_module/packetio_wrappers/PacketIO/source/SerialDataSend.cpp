//////////////////////////////////////////////////////////////////////////
//SerialDataSend.cpp

#include "PacketIO/SerialDataSend.h"
#include "cpf/crc_tools.h"

CSerialSendTask::CSerialSendTask()
{
	m_bUserStop = TRUE;
	
	m_nSerialIndex = 0;
	m_nSerialFd = -1;
	m_pSendBuf = NULL;
	memset(&m_tagSendQueue, 0, sizeof(TAG_COMMOM_QUEUE_MANAGER));
	sem_init(&m_sem, 0, 0);

	memset(m_acPacketBuf, 0, 1600);
	m_nPacketLen = 0;
}

CSerialSendTask::~CSerialSendTask()
{
	if (m_pSendBuf)
	{
		QueueUnInitialize(&m_tagSendQueue);
		free(m_pSendBuf);

		m_pSendBuf = NULL;
	}

	m_bUserStop = TRUE;
	sem_destroy(&m_sem);
}

int CSerialSendTask::f_CreateQueue(ACE_UINT32 nBufferSize)
{
	m_pSendBuf = (unsigned char*)malloc(nBufferSize);
	if (NULL == m_pSendBuf)
	{
		return -1;
	}

	memset(m_pSendBuf, 0, nBufferSize);

	QueueInitialize_new(&m_tagSendQueue, m_pSendBuf, nBufferSize, 0);

	return 0;
}

int CSerialSendTask::svc()
{
	unsigned char* pPacket = NULL;
	int iPacketLen;
	int i;
	unsigned short usChecksum;

	int iLeftDataLen;
	int iSentDataLen;
	
	//帧定界符
	m_acPacketBuf[0] = 0x7E;
			
	while(!m_bUserStop)
	{
		sem_wait(&m_sem);

		iPacketLen = QueueReadNextPacket(&m_tagSendQueue, &pPacket);
		if (0 < iPacketLen)
		{
			m_nPacketLen = 1;
			
			//数据包
			for (i=0; i<iPacketLen; i++)
			{
				if ((0x7D == pPacket[i]) || (0x7E == pPacket[i]))
				{
					m_acPacketBuf[m_nPacketLen++] = 0x7D;
					m_acPacketBuf[m_nPacketLen++] = 0x7D ^ pPacket[i];
				}
				else
				{
					m_acPacketBuf[m_nPacketLen++] = pPacket[i];
				}
			}

			//校验码
			usChecksum = CPF::GetFCS16(pPacket, iPacketLen);
			m_acPacketBuf[m_nPacketLen] = (unsigned char)(usChecksum >> 8);
			if (0x7D == m_acPacketBuf[m_nPacketLen])
			{
				m_nPacketLen++;
				m_acPacketBuf[m_nPacketLen++] = 0x7D ^ 0x7D;
			}
			else if (0x7E == m_acPacketBuf[m_nPacketLen])
			{
				m_acPacketBuf[m_nPacketLen++] = 0x7D;
				m_acPacketBuf[m_nPacketLen++] = 0x7D ^ 0x7E;
			}
			else
			{
				m_nPacketLen++;
			}
			m_acPacketBuf[m_nPacketLen] = (unsigned char)(usChecksum);
			if (0x7D == m_acPacketBuf[m_nPacketLen])
			{
				m_nPacketLen++;
				m_acPacketBuf[m_nPacketLen++] = 0x7D ^ 0x7D;
			}
			else if (0x7E == m_acPacketBuf[m_nPacketLen])
			{
				m_acPacketBuf[m_nPacketLen++] = 0x7D;
				m_acPacketBuf[m_nPacketLen++] = 0x7D ^ 0x7E;
			}
			else
			{
				m_nPacketLen++;
			}

			//帧定界符
			m_acPacketBuf[m_nPacketLen++] = 0x7E;

			//发送
			iLeftDataLen = m_nPacketLen;
			iSentDataLen = 0;
			while (0 < iLeftDataLen)
			{
				iSentDataLen = write(m_nSerialFd, m_acPacketBuf+iSentDataLen, iLeftDataLen);
				if (0 > iSentDataLen)
				{
					break;//发送失败，包丢失
				}
				else
				{
					iLeftDataLen -= iSentDataLen;
				}
			}
		}
	}
	
	return 0;
}

CSerialDataSend::CSerialDataSend()
{
}

CSerialDataSend::~CSerialDataSend()
{
}

int CSerialDataSend::f_Init()
{
	return 0;
}

int CSerialDataSend::f_AddSerialIndex(ACE_UINT8 nIndex, int nFd)
{
	CSerialSendTask* piSerialSendTask = new CSerialSendTask;
	if (NULL == piSerialSendTask)
	{
		return -1;
	}

	int ret = piSerialSendTask->f_CreateQueue(64000);
	if (0 > ret)
	{
		return -2;
	}
	piSerialSendTask->m_nSerialIndex = nIndex;
	piSerialSendTask->m_nSerialFd = nFd;

	m_vtSerialSendTask.push_back(piSerialSendTask);

	return 0;
}

int CSerialDataSend::f_DeleteSerialIndex(ACE_UINT8 nIndex)
{
	m_vtSerialSendTask[nIndex-1]->m_bUserStop = TRUE;

	return 0;
}

void CSerialDataSend::f_StartSend()
{
	size_t max = m_vtSerialSendTask.size();
	for (size_t i=0; i<max; i++)
	{
		m_vtSerialSendTask[i]->m_bUserStop = FALSE;
		m_vtSerialSendTask[i]->activate();
	}
}

void CSerialDataSend::f_StopSend()
{
	size_t max = m_vtSerialSendTask.size();
	for (size_t i=0; i<max; i++)
	{
		m_vtSerialSendTask[i]->m_bUserStop = TRUE;
	}
}

int CSerialDataSend::f_SendPacket(unsigned char * pPacket, unsigned int nPacketlen, ACE_UINT8 nIndex)
{
	TAG_COMMOM_QUEUE_MANAGER* pQueue = &(m_vtSerialSendTask[nIndex-1]->m_tagSendQueue);
	
	void* pBuf = QueueWriteAllocateBuffer(pQueue, nPacketlen);
	if (NULL == pBuf)
	{
		return -1;
	}
	
	memcpy(pBuf, pPacket, nPacketlen);
	QueueWriteDataFinished(pQueue, nPacketlen);

	sem_post(&(m_vtSerialSendTask[nIndex-1]->m_sem));

	return 0;
}
