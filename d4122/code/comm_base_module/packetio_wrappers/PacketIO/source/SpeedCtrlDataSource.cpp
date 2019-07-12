//////////////////////////////////////////////////////////////////////////
//SpeedCtrlDataSource.cpp

#include "PacketIO/SpeedCtrlDataSource.h"
#include "cpf/PktSpeedCtrl.h"

CSpeedCtrlDataSource::CSpeedCtrlDataSource(void)
{
	m_last_packet_len = 0;

	m_pPktSpeedCtrl = NULL;
}

CSpeedCtrlDataSource::~CSpeedCtrlDataSource(void)
{
	fini();
}


BOOL CSpeedCtrlDataSource::init(IRecvDataSource * pRealDataSource,bool bdelete,
		  ACE_UINT64 speed,unsigned int uint_nums)
{
	IExtDataSource::init(pRealDataSource,bdelete);

	m_speed = speed;
	m_uint_nums = uint_nums;

	return true;
}

void CSpeedCtrlDataSource::fini()
{
	IExtDataSource::fini();

	if( m_pPktSpeedCtrl )
	{
		delete m_pPktSpeedCtrl;
		m_pPktSpeedCtrl = NULL;
	}

	return;
}


BOOL CSpeedCtrlDataSource::StartToRecv()
{
	m_bkType = PACKET_EMPTY;

	m_last_packet_len = 0;

	CreatePktSpeedCtrl();

	return IExtDataSource::StartToRecv();
}

void CSpeedCtrlDataSource::StopRecving()
{
	IExtDataSource::StopRecving();
}

void CSpeedCtrlDataSource::CreatePktSpeedCtrl()
{
	if( m_pPktSpeedCtrl )
	{
		delete m_pPktSpeedCtrl;
		m_pPktSpeedCtrl = NULL;
	}

	if( m_speed > 0 && m_speed != (ACE_UINT64)-1 )
	{
		m_pPktSpeedCtrl = ::CreatePktSpeedCtrl(0,m_speed,m_uint_nums);
	}

	return;
}

PACKET_LEN_TYPE CSpeedCtrlDataSource::GetPacket(RECV_PACKET& packet)
{
	PACKET_LEN_TYPE len_type = PACKET_EMPTY;

	if( !IsBackupLastPkt() )
	{//以前没有备份

		len_type = m_pRealDataSource->GetPacket(packet);

		if( PACKET_OK != len_type )
		{
			return len_type;
		}

		m_last_packet_len = packet.nPktlen;
	}

	ACE_Time_Value sleeptime;

	if( !m_pPktSpeedCtrl->IfCanSendPkt(m_last_packet_len,&sleeptime) )
	{//不可以发送了

		if( !IsBackupLastPkt() )
		{//如果以前没有备份，才需要备份
			m_bkType = len_type;
			m_bkRecvPacket = packet;
		}

		packet.pHeaderinfo = NULL;
		packet.pPacket = NULL;

		if( sleeptime.get_msec() > 5 )
		{
			return PACKET_EMPTY;
		}
		else
		{
			return PACKET_CALL_NEXT;
		}
	}
	else
	{//可以发送了

		if( IsBackupLastPkt() )
		{//如果以前有备份，则返回以前备份的数据

			len_type = m_bkType;
			packet = m_bkRecvPacket;
		}

		m_bkType = PACKET_EMPTY;

		return len_type;
	
	}

	ACE_ASSERT(false);

	return PACKET_CALL_NEXT;

}
