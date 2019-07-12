//////////////////////////////////////////////////////////////////////////
//MultiFileDataSource.cpp

#include "PacketIO/MultiFileDataSource.h"
#include "PacketIO/DataSourceHelper.h"
#include "PacketIO/ZCCPTDataSource.h"
#include "ace/Guard_T.h"

CMultiFileDataSource::CMultiFileDataSource(void)
{
	m_pCurDataSource = NULL;
	m_nCurIndex = -1;

	m_totalpkts = -1;
	m_totalbytes = -1;

	m_re_calculate_time = true;
	m_bcur_systime = false;
	m_bskip_error_file = false;

	m_nReadNums = 0;
	m_nBufferSizeMB = 4;
}

CMultiFileDataSource::~CMultiFileDataSource(void)
{
	Close();
}

bool CMultiFileDataSource::GetDataSourceName(std::string& name)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_lock);

	if( m_pCurDataSource )
	{
		return m_pCurDataSource->GetDataSourceName(name);
	}
	else
	{
		if( m_vtFileItem.size() > 0 )
		{
			name = m_vtFileItem[0].second;
			return true;
		}
	}

	return false;		
}

void CMultiFileDataSource::fini()
{
	m_vtFileItem.clear();
}

BOOL CMultiFileDataSource::Open_EthrealFiles(const CStdStringArray& vtFilename,
								bool re_calculate_time,bool bcur_systime,bool skip_error_file)
{
	init_Ethreal(re_calculate_time,bcur_systime,skip_error_file);
	
	m_vtFileItem.clear();
	AddMultiFileName(DRIVER_TYPE_FILE_ETHEREAL,vtFilename);

	return (OpenDataSource() == 0);
}

BOOL CMultiFileDataSource::Open_CptFiles(const CStdStringArray& vtFilename,
										 unsigned int nReadNums,unsigned int bufsizeMB,\
										 bool re_calculate_time,bool bcur_systime,bool skip_error_file)
{
	init_Cpt(nReadNums,bufsizeMB,re_calculate_time,bcur_systime,skip_error_file);

	m_vtFileItem.clear();
	AddMultiFileName(DRIVER_TYPE_FILE_ZCCPT,vtFilename);

	return (OpenDataSource() == 0);
}


BOOL CMultiFileDataSource::Open_EthrealFile(const char * filename,
								bool re_calculate_time,bool bcur_systime,bool skip_error_file)
{
	init_Ethreal(re_calculate_time,bcur_systime,skip_error_file);

	m_vtFileItem.clear();
	AddFileName(DRIVER_TYPE_FILE_ETHEREAL,filename);

	return (OpenDataSource() == 0);
}

BOOL CMultiFileDataSource::Open_CptFile(const char * filename,
										unsigned int nReadNums,unsigned int bufsizeMB,
										bool re_calculate_time,bool bcur_systime,bool skip_error_file)
{
	init_Cpt(nReadNums,bufsizeMB,re_calculate_time,bcur_systime,skip_error_file);

	m_vtFileItem.clear();
	AddFileName(DRIVER_TYPE_FILE_ZCCPT,filename);

	return (OpenDataSource() == 0);
}

//一次调用传入多个
void CMultiFileDataSource::AddMultiFileName(DRIVER_TYPE type,const CStdStringArray& vtFilename)
{
	for(size_t i = 0; i < vtFilename.size(); ++i)
	{
		m_vtFileItem.push_back(std::make_pair(type,vtFilename[i]));
	}	
}

void CMultiFileDataSource::AddFileName(DRIVER_TYPE type,const char * filename)
{
	m_vtFileItem.push_back(std::make_pair(type,std::string(filename)));
}


int CMultiFileDataSource::OpenDataSource()
{
	m_totalpkts = -1;
	m_totalbytes = -1;

	if( m_vtFileItem.size() == 0 )
		return -1;

	m_totalpkts = 0;
	m_totalbytes = 0;

	//尝试看是否所有文件都能正确打开
	for(size_t i = 0; i < m_vtFileItem.size(); ++i)
	{
		ISingleFileRecvDataSource * pFileDataSource = OpenByIndex((int)i);

		if( !pFileDataSource )
			return -1;

		CalculateTotalPkts(pFileDataSource);

		pFileDataSource->Close();
		CDataSourceHelper::DestroyRecvDataSource(pFileDataSource);
	}

	return 0;
}

void CMultiFileDataSource::CloseDataSource()
{
	StopRecving();
}

void CMultiFileDataSource::GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops)
{
	pkts = m_totalpkts;
	bytes = m_totalbytes;
	nloops = 1;
}

