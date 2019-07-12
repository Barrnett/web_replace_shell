//////////////////////////////////////////////////////////////////////////
//CPTWriteDataStream.cpp

#include "PacketIO/CPTWriteDataStream.h"
#include "./zcdata/CapSave.h"

void RECV_PACKET_T0_PACKET_CONTEXT(const RECV_PACKET& packet, PACKET_CONTEXT& pktContext)
{
	pktContext.dwLengthPacket = packet.nPktlen;
	pktContext.dwLengthPacket = packet.nCaplen;

	pktContext.pPacket = (BYTE *)packet.pPacket;
	pktContext.dwOffsetBit = packet.nOffsetBit;
	pktContext.dwAddiBitLen = packet.nPktAddiBitLen;
	
	pktContext.pHeaderInfo->wdProto = packet.pHeaderinfo->wdProto;

	pktContext.pHeaderInfo->btHardwareType = packet.pHeaderinfo->btHardwareType;
	pktContext.pHeaderInfo->btCardNo = packet.pHeaderinfo->btCardNo;
	pktContext.pHeaderInfo->btInterfaceNo = packet.pHeaderinfo->btInterfaceNo;

	pktContext.pHeaderInfo->btFlag.Tx = 0;
	pktContext.pHeaderInfo->btFlag.Rx = 0;

	pktContext.pHeaderInfo->stamp = packet.pHeaderinfo->stamp;
	pktContext.pHeaderInfo->dwError = packet.pHeaderinfo->dwError;

}

CCPTWriteDataStream::CCPTWriteDataStream(void)
{
	m_dwMaxFileLengthMb = 0;
	m_dwBufSize = 0;
	m_nMultiFiles = 1;
	m_ncaptime = 0;

	m_pCapSave = NULL;

	HEADER_INFO * pTmpHeaderInfo = new HEADER_INFO;
	memset(pTmpHeaderInfo,0x00,sizeof(*pTmpHeaderInfo));
	pTmpHeaderInfo->btHeaderLength = sizeof(HEADER_INFO);

	m_pHeaderInfo = (void *)pTmpHeaderInfo;
}

CCPTWriteDataStream::~CCPTWriteDataStream(void)
{
	CloseDataStream();

	delete ((HEADER_INFO *)m_pHeaderInfo);
	m_pHeaderInfo = NULL;
}

BOOL CCPTWriteDataStream::init(LPCSTR szFileName,
									DWORD dwMaxFileLengthMb,
									DWORD dwBufSize,
									int multifiles,
									unsigned int ncaptime)
{
	SetFileName(szFileName);
	m_dwMaxFileLengthMb = dwMaxFileLengthMb;
	m_dwBufSize = dwBufSize;
	m_nMultiFiles = multifiles;
	m_ncaptime = ncaptime;

	return true;
}

void CCPTWriteDataStream::fini()
{
	return;
}

//是否支持写控制．
BOOL CCPTWriteDataStream::IsSupportWriteCtrl() const
{
	return true;
}

IFileWriteDataStream * CCPTWriteDataStream::CloneAfterIni()
{
	CCPTWriteDataStream * pCPTWriteDataStream = new CCPTWriteDataStream;

	if( pCPTWriteDataStream )
	{
		if( !pCPTWriteDataStream->init(m_strFileName.c_str(),m_dwMaxFileLengthMb,
				m_dwBufSize,m_nMultiFiles,m_ncaptime) )
		{
			delete pCPTWriteDataStream;
			return NULL;
		}

		return pCPTWriteDataStream;

	}

	return NULL;
}

//相当于init+OpenDataStream
BOOL CCPTWriteDataStream::Open(LPCSTR szFileName,
							   DWORD dwMaxFileLengthMb,
							   DWORD dwBufSize,
							   int multifiles,
							   unsigned int ncaptime)
{
	if( !init(szFileName,dwMaxFileLengthMb,dwBufSize,multifiles,ncaptime) )
		return false;

	return (OpenDataStream()==0);
}

void CCPTWriteDataStream::Close()
{
	CloseDataStream();
	fini();
	return;
}

int CCPTWriteDataStream::OpenDataStream()
{
	CloseDataStream();

	ACE_ASSERT(!m_strFileName.empty());

	if( m_strFileName.empty() )
	{
		return -1;
	}

	m_pCapSave = new CCapSave;
	
	if( m_pCapSave->Init(
		GetFileName(),m_dwMaxFileLengthMb,
		m_dwBufSize,m_nMultiFiles,m_ncaptime) )
	{
		return 0;
	}

	delete m_pCapSave;
	m_pCapSave = NULL;

	return -1;

}

void CCPTWriteDataStream::CloseDataStream()
{
	if( m_pCapSave )
	{
		m_pCapSave->CloseAll();
		delete m_pCapSave;
		m_pCapSave = NULL;
	}
}


int CCPTWriteDataStream::WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums)
{
	if( type == PACKET_OK )
	{
		PACKET_CONTEXT pktContext;

		pktContext.pHeaderInfo = (HEADER_INFO *)m_pHeaderInfo;

		RECV_PACKET_T0_PACKET_CONTEXT(packet,pktContext);

		return m_pCapSave->RecordPacket(pktContext.pPacket,
			pktContext.dwLengthPacket,
			pktContext.pHeaderInfo);
	}

	return -1;
}

//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
int CCPTWriteDataStream::GetConnectState()
{
	if( m_pCapSave )
	{
		return 1;
	}

	return 0;
}


bool CCPTWriteDataStream::GetDataStreamName(std::string& name)
{
	if( m_strFileName.empty() )
	{
		name = "cpt file";
	}
	else
	{
		name = m_strFileName;
	}

	return true;
}
