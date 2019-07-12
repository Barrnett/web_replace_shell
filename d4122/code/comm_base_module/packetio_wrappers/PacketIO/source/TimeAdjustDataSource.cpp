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
//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
void CTimeAdjustDataSource::StopRecving()
{
	if( m_pTimeAdjustTool )
	{
		m_pTimeAdjustTool->stop_working();
	} 

	return IExtDataSource::StopRecving();

}

//��������
PACKET_LEN_TYPE CTimeAdjustDataSource::GetPacket(RECV_PACKET& packet)
{
	PACKET_LEN_TYPE lentype = m_pRealDataSource->GetPacket(packet);

	if( m_pTimeAdjustTool )
	{
		m_pTimeAdjustTool->do_work(lentype,packet);
	}

	return lentype;


}


