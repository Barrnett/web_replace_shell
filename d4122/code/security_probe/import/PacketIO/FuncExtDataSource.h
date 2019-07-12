//////////////////////////////////////////////////////////////////////////
//FuncExtDataSource.h

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"
#include "cpf/Octets.h"

class CTimeAdjustTool;
class CCopyAndDiscardTool;
class CLoopTool;

class PACKETIO_CLASS CFuncExtDataSource :	public IExtDataSource
{
public:
	CFuncExtDataSource(void);
	virtual ~CFuncExtDataSource(void);

public:
	BOOL init(IRecvDataSource * pRealDataSource,bool bdelete,
		int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp,
		BOOL bCopyPkt,unsigned int discard_rate,
		size_t nloopnums,BOOL re_calculate_time
		);

	void fini();

	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	virtual BOOL StartToRecv();
	virtual void StopRecving();

private:
	CCopyAndDiscardTool *	m_pCopyAndDiscardTool;

	CLoopTool *				m_pLoopTool;

	CTimeAdjustTool *		m_pTimeAdjustTool;

};
