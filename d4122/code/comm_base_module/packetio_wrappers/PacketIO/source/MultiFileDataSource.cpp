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

//һ�ε��ô�����
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

	//���Կ��Ƿ������ļ�������ȷ��
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

//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
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

//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
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
			{//��ȡ��׼ʱ��

				m_nCurFileLastTime = m_nCurFileFirstTime = packet.pHeaderinfo->stamp;

				//�õ���һ���ļ��ĵ�һ������ʱ�䣬���ǻ�׼ʱ��
				m_nBaseTime = m_nCurFileLastTime;

			}
			else
			{
				m_nCurFileLastTime = packet.pHeaderinfo->stamp;
			}

			//���ڻ�׼ʱ��͵�ǰ����ʱ�����ʱ�����
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

		//����ļ�ȫ���طŽ���������Ϊ������
		if( m_nCurIndex == m_vtFileItem.size()-1 )
			return PACKET_NO_MORE;

		if( !m_bcur_systime && m_re_calculate_time )
		{
			//������׼ʱ��
			m_nBaseTime += (m_nCurFileLastTime-m_nCurFileFirstTime);

			m_nCurFileLastTime = m_nCurFileFirstTime = CTimeStamp32::zero;
		}

		++m_nCurIndex;

		//��һ���ļ�
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

		//���ļ��������Ӧ������������һ���ļ�ȥ��
		if( PACKET_NO_MORE == lentype )
			continue;

		if( !m_bcur_systime && m_re_calculate_time )
		{
			//�õ���һ������ʱ��
			m_nCurFileLastTime = m_nCurFileFirstTime = packet.pHeaderinfo->stamp;

			//���ڻ�׼ʱ��͵�ǰ����ʱ�����ʱ�����
			packet.pHeaderinfo->stamp = 
				(m_nBaseTime + (CTimeStamp32(packet.pHeaderinfo->stamp) - m_nCurFileFirstTime)).m_ts;	
		}
	
		return lentype;		
	}

	ACE_ASSERT(false);

	return PACKET_NO_MORE;
}


//��������λ��ָ��ʼ�ĵط���
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
	ACE_UINT64		curtotalpkts = -1;//����Դ���ܰ���
	ACE_UINT64		curtotalbytes = -1;//����Դ�����ֽ���

	unsigned int	nloops = 1;

	//�õ���ǰ�ļ����ܰ��������Լ���
	pFileDataSource->GetTotalPkts(curtotalpkts,curtotalbytes,nloops);

	//�����ǰ���ļ��İ�����Ч�����ߵ�ǰ�ļ��İ�����Ч������Ҫ����
	if( m_totalpkts == -1 || curtotalpkts == -1 )
		m_totalpkts = -1;
	else
		m_totalpkts += curtotalpkts;

	//�����ǰ���ļ����ֽ�����Ч�����ߵ�ǰ�ļ����ֽ�����Ч������Ҫ����
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

//һ�ε��ô������ļ������Զ�ε��á��ļ����ۼ�
void CVPKMultiFileDataSource::AddMultiFileName(DRIVER_TYPE type,const CStdStringArray& vtFilename)
{
	if( m_pMultifileDataSource )
	{
		m_pMultifileDataSource->AddMultiFileName(type,vtFilename);
	}

}

//���Զ�ε���AddFileName���ļ����ۼ�
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

