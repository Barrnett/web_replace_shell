//////////////////////////////////////////////////////////////////////////
//WinpCapDataSource.cpp

#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS

#include "PacketIO/WinpCapDataSource.h"

#include "./winpcap/Packet32.h"
#include "PacketIO/MyProtocolID.h"

#include "adapterobj.h"
//#include "Packet.h"

#define PACKET_POINTER(p) ((PACKET *)p)

//////////////////////////////////////////////////////////////////////////
//CWinpCapDataSource
//////////////////////////////////////////////////////////////////////////

CWinpCapDataSource::CWinpCapDataSource()
{
	m_pAdapterObj = NULL;

	memset(&m_headerInfo,0x00,sizeof(m_headerInfo));

	m_headerInfo.btHeaderLength = sizeof(m_headerInfo);
	m_headerInfo.btVersion = 1;
	m_headerInfo.btCardNo = 1;
	m_headerInfo.btInterfaceNo = 1;
	m_headerInfo.wdProto = MY_PROTOCOL_MAC;

	m_pPacket = PacketAllocatePacket();

	//m_pPacket = new BYTE[sizeof(PACKET)];
	//memset(m_pPacket,0x00,sizeof(PACKET));

	memset(&m_winpcap_drv_param,0x00,sizeof(m_winpcap_drv_param));

	m_ullDroppedPkts = 0;
	m_ullDroppedBytes = 0;
}

CWinpCapDataSource::~CWinpCapDataSource()
{
	if( m_pAdapterObj )
	{
		delete m_pAdapterObj;
		m_pAdapterObj = NULL;
	}

	if( m_pPacket )
	{
		if( PACKET_POINTER(m_pPacket)->Buffer )
		{
			delete []PACKET_POINTER(m_pPacket)->Buffer;
			PACKET_POINTER(m_pPacket)->Buffer = NULL;
			PACKET_POINTER(m_pPacket)->Length = 0;
			PACKET_POINTER(m_pPacket)->bIoComplete = FALSE;
		}

		PacketFreePacket(PACKET_POINTER(m_pPacket));
		m_pPacket = NULL;
	}

	FreePacket();
}

BOOL CWinpCapDataSource::init(const WINPCAP_RecvDRV_PARAM& param)
{
	memcpy(&m_winpcap_drv_param,&param,sizeof(param));

	return true;
}


void CWinpCapDataSource::fini()
{

}

int CWinpCapDataSource::OpenDataSource()
{
	if( !m_pAdapterObj )
		return -1;

	if( !m_pAdapterObj->Open(m_winpcap_drv_param.cardIndex) )
		return -1;

	do 
	{
		if( 0 == m_winpcap_drv_param.filterMode )
		{
#define NDIS_PACKET_TYPE_PROMISCUOUS 0x0020
			if( !m_pAdapterObj->SetHwFilter(NDIS_PACKET_TYPE_PROMISCUOUS) )
				break;
		}
		else
		{
			if( !m_pAdapterObj->SetHwFilter(m_winpcap_drv_param.filterMode) )
				break;
		}


		if( !m_pAdapterObj->SetDriverBuff(m_winpcap_drv_param.driveBuffer*1024*1024) )
			break;

		if( PACKET_POINTER(m_pPacket)->Buffer )
		{
			delete []PACKET_POINTER(m_pPacket)->Buffer;
			PACKET_POINTER(m_pPacket)->Buffer = NULL;
			PACKET_POINTER(m_pPacket)->Length = 0;
			PACKET_POINTER(m_pPacket)->bIoComplete = FALSE;
		}

		UINT length = m_winpcap_drv_param.readBuffer*1024*1024;
		BYTE * pBuffer = new BYTE[length];

		if( !pBuffer )
			break;

		PacketInitPacket(PACKET_POINTER(m_pPacket), pBuffer, length);

		//PACKET_POINTER(m_pPacket)->Buffer = pBuffer;
		//PACKET_POINTER(m_pPacket)->Length = length;
		//PACKET_POINTER(m_pPacket)->bIoComplete = FALSE;

		if( 0 != m_winpcap_drv_param.minToCopy )
			m_pAdapterObj->SetMinToCopy(m_winpcap_drv_param.minToCopy);

		if( 0 != m_winpcap_drv_param.readTimeout )
			m_pAdapterObj->SetReadTimeout(m_winpcap_drv_param.readTimeout);

		return 0;

	} while(0);

	m_pAdapterObj->Close();

	return -1;
}
void CWinpCapDataSource::CloseDataSource()
{
	if( m_pAdapterObj )
	{
		m_pAdapterObj->Close();
	}
}