ISingleFileRecvDataSource * CMultiFileDataSource::OpenByIndex(int index)
{
	ISingleFileRecvDataSource * pCurDataSource  =
		CreateSingleFileRecvDataSource(m_vtFileItem[index].first);

	if( pCurDataSource == NULL )
		return NULL;

	BOOL bOpen = false;

	if( DRIVER_TYPE_FILE_ETHEREAL == m_vtFileItem[index].first )
	{
		bOpen = pCurDataSource->Open(m_vtFileItem[index].second.c_str(),m_bcur_systime);
	}
	else if( DRIVER_TYPE_FILE_ZCCPT == m_vtFileItem[index].first )
	{
		bOpen = ((CZCCPTDataSource *)pCurDataSource)->Open(
			m_vtFileItem[index].second.c_str(),m_bcur_systime,
			m_nReadNums,m_nBufferSizeMB);
	}

	if( !bOpen )
	{
		CDataSourceHelper::DestroyRecvDataSource(pCurDataSource);
		pCurDataSource = NULL;
		return NULL;
	}

	return pCurDataSource;
}

//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
BOOL CMultiFileDataSource::StartToRecv()
{
	if( m_vtFileItem.size() == 0 )
		return false;

	m_nCurIndex = 0;

	m_pCurDataSource = OpenByIndex(m_nCurIndex);

	m_nBaseTime = CTimeStamp32::zero;
	m_nCurFileLastTime = m_nCurFileFirstTime =  CTimeStamp32::zero;

	if (m_pCurDataSource)
		return m_pCurDataSource->StartToRecv();
	else
		return FALSE;
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CMultiFileDataSource::StopRecving()
{
	if( m_pCurDataSource )
	{
		m_pCurDataSource->StopRecving();
		m_pCurDataSource->CloseDataSource();
		
		CDataSourceHelper::DestroyRecvDataSource(m_pCurDataSource);
		m_pCurDataSource = NULL;
	}

	m_nBaseTime = CTimeStamp32::zero;
	m_nCurFileLastTime = m_nCurFileFirstTime =  CTimeStamp32::zero;

	return;
}


PACKET_LEN_TYPE CMultiFileDataSource::GetPacket(RECV_PACKET& packet)
{
	PACKET_LEN_TYPE lentype = m_pCurDataSource->GetPacket(packet);

	if( PACKET_OK == lentype || PACKET_TIMER == lentype)
	{
		if( !m_bcur_systime && m_re_calculate_time )
		{
			if( m_nBaseTime == CTimeStamp32::zero )
			{//获取基准时间

				m_nCurFileLastTime = m_nCurFileFirstTime = packet.pHeaderinfo->stamp;

				//得到第一个文件的第一个包的时间，就是基准时间
				m_nBaseTime = m_nCurFileLastTime;

			}
			else
			{
				m_nCurFileLastTime = packet.pHeaderinfo->stamp;
			}

			//基于基准时间和当前包的时间进行时间调整
			packet.pHeaderinfo->stamp = 
				(m_nBaseTime + (CTimeStamp32(packet.pHeaderinfo->stamp) - m_nCurFileFirstTime)).m_ts;

		}			
	}
	else if( PACKET_NO_MORE == lentype )
	{
		lentype = TurnToNextFileAndRead(packet);
	}
	else if( PACKET_DRV_ERROR == lentype )
	{
		if( m_bskip_error_file )
		{
			lentype = TurnToNextFileAndRead(packet);
		}
	}

	return lentype;	

}

PACKET_LEN_TYPE CMultiFileDataSource::TurnToNextFileAndRead(RECV_PACKET& packet)
{	
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_lock);

	while(1)
	{
		m_pCurDataSource->StopRecving();
		m_pCurDataSource->CloseDataSource();

		CDataSourceHelper::DestroyRecvDataSource(m_pCurDataSource);
		m_pCurDataSource = NULL;

		//如果文件全部回放结束，则认为结束了
		if( m_nCurIndex == m_vtFileItem.size()-1 )
			return PACKET_NO_MORE;

		if( !m_bcur_systime && m_re_calculate_time )
		{
			//调整基准时间
			m_nBaseTime += (m_nCurFileLastTime-m_nCurFileFirstTime);

			m_nCurFileLastTime = m_nCurFileFirstTime = CTimeStamp32::zero;
		}

		++m_nCurIndex;

		//下一个文件
		m_pCurDataSource = 	OpenByIndex(m_nCurIndex);

		if( m_pCurDataSource == NULL )
			return PACKET_DRV_ERROR;

		if( !m_pCurDataSource->StartToRecv() )
		{
			CDataSourceHelper::DestroyRecvDataSource(m_pCurDataSource);
			m_pCurDataSource = NULL;
			return PACKET_DRV_ERROR;
		}

		PACKET_LEN_TYPE lentype = m_pCurDataSource->GetPacket(packet);

		//空文件的情况，应该立即跳到下一个文件去读
		if( PACKET_NO_MORE == lentype )
			continue;

		if( !m_bcur_systime && m_re_calculate_time )
		{
			//得到第一个包的时间
			m_nCurFileLastTime = m_nCurFileFirstTime = packet.pHeaderinfo->stamp;

			//基于基准时间和当前包的时间进行时间调整
			packet.pHeaderinfo->stamp = 
				(m_nBaseTime + (CTimeStamp32(packet.pHeaderinfo->stamp) - m_nCurFileFirstTime)).m_ts;	
		}
	
		return lentype;		
	}

	ACE_ASSERT(false);

	return PACKET_NO_MORE;
}


