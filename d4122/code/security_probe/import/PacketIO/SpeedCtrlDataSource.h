//////////////////////////////////////////////////////////////////////////
//SpeedCtrlDataSource.h

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

class CPktSpeedCtrl;

class PACKETIO_CLASS CSpeedCtrlDataSource : public IExtDataSource
{
public:
	CSpeedCtrlDataSource(void);
	virtual ~CSpeedCtrlDataSource(void);


	BOOL init(IRecvDataSource * pRealDataSource,bool bdelete,
		ACE_UINT64 speed,unsigned int uint_nums);

	void fini();

	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	virtual BOOL StartToRecv();
	virtual void StopRecving();

private:
	//备份上一次读出了包的数据
	//我们用m_bkRecvPacket.pHeaderinfo=NULL表示有无备份以前的数据
	RECV_PACKET		m_bkRecvPacket;

	//备份上一次读出了包的数据的长度类型
	PACKET_LEN_TYPE	m_bkType;

	//上一个数据包的长度
	unsigned int	m_last_packet_len;

private:
	inline BOOL IsBackupLastPkt() const
	{
		return (PACKET_EMPTY != m_bkType);
	}

private:
	ACE_UINT64		m_speed;
	unsigned int	m_uint_nums;
	CPktSpeedCtrl *	m_pPktSpeedCtrl;

	void CreatePktSpeedCtrl();


};
