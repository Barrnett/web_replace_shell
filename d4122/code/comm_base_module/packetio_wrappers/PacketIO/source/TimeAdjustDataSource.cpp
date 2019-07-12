//////////////////////////////////////////////////////////////////////////
//TimeAdjustDataSource.cpp


#include "PacketIO/TimeAdjustDataSource.h"
#include "TimeAdjustTool.h"

CTimeAdjustDataSource::CTimeAdjustDataSource(void)
{
	m_pTimeAdjustTool = NULL;
}

CTimeAdjustDataSource::~CTimeAdjustDataSource(void)
{
	Close();
}


void CTimeAdjustDataSource::init(IRecvDataSource * pRealDataSource,int delete_real_source,
								 int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp)
{
	IExtDataSource::init(pRealDataSource,delete_real_source);

	if( 0 != nAdjustType )
	{
		m_pTimeAdjustTool = new CTimeAdjustTool;

		m_pTimeAdjustTool->init(nAdjustType,pAdjustTimeStamp);
	}

	return;

}

void CTimeAdjustDataSource::fini()
{
	IExtDataSource::fini();
	
}

BOOL CTimeAdjustDataSource::StartToRecv()
{
	if( m_pTimeAdjustTool )
	{
		m_pTimeAdjustTool->start_to_work();
	}

	return IExtDataSource::StartToRecv();
}
//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CTimeAdjustDataSource::StopRecving()
{
	if( m_pTimeAdjustTool )
	{
		m_pTimeAdjustTool->stop_working();
	} 

	return IExtDataSource::StopRecving();

}

//接收数据
PACKET_LEN_TYPE CTimeAdjustDataSource::GetPacket(RECV_PACKET& packet)
{
	PACKET_LEN_TYPE lentype = m_pRealDataSource->GetPacket(packet);

	if( m_pTimeAdjustTool )
	{
		m_pTimeAdjustTool->do_work(lentype,packet);
	}

	return lentype;


}


