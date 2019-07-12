//////////////////////////////////////////////////////////////////////////
//CapdataObserver.cpp

#include "CapdataObserver.h"
#include "PacketIO/DataSendMgr.h"
#include "cpf/path_tools.h"

CCapdataObserver::CCapdataObserver(void)
{
	m_pDataSendMgr = NULL;
}

CCapdataObserver::~CCapdataObserver(void)
{
	fini();
}

BOOL CCapdataObserver::init()
{
	m_pDataSendMgr = new CDataSendMgr;

	char fullname[MAX_PATH];
	CPF::GetModuleOtherFileName(fullname,sizeof(fullname),"ini","capdata.ini");

	if( !m_pDataSendMgr->init(fullname,0,NULL) )
		return false;

	if( !m_pDataSendMgr->OpenWriteDataStream() )
		return false;

	return true;

}
void CCapdataObserver::fini()
{
	if( m_pDataSendMgr )
	{
		m_pDataSendMgr->CloseWriteDataStream();
		m_pDataSendMgr->close();

		delete m_pDataSendMgr;
		m_pDataSendMgr = NULL;
	}
	
	return;
}

BOOL CCapdataObserver::Reset()
{
	return true;

}
BOOL CCapdataObserver::OnStart()
{
	if( m_pDataSendMgr->GetCurWriteDataStream() )
	{
		return m_pDataSendMgr->GetCurWriteDataStream()->StartToWrite();
	}

	return false;
}

BOOL CCapdataObserver::OnStop(int type)
{
	if( m_pDataSendMgr->GetCurWriteDataStream() )
	{
		m_pDataSendMgr->GetCurWriteDataStream()->StopWriting();
	}

	return false;

}

BOOL CCapdataObserver::OnPause()
{
	return true;
}

BOOL CCapdataObserver::OnContinue()
{
	return true;
}

BOOL CCapdataObserver::Push(PACKET_LEN_TYPE type,RECV_PACKET& packet)
{
	if( type == PACKET_OK 
		&& m_pDataSendMgr->GetCurWriteDataStream() )
	{
		m_pDataSendMgr->GetCurWriteDataStream()->WritePacket(type,packet,-1,1);
	}

	return true;
}
