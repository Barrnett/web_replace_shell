//////////////////////////////////////////////////////////////////////////
//WinpCapSendPacket.cpp


#include "cpf/cpf.h"

#ifdef OS_WINDOWS

#include "PacketIO/WinpCapSendPacket.h"
//#include "Packet.h"
#include "packet32.h"
#include "adapterobj.h"

CWinpCapSendPacket::CWinpCapSendPacket()
{
	m_lpAdapter = NULL;
}

CWinpCapSendPacket::~CWinpCapSendPacket()
{
	Close();
}

/*==========================================================
* Function	    : CWinpCapSendPacket::Open
* Description	: Open adapter
* Return	    : BOOL 
* Comments		: 
*=========================================================*/
BOOL CWinpCapSendPacket::Open(int indexofcard)	
{
	char   AdapterList[10][1024];

	memset(AdapterList,0x00,sizeof(AdapterList));

	int ncount = adapterobj::GetAdaptersName_NT(AdapterList);

	if( ncount <= indexofcard )
		return false;

	m_lpAdapter =   PacketOpenAdapter( AdapterList[indexofcard] );		
	if( !m_lpAdapter || (((LPADAPTER)m_lpAdapter)->hFile == INVALID_HANDLE_VALUE) )
	{
		DWORD dwErrorCode = GetLastError();
		//TRACE("Unable to open the send driver, Error Code : %lx\n",dwErrorCode); 
		return FALSE;
	}	

	return true;
}

/*==========================================================
* Function	    : CWinpCapSendPacket::Close
* Description	: Close Adapter
* Return	    : void 
* Comments		: 
*=========================================================*/
void CWinpCapSendPacket::Close()
{
	if( m_lpAdapter )
	{
		PacketCloseAdapter( ((LPADAPTER)m_lpAdapter) );
		m_lpAdapter = NULL;
	}
}

/*==========================================================
* Function	    : CWinpCapSendPacket::SendPacket
* Description	: Send Packet according to Packet direction(dire)
* Return	    : BOOL 
* Parament		: PACKETDIRECTION dire
* Parament		: const char *pszPacket
* Parament		: DWORD dwPacketLen
* Comments		: 
*=========================================================*/
BOOL CWinpCapSendPacket::SendPacket(const char *pszPacket, DWORD dwPacketLen)
{
	PACKET PacketSend;

	if( dwPacketLen < 60 )
	{
		//memset((void*)(pszPacket+dwPacketLen), 0, 60-dwPacketLen);
		dwPacketLen = 60;
	}
	//Set PACKET
	PacketSend.Buffer = (void *)pszPacket;
	PacketSend.ulBytesReceived = 0;
	PacketSend.bIoComplete = FALSE;
	PacketSend.Length = dwPacketLen;

	//Get Adapter according to Packet direction(dire)

	return PacketSendPacket(((LPADAPTER)m_lpAdapter), &PacketSend, true );
}

int CWinpCapSendPacket::GetConnectState()
{
	if( m_lpAdapter )
	{
		return 1;
	}

	return 0;

}

//////////////////////////////////////////////////////////////////////////
//CWinpcapWriteDataStream
//////////////////////////////////////////////////////////////////////////

CWinpcapWriteDataStream::CWinpcapWriteDataStream()
{
	m_nIndex = 0;
}

CWinpcapWriteDataStream::~CWinpcapWriteDataStream()
{
	Close();
}

//相当于init+OpenDataStream
BOOL CWinpcapWriteDataStream::Open(int index)
{
	if( FALSE == init(index) )
		return FALSE;

	if( OpenDataStream() == 0 )
		return TRUE;

	return FALSE;
}

//相当于CloseDataStream+fini
void CWinpcapWriteDataStream::Close()
{
	CloseDataStream();

	fini();

	return;
}


BOOL CWinpcapWriteDataStream::init(int index)
{
	m_nIndex = index;

	return true;
}
void CWinpcapWriteDataStream::fini()
{

}

//打开设备．这个函数应该在参数设置好之后调用
//成功返回0,否则返回-1
int CWinpcapWriteDataStream::OpenDataStream()
{
	if( TRUE == m_winpcapSendPacket.Open(m_nIndex) )
		return 0;

	return -1;
}

//关闭设备
void CWinpcapWriteDataStream::CloseDataStream()
{
	m_winpcapSendPacket.Close();
}

int CWinpcapWriteDataStream::WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums)
{
	if( type != PACKET_OK )
		return -1;

	if( TRUE == m_winpcapSendPacket.SendPacket(pdata,(DWORD)(datalen-4)) )
		return 0;

	return -1;

}

int CWinpcapWriteDataStream::GetConnectState()
{
	return m_winpcapSendPacket.GetConnectState();

}

bool CWinpcapWriteDataStream::GetDataStreamName(std::string& name)
{
	name = "winpcap";

	return true;
}



#endif//OS_WINDOWS

