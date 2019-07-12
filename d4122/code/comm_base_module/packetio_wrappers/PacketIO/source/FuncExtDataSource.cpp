//////////////////////////////////////////////////////////////////////////
//FuncExtDataSource.cpp

#include "PacketIO/FuncExtDataSource.h"
#include "TimeAdjustTool.h"

CFuncExtDataSource::CFuncExtDataSource(void)
{
	m_pTimeAdjustTool = NULL;
	m_pCopyAndDiscardTool = NULL;
	m_pLoopTool = NULL;
}

CFuncExtDataSource::~CFuncExtDataSource(void)
{
	fini();
}

BOOL CFuncExtDataSource::init(IRecvDataSource * pRealDataSource,bool bdelete,
							  int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp,
							  BOOL bCopyPkt,unsigned int discard_rate,
							  size_t nloopnums,BOOL re_calculate_time
							  )
{
	IExtDataSource::init(pRealDataSource,bdelete);

	if( bCopyPkt || discard_rate != 0 )
	{
		m_pCopyAndDiscardTool = new CCopyAndDiscardTool;

		m_pCopyAndDiscardTool->init(bCopyPkt,discard_rate);
	}

	if( 0 != nAdjustType )
	{
		m_pTimeAdjustTool = new CTimeAdjustTool;

		m_pTimeAdjustTool->init(nAdjustType,pAdjustTimeStamp);
	}

	if( 1 != nloopnums )
	{
		m_pLoopTool = new CLoopTool;

		//���Ҫ��Ϊ����Ϊ��ǰʱ�䣬��ѭ������Ҫ����ʱ��
		re_calculate_time = (re_calculate_time && (nAdjustType != 1));

		m_pLoopTool->init(pRealDataSource,nloopnums,re_calculate_time);

	}

	return true;
}

void CFuncExtDataSource::fini()
{
	IExtDataSource::fini();

	if( m_pTimeAdjustTool )
	{
		delete m_pTimeAdjustTool;
		m_pTimeAdjustTool = NULL;
	}

	if( m_pCopyAndDiscardTool )
	{
		delete m_pCopyAndDiscardTool;
		m_pCopyAndDiscardTool = NULL;
	}

	if( m_pLoopTool )
	{
		delete m_pLoopTool;
		m_pLoopTool = NULL;
	}

	return;
}


BOOL CFuncExtDataSource::StartToRecv()
{
	if( m_pTimeAdjustTool )
	{
		m_pTimeAdjustTool->start_to_work();
	}

	if( m_pCopyAndDiscardTool )
	{
		m_pCopyAndDiscardTool->start_to_work();
	}

	if( m_pLoopTool )
	{
		m_pLoopTool->start_to_work();
	}

	return IExtDataSource::StartToRecv();
}

void CFuncExtDataSource::StopRecving()
{
	if( m_pTimeAdjustTool )
	{
		m_pTimeAdjustTool->stop_working();
	}

	if( m_pCopyAndDiscardTool )
	{
		m_pCopyAndDiscardTool->stop_working();
	}

	if( m_pLoopTool )
	{
		m_pLoopTool->stop_working();
	}

	IExtDataSource::StopRecving();

	return;
}


//��������
PACKET_LEN_TYPE CFuncExtDataSource::GetPacket(RECV_PACKET& packet)
{
	PACKET_LEN_TYPE len_type = m_pRealDataSource->GetPacket(packet);

	if(m_pLoopTool )
	{
		m_pLoopTool->do_work(len_type,packet);
	}	

	//���뽫����ģ�����loop֮�󣬷���ʱ��ͻ����򣬵���ԭ��û���ҵ���
	//???????????
	if( m_pCopyAndDiscardTool )
	{
		m_pCopyAndDiscardTool->do_work(len_type,packet);
	}

	//ʱ�����Ӧ�÷��������
	if( m_pTimeAdjustTool )
	{
		m_pTimeAdjustTool->do_work(len_type,packet);
	}

	return len_type;	
}