//将读数据位置指向开始的地方。
BOOL CMultiFileDataSource::SeekToBegin()
{
	StopRecving();

	m_nCurIndex = 0;

	return StartToRecv();
}

int CMultiFileDataSource::GetConnectState()
{
	if( m_pCurDataSource )
	{
		return m_pCurDataSource->GetConnectState();
	}

	return 0;
}


void CMultiFileDataSource::CalculateTotalPkts(ISingleFileRecvDataSource * pFileDataSource)
{
	ACE_UINT64		curtotalpkts = -1;//数据源的总包数
	ACE_UINT64		curtotalbytes = -1;//数据源的总字节数

	unsigned int	nloops = 1;

	//得到当前文件的总包数和总自己数
	pFileDataSource->GetTotalPkts(curtotalpkts,curtotalbytes,nloops);

	//如果以前的文件的包数无效，或者当前文件的包数无效，则不需要计算
	if( m_totalpkts == -1 || curtotalpkts == -1 )
		m_totalpkts = -1;
	else
		m_totalpkts += curtotalpkts;

	//如果以前的文件的字节数无效，或者当前文件的字节数无效，则不需要计算
	if( m_totalbytes == -1 || curtotalbytes == -1 )
		m_totalbytes = -1;
	else
		m_totalbytes += curtotalbytes;

	return;
}

ISingleFileRecvDataSource * CMultiFileDataSource
::CreateSingleFileRecvDataSource(DRIVER_TYPE type)
{
	return (ISingleFileRecvDataSource *)CDataSourceHelper::CreateRecvDataSource(type,(ACE_UINT64)-1,0,0,NULL,false,0,false,1,0);

}


//////////////////////////////////////////////////////////////////////////
//CVPKMultiFileDataSource
//////////////////////////////////////////////////////////////////////////

CVPKMultiFileDataSource::CVPKMultiFileDataSource()
{
	m_pMultifileDataSource = NULL;
}

CVPKMultiFileDataSource::~CVPKMultiFileDataSource()
{
	Close();
}

BOOL CVPKMultiFileDataSource::Open(DRIVER_TYPE type,const CStdStringArray& vtFilename,int re_calculate_time)
{
	CVPKMultiFileDataSource::init(re_calculate_time);

	CVPKMultiFileDataSource::AddMultiFileName(type,vtFilename);

	return (OpenDataSource() == 0);
}

BOOL CVPKMultiFileDataSource::Open(DRIVER_TYPE type,const char * filename,int re_calculate_time)
{
	CVPKMultiFileDataSource::init(re_calculate_time);

	CVPKMultiFileDataSource::AddFileName(type,filename);

	return (OpenDataSource() == 0);
}

void CVPKMultiFileDataSource::Close()
{
	CloseDataSource();

	CVPKMultiFileDataSource::fini();
}


void CVPKMultiFileDataSource::init(int re_calculate_time)
{
	return;
	m_pMultifileDataSource = new CMultiFileDataSource;

	if( m_pMultifileDataSource )
	{
		//m_pMultifileDataSource->init(re_calculate_time);
	}

	CVPKHelpDataSource::init(m_pMultifileDataSource,1);
}

//一次调用传入多个文件，可以多次调用。文件会累加
void CVPKMultiFileDataSource::AddMultiFileName(DRIVER_TYPE type,const CStdStringArray& vtFilename)
{
	if( m_pMultifileDataSource )
	{
		m_pMultifileDataSource->AddMultiFileName(type,vtFilename);
	}

}

//可以多次调用AddFileName。文件会累加
void CVPKMultiFileDataSource::AddFileName(DRIVER_TYPE type,const char * filename)
{
	if( m_pMultifileDataSource )
	{
		m_pMultifileDataSource->AddFileName(type,filename);
	}
}

void CVPKMultiFileDataSource::fini()
{
	if( m_pMultifileDataSource )
	{
		m_pMultifileDataSource->fini();
	}

	CVPKHelpDataSource::fini();

	m_pMultifileDataSource = NULL;

	return;

}

