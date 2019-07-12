//////////////////////////////////////////////////////////////////////////
//SerialDataRecv.cpp

#include "cpf/ostypedef.h"


#include "PacketIO/SerialDataRecv.h"
#include "cpf/path_tools.h"
#include "cpf/Common_Func_Macor.h"
#include "cpf/crc_tools.h"

CSerialRecvTask::CSerialRecvTask()
{
	m_bUserStop = TRUE;
	
	m_nSerialIndex = 0;
	m_nSerialFd = -1;
	m_pmutex = NULL;
	m_pRecvQueue = NULL;

	m_eRecvStat = RECV_IDLE;
	memset(m_acPacketBuf, 0 , 1600);
	//m_nPacketLen = 0;
}

CSerialRecvTask::~CSerialRecvTask()
{
	pthread_mutex_unlock(m_pmutex);

	close();
}

// 转义定义如下:
//     0x7E  --> 帧头和帧尾，接收缓存不保存帧定界符
//     转义: 7E ---> 7D,(7D ^ 7E)
//     转义: 7D ---> 7D,(7D ^ 7D)
int CSerialRecvTask::svc()
{
	int nReadLen = 0;
	void* pQueueBuf = NULL;
	unsigned char* pRecvData = NULL;
	unsigned int nPacketLen = 0;
	unsigned char acBuf[2] = {0};
	unsigned short usChecksum = 0;	

	((SERIAL_RECV_HEAD_S*)m_acPacketBuf)->nSerialFd = m_nSerialFd;
	((SERIAL_RECV_HEAD_S*)m_acPacketBuf)->nSerialIndex = m_nSerialIndex;
	pRecvData = m_acPacketBuf + sizeof(SERIAL_RECV_HEAD_S);
	
 	while(!m_bUserStop)
	{
		nReadLen = read(m_nSerialFd, acBuf, 1);
		if (0 >= nReadLen)
		{ 
			continue; 
		}

		switch (m_eRecvStat)
		{
			case RECV_IDLE:
				if (0x7E == acBuf[0])
				{
					nPacketLen = 0;
					m_eRecvStat = RECV_DATA;
				}
				break;

			case RECV_DATA:
				if (0x7D == acBuf[0])
				{
					m_eRecvStat = RECV_7D;
				}
				else if (0x7E == acBuf[0])
				{
					if (2 < nPacketLen)
					{
						nPacketLen -= 2;
						usChecksum = CPF::GetFCS16(pRecvData, nPacketLen);
						if (((unsigned char)(usChecksum>>8) == pRecvData[nPacketLen]) && ((unsigned char)(usChecksum) == pRecvData[nPacketLen+1]))
						{
							nPacketLen += sizeof(SERIAL_RECV_HEAD_S);
							pthread_mutex_lock(m_pmutex);
								pQueueBuf = QueueWriteAllocateBuffer(m_pRecvQueue, nPacketLen);
								if (NULL != pQueueBuf)
								{
									
									memcpy(pQueueBuf, m_acPacketBuf, nPacketLen);
									QueueWriteDataFinished(m_pRecvQueue, nPacketLen);
								}
							pthread_mutex_unlock(m_pmutex);
						}
					}

					m_eRecvStat = RECV_IDLE;
				}
				else
				{
					pRecvData[nPacketLen++] = acBuf[0];
				}
				break;

			case RECV_7D:
				if ((0x7D == acBuf[0]) || (0x7E == acBuf[0]))
				{
					m_eRecvStat = RECV_IDLE;
				}
				else
				{
					pRecvData[nPacketLen++] = 0x7D ^ acBuf[0];
					m_eRecvStat = RECV_DATA;
				}
				break;

			default:
				break;
		}
	}

	return 0;
}

CSerialDataRecv::CSerialDataRecv()
{
	m_pRecvBuf = NULL;
	memset(&m_tagRecvQueue, 0, sizeof(TAG_COMMOM_QUEUE_MANAGER));
	memset(&m_tagPacketHead, 0, sizeof(PACKET_HEADER));
	pthread_mutex_init(&m_mutex, NULL);
	m_vtSerialRecvTask.clear();
}

CSerialDataRecv::~CSerialDataRecv()
{
	size_t max = m_vtSerialRecvTask.size();
	for (size_t i=0; i<max; i++)
	{
		delete m_vtSerialRecvTask[i];
	}
	m_vtSerialRecvTask.clear();
	
	if (m_pRecvBuf)
	{
		QueueUnInitialize(&m_tagRecvQueue);
		free(m_pRecvBuf);
		
		m_pRecvBuf = NULL;
	}

	pthread_mutex_unlock(&m_mutex);
	pthread_mutex_destroy(&m_mutex);
}


int CSerialDataRecv::f_Init()
{
	m_pRecvBuf = (unsigned char *)malloc(64000);
	if (NULL == m_pRecvBuf)
	{
		return -1;
	}

	memset(m_pRecvBuf, 0, 64000);

	QueueInitialize_new(&m_tagRecvQueue, m_pRecvBuf, 64000, 0);
	
	return 0;
}

int CSerialDataRecv::f_AddSerialIndex(ACE_UINT8 nIndex, int nFd)
{
	CSerialRecvTask* piSerialRecvTask = new CSerialRecvTask;
	if (NULL == piSerialRecvTask)
	{
		return -1;
	}

	piSerialRecvTask->m_nSerialIndex = nIndex;
	piSerialRecvTask->m_nSerialFd = nFd;
	piSerialRecvTask->m_pmutex = &m_mutex;
	piSerialRecvTask->m_pRecvQueue = &m_tagRecvQueue;

	m_vtSerialRecvTask.push_back(piSerialRecvTask);

	return 0;
}

int CSerialDataRecv::f_DeleteSerialIndex(ACE_UINT8 nIndex)
{
	m_vtSerialRecvTask[nIndex-1]->m_bUserStop = TRUE;

	return 0;
}

void CSerialDataRecv::f_StartRecv()
{
	size_t max = m_vtSerialRecvTask.size();
	for (size_t i=0; i<max; i++)
	{
		m_vtSerialRecvTask[i]->m_bUserStop = FALSE;
		m_vtSerialRecvTask[i]->activate();
	}
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CSerialDataRecv::f_StopRecv()
{
	size_t max = m_vtSerialRecvTask.size();
	for (size_t i=0; i<max; i++)
	{
		m_vtSerialRecvTask[i]->m_bUserStop = TRUE;
		//m_vtSerialRecvTask[i]->wait();
	}

	QueueClearQueue(&m_tagRecvQueue);
}

//接收数据
PACKET_LEN_TYPE CSerialDataRecv::f_GetPacket(RECV_PACKET& packet)
{
	unsigned char* pPacket = NULL;
	
	int iPacketLen = QueueReadNextPacket(&m_tagRecvQueue, &pPacket);
	if (0 < iPacketLen)
	{
		m_tagPacketHead.btCardNo = ((SERIAL_RECV_HEAD_S*)pPacket)->nSerialIndex;
		m_tagPacketHead.stamp.from_ace_timevalue(ACE_OS::gettimeofday());
		packet.pHeaderinfo = &m_tagPacketHead;
		
		packet.pPacket = pPacket + sizeof(SERIAL_RECV_HEAD_S);
		packet.nPktlen = iPacketLen - sizeof(SERIAL_RECV_HEAD_S);

		return PACKET_SERIAL_OK;
	}
	else
	{
		return PACKET_SERIAL_EMPTY;
	}
}


