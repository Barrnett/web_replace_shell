//////////////////////////////////////////////////////////////////////////
//SendPktObserver.cpp

#include "PacketIO/SendPktObserver.h"
//#include "cpf/CommonRTPDecode.h"
//#include "cpf/CommonMacTCPIPDecode.h"

CSendPktObserver::CSendPktObserver(void)
{
	m_pWriteDataStream = NULL;
}

CSendPktObserver::~CSendPktObserver(void)
{
}

BOOL CSendPktObserver::init(IWriteDataStream * pWriteDataStream)
{
	m_pWriteDataStream = pWriteDataStream;

	return TRUE;
}

void CSendPktObserver::fini()
{
	m_pWriteDataStream = NULL;
}

BOOL CSendPktObserver::Reset()
{
	return TRUE;
}
BOOL CSendPktObserver::OnStart()
{
	if( m_pWriteDataStream )
	{
		m_pWriteDataStream->StartToWrite();
	}

	return TRUE;
}
BOOL CSendPktObserver::OnStop(int type)
{
	if( m_pWriteDataStream )
	{
		m_pWriteDataStream->StopWriting(false);
	}
	
	return TRUE;
}

BOOL CSendPktObserver::OnPause()
{
	return TRUE;
}
BOOL CSendPktObserver::OnContinue()
{
	return TRUE;
}

BOOL CSendPktObserver::Push(PACKET_LEN_TYPE type,RECV_PACKET& packet)
{
	if( type == PACKET_OK )
	{
		m_pWriteDataStream->WritePacket(type,packet,-1,-1);
	}

	return TRUE;

}

ACE_UINT64 CSendPktObserver::GetTotalSendPkts() const
{	
	if( !m_pWriteDataStream )
		return 0;

	return m_pWriteDataStream->GetTotalSendPkts();	
}

ACE_UINT64 CSendPktObserver::GetTotalFailedPkts() const
{	
	if( !m_pWriteDataStream )
		return 0;

	return m_pWriteDataStream->GetTotalFailedPkts();
}