void CWinpCapDataSource::FreePacket()
{
	if( m_pPacket )
	{
		PACKET_POINTER(m_pPacket)->bIoComplete = FALSE;
	}
}

BOOL CWinpCapDataSource::StartToRecv()
{	
	m_ullDroppedPkts = 0;
	m_ullDroppedBytes = 0;

	return m_pAdapterObj->StartCapture();
}
//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CWinpCapDataSource::StopRecving()
{
	m_pAdapterObj->StopCapture();
}

BOOL CWinpCapDataSource::ReadDataFromHW()
{	
	PACKET_POINTER(m_pPacket)->bIoComplete = false;

	return (m_pAdapterObj->ReceivePacket(PACKET_POINTER(m_pPacket),TRUE) );
}


//得到因为上层处理来不及，底层丢包的数目
void CWinpCapDataSource::GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
{
	bpf_stat stat;
	
	if( m_pAdapterObj->GetStats(&stat) )
	{
		//m_ullDroppedPkts += stat.bs_drop;
		m_ullDroppedPkts = (ACE_UINT64)stat.bs_drop;

		pkts = m_ullDroppedPkts;
	}
	else
	{
		pkts = m_ullDroppedPkts;
	}

	bytes = -1;

	return;
}

int CWinpCapDataSource::GetConnectState()
{
	if( m_pAdapterObj )
	{
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//CSingleCardWinpCapDataSource
//////////////////////////////////////////////////////////////////////////

CSingleCardWinpCapDataSource::CSingleCardWinpCapDataSource()
{
	m_pAdapterObj = NULL;
	m_ulOff = 0;
	m_ulBytesReceived = 0;
}

CSingleCardWinpCapDataSource::~CSingleCardWinpCapDataSource()
{

}

BOOL CSingleCardWinpCapDataSource::init(const WINPCAP_RecvDRV_PARAM& param)
{
	m_ulOff = 0;
	m_ulBytesReceived = 0;

	m_pAdapterObj = new adapterobj(adapterobj::INPUTDATATYPE_NET);

	if( !m_pAdapterObj )
		return false;

	return CWinpCapDataSource::init(param);
}

void CSingleCardWinpCapDataSource::fini()
{
	CWinpCapDataSource::fini();
}

bool CSingleCardWinpCapDataSource::GetDataSourceName(std::string& name)
{
	char buf[256] = {0};

	sprintf(buf,"winpcap single cardindex = %d",m_winpcap_drv_param.cardIndex);

	name = buf;

	return true;
}

int CSingleCardWinpCapDataSource::SingleCard_ReadDataFromHW()
{
	if( !CWinpCapDataSource::ReadDataFromHW() )
		return -1;

	if( PACKET_POINTER(m_pPacket)->ulBytesReceived > PACKET_POINTER(m_pPacket)->Length )
	{
		FreePacket();
			
		ACE_ASSERT(FALSE);
		ACE_TRACE("驱动读包长度错误\r\n");

		return 0;
	}

	m_ulOff = 0;

	return PACKET_POINTER(m_pPacket)->ulBytesReceived;
}

PACKET_LEN_TYPE CSingleCardWinpCapDataSource::GetPacket(RECV_PACKET& packet)
{
	if( m_ulBytesReceived == 0 || m_ulOff >= m_ulBytesReceived )
	{
		if( m_ulOff >= m_ulBytesReceived )
		{
			FreePacket();	
		}

		m_ulBytesReceived = SingleCard_ReadDataFromHW();
	}

	if( (int)m_ulBytesReceived <= 0 )
	{
		return PACKET_EMPTY;	
	}

	bpf_hdr * hdr=(bpf_hdr *)((char *)PACKET_POINTER(m_pPacket)->Buffer+m_ulOff);

	if( hdr->bh_datalen > 1600 )
	{
		//ACE_TRACE("超长包 %d\r\n",hdr->bh_datalen);
		ACE_ASSERT(FALSE);	

		//要求重新读,
		m_ulBytesReceived = 0;
		m_ulOff = 0;

		return PACKET_CALL_NEXT;	
	}

	m_ulOff+=hdr->bh_hdrlen;
	packet.pPacket = (char *)PACKET_POINTER(m_pPacket)->Buffer + m_ulOff;
	m_ulOff=Packet_WORDALIGN(m_ulOff+hdr->bh_datalen);
	
	//防止包长度越界
	if(m_ulOff>m_ulBytesReceived)
	{				
		FreePacket();

		//ACE_TRACE("包长度越界 %d\r\n",hdr->bh_datalen);
		ACE_ASSERT(FALSE);
		//要求重新读,
		m_ulBytesReceived = 0;
		m_ulOff = 0;
		
		return PACKET_CALL_NEXT;	
	}

	if((hdr->bh_datalen<54)||(hdr->bh_datalen>1518+4))
	{	
		return PACKET_CALL_NEXT;	
	}

	packet.pHeaderinfo = &m_headerInfo;
	packet.nPktlen = hdr->bh_datalen+4;
	packet.pHeaderinfo->stamp.from_timeval(hdr->bh_tstamp);
	packet.nCaplen = packet.nPktlen;
	packet.nOffsetBit = 0;
	packet.nCapAddiBitLen = 0;
	packet.nPktAddiBitLen = 0;

	return 	PACKET_OK;
}


//////////////////////////////////////////////////////////////////////////
//CDoubleCardWinpCapDataSource
//////////////////////////////////////////////////////////////////////////

CDoubleCardWinpCapDataSource::CDoubleCardWinpCapDataSource()
{
	m_pAdapterObj = NULL;

	m_nPacketNums = 0;
	m_pCurBuffer = 0;
}

CDoubleCardWinpCapDataSource::~CDoubleCardWinpCapDataSource()
{

}

BOOL CDoubleCardWinpCapDataSource::init(const WINPCAP_RecvDRV_PARAM& param)
{
	m_nPacketNums = 0;
	m_pCurBuffer = 0;

	m_pAdapterObj = new adapterobj(adapterobj::INPUTDATATYPE_TAP);

	if( !m_pAdapterObj )
		return false;

	return CWinpCapDataSource::init(param);
}

void CDoubleCardWinpCapDataSource::fini()
{
	CWinpCapDataSource::fini();
}

bool CDoubleCardWinpCapDataSource::GetDataSourceName(std::string& name)
{
	name = "winpcap double card";

	return true;
}

int CDoubleCardWinpCapDataSource::DoubleCard_ReadDataFromHW()
{
	if( !CWinpCapDataSource::ReadDataFromHW() )
		return -1;

	if( PACKET_POINTER(m_pPacket)->ulBytesReceived==0 )
		return 0;

	DWORD packetNums = *(DWORD *)(PACKET_POINTER(m_pPacket)->Buffer);
		
	if(packetNums == 0xffffffff)
	{
		FreePacket();	

		packetNums = 0;
	}

	if( packetNums )
	{
		m_pCurBuffer = (char *)PACKET_POINTER(m_pPacket)->Buffer + 4;
	}
	else
	{
		m_pCurBuffer = NULL;
	}

	return packetNums;
}

PACKET_LEN_TYPE CDoubleCardWinpCapDataSource::GetPacket(RECV_PACKET& packet)
{
	if( m_nPacketNums == 0 )
	{
		FreePacket();

		m_nPacketNums = DoubleCard_ReadDataFromHW();
	}

	if( m_nPacketNums == 0 )
	{
		return PACKET_EMPTY;	
	}

	if( (int)m_nPacketNums == -1 )
	{
		return PACKET_DRV_ERROR;
	}
	
	struct HD{
		DWORD length;
		timeval timestamp;
		DWORD  netcardid;
	};
	
	HD *hd = (HD*)(m_pCurBuffer);
	m_pCurBuffer += sizeof(HD);

	packet.pHeaderinfo = &m_headerInfo;
	packet.pPacket = m_pCurBuffer;
	packet.nPktlen = hd->length+4;
	packet.pHeaderinfo->stamp.from_timeval(hd->timestamp);
	packet.nCaplen = packet.nPktlen;
	packet.nOffsetBit = 0;
	packet.nCapAddiBitLen = 0;
	packet.nPktAddiBitLen = 0;
	
	m_pCurBuffer += Packet_WORDALIGN(hd->length);
	
	--m_nPacketNums;

	return PACKET_OK;
} 



//////////////////////////////////////////////////////////////////////////
//CVPKWinpCapDataSource
//////////////////////////////////////////////////////////////////////////

CVPKWinpCapDataSource::CVPKWinpCapDataSource()
{
	m_pWinpcapDataSource = NULL;

}

CVPKWinpCapDataSource::~CVPKWinpCapDataSource()
{
	Close();
}


BOOL CVPKWinpCapDataSource::Open(int nopen,int type,const WINPCAP_RecvDRV_PARAM& param)
{
	if( 0 == type )
	{
		m_pWinpcapDataSource = new CSingleCardWinpCapDataSource; 		
	}
	else if( 1 == type )
	{
		m_pWinpcapDataSource = new CDoubleCardWinpCapDataSource;
	}
	else
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	CVPKHelpDataSource::init(m_pWinpcapDataSource,1);

	return m_pWinpcapDataSource->Open(param);
}

void CVPKWinpCapDataSource::Close()
{
	CloseDataSource();

	CVPKHelpDataSource::fini();

}

#endif//OS_WINDOWS

