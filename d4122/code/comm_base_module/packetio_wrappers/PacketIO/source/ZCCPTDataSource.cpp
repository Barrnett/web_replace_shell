//////////////////////////////////////////////////////////////////////////
//ZCCPTDataSource.cpp

#include "PacketIO/ZCCPTDataSource.h"
#include "zcdata/CapFileHead.h"
#include "zcdata/CapDataRead.h"

void PACKET_CONTEXT_T0_RECV_PACKET(const PACKET_CONTEXT& pktContext, RECV_PACKET& packet)
{
	packet.nPktlen = pktContext.dwLengthPacket;
	packet.nCaplen = pktContext.dwLengthPacket;

	packet.pPacket = (void *)pktContext.pPacket;
	packet.nOffsetBit = pktContext.dwOffsetBit;
	packet.nPktAddiBitLen = pktContext.dwAddiBitLen;
	packet.nCapAddiBitLen = 0;

	packet.pHeaderinfo->wdProto =  pktContext.pHeaderInfo->wdProto;
	packet.pHeaderinfo->btHardwareType =  pktContext.pHeaderInfo->btHardwareType;
	packet.pHeaderinfo->btCardNo =  pktContext.pHeaderInfo->btCardNo;	
	packet.pHeaderinfo->btInterfaceNo = pktContext.pHeaderInfo->btInterfaceNo;

	packet.pHeaderinfo->nFlag = 0;

	packet.pHeaderinfo->stamp = pktContext.pHeaderInfo->stamp;
	packet.pHeaderinfo->dwError = pktContext.pHeaderInfo->dwError;

}

CZCCPTDataSource::CZCCPTDataSource(void)
{
	m_pCapDataRead = NULL;

	memset(&m_headerInfo,0x00,sizeof(m_headerInfo));
	m_headerInfo.btHeaderLength = sizeof(m_headerInfo);

	m_headerInfo.btCardNo = 1;
	m_headerInfo.btHardwareType = 0;
	m_headerInfo.btInterfaceNo = 0;
	m_headerInfo.btVersion = 1;
	m_headerInfo.nFlag = 0;
	m_headerInfo.dwError = 0;
	m_headerInfo.stamp.sec = 0;
	m_headerInfo.stamp.ns = 0;

	m_nReadNums = 0;
	m_nBufferSizeMB = 4;

}

CZCCPTDataSource::~CZCCPTDataSource(void)
{
}

int CZCCPTDataSource::OpenDataSource()
{
	CloseDataSource();

	m_pCapDataRead = new CCapDataRead;

	if( m_pCapDataRead->Init(m_strFileName.c_str(),m_nReadNums,m_nBufferSizeMB*1024*1024) == TRUE )
		return 0;

	delete m_pCapDataRead;
	m_pCapDataRead = NULL;

	m_lastPktStamp = CTimeStamp32::zero;

	return -1;
}


void CZCCPTDataSource::CloseDataSource()
{
	if( m_pCapDataRead )
	{
		m_pCapDataRead->CloseAll();
		delete m_pCapDataRead;
		m_pCapDataRead = NULL;
	}
}

//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
BOOL CZCCPTDataSource::StartToRecv()
{
	m_lastPktStamp = CTimeStamp32::zero;

	return true;
}

//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
void CZCCPTDataSource::StopRecving()
{

}


void CZCCPTDataSource::GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops)
{
	nloops = 1;

	if( m_pCapDataRead )
	{
		pkts = m_pCapDataRead->GetCurTotalFrames();
		bytes = m_pCapDataRead->GetCurTotalSize();
	}
	else
	{
		pkts = bytes = -1;
	}
}

bool CZCCPTDataSource::GetDataSourceName(std::string& name)
{
	if( m_pCapDataRead )
	{
		if( m_pCapDataRead->GetCurFile() )
		{
			CStr strFullName;
			m_pCapDataRead->MakeFullPathName(m_pCapDataRead->GetCurFile(),strFullName);

			std::string otherinfo = GetFileOtherInfo(strFullName.GetString(),&m_lastPktStamp.m_ts);

			name = m_pCapDataRead->GetCurFile();

			name += otherinfo ;

			return true;
		}
		else
		{
			name = m_strFileName;

			return true;
		}
	}

	return false;
}

//��������
PACKET_LEN_TYPE CZCCPTDataSource::GetPacket(RECV_PACKET& packet)
{
	PACKET_CONTEXT pktContext;

	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��

	int nrtn = m_pCapDataRead->GetNextPacket(pktContext);

	if( 1 == nrtn )
	{
		packet.pHeaderinfo = &m_headerInfo;
		PACKET_CONTEXT_T0_RECV_PACKET(pktContext,packet);

		if( CTimeStamp32(m_headerInfo.stamp) < m_lastPktStamp )
		{//���ʱ��С����һ������ʱ�䣬������ǰ����ʱ��
			m_headerInfo.stamp = m_lastPktStamp.m_ts;
		}
		else
		{
			m_lastPktStamp.m_ts = m_headerInfo.stamp;
		}

		AdjustToCurSystime(PACKET_OK,packet);

		return PACKET_OK;

	}
	else if( 0 == nrtn )
	{
		return PACKET_DRV_ERROR;
	}
	else if( -1 == nrtn )
	{

		return PACKET_NO_MORE;
	}
	else
	{
		ACE_ASSERT(false);
		return PACKET_DRV_ERROR;
	}

}

//��������λ��ָ��ʼ�ĵط���
BOOL CZCCPTDataSource::SeekToBegin()
{
	m_lastPktStamp = CTimeStamp32::zero;

	return m_pCapDataRead->SetToIndexPacket(0);
}

//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
int CZCCPTDataSource::GetConnectState()
{
	if( m_pCapDataRead )
	{
		return 1;
	}

	return 0;

}

//////////////////////////////////////////////////////////////////////////
//CVPKZCCPTDataSource
//////////////////////////////////////////////////////////////////////////


CVPKZCCPTDataSource::CVPKZCCPTDataSource()
{
	m_pZCCPTDataSource = NULL;

}

CVPKZCCPTDataSource::~CVPKZCCPTDataSource()
{
	Close();
}

BOOL CVPKZCCPTDataSource::Open(const char *filename,BOOL bcur_systime)
{
	return false;

	//m_pZCCPTDataSource = new CZCCPTDataSource;

	//if( !m_pZCCPTDataSource )
	//	return false;

	//if( !m_pZCCPTDataSource->Open(filename,bcur_systime) )
	//{
	//	m_pZCCPTDataSource->CloseDataSource();
	//	delete m_pZCCPTDataSource;
	//	m_pZCCPTDataSource = NULL;

	//	return false;
	//}

	//CVPKHelpDataSource::init(m_pZCCPTDataSource,1);

	return true;
}

void CVPKZCCPTDataSource::Close()
{
	CloseDataSource();

	CVPKHelpDataSource::fini();

}

