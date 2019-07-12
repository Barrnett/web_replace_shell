//////////////////////////////////////////////////////////////////////////
//MultiFileWriteStream.cpp

#include "PacketIO/MultiFileWriteStream.h"
#include "WriteFileCtrl.h"
#include "PacketIO/DataSourceHelper.h"
#include "cpf/path_tools.h"

CMultiFileWriteStream::CMultiFileWriteStream(void)
{
	m_pWriteFileCtrl = NULL;

	m_pCurFileDataStream = NULL;

	m_nLastRtn = 0;
}

CMultiFileWriteStream::~CMultiFileWriteStream(void)
{
	fini();
}


BOOL CMultiFileWriteStream::init(IFileWriteDataStream * pFirstFileDataStream,
								 unsigned int numfiles,
								 unsigned int one_filsize_mb,
								 unsigned int captime)
{
	m_strOrgFilename = pFirstFileDataStream->GetFileName();

	m_pCurFileDataStream = pFirstFileDataStream;

	m_pWriteFileCtrl = new CWriteFileCtrl;

	if( !m_pWriteFileCtrl->init(numfiles,one_filsize_mb,captime) )
	{
		delete m_pWriteFileCtrl;
		m_pWriteFileCtrl = NULL;
	}

	m_nLastRtn = 0;

	return true;
}

void CMultiFileWriteStream::fini()
{
	if( m_pCurFileDataStream )
	{
		CDataSourceHelper::DestroyWriteDataStream(m_pCurFileDataStream);
		m_pCurFileDataStream = NULL;
	}

	if( m_pWriteFileCtrl )
	{
		delete m_pWriteFileCtrl;
		m_pWriteFileCtrl = NULL;
	}

	return;
}


int CMultiFileWriteStream::OpenDataStream()
{
	if( m_pCurFileDataStream )
	{
		return m_pCurFileDataStream->OpenDataStream();
	}

	return -1;
}

//关闭设备
void CMultiFileWriteStream::CloseDataStream()
{
	if( m_pCurFileDataStream )
	{
		m_pCurFileDataStream->CloseDataStream();
	}
}

int CMultiFileWriteStream::WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums)
{
	if( type != PACKET_OK )
		return -1;

	if( m_nLastRtn != 0 )
		return m_nLastRtn;

	m_nLastRtn = m_pCurFileDataStream->WritePacket(type,packet,index_array,try_nums);

	if( m_nLastRtn != 0 )
	{
		return m_nLastRtn;
	}

	if( m_pCurFileDataStream->IsSupportWriteCtrl() )
	{//如果本身已经支持了写控制
		return m_nLastRtn;
	}

	m_nLastRtn = m_pWriteFileCtrl->on_write_packet(packet.nCaplen);

	if( m_nLastRtn == 0 )
	{
		return 0;
	}

	if( m_nLastRtn == -2 || m_nLastRtn == -3 )
	{
		m_pCurFileDataStream->CloseDataStream();
		CDataSourceHelper::DestroyWriteDataStream(m_pCurFileDataStream);
		m_pCurFileDataStream = NULL;

		m_nLastRtn = -3;
		return m_nLastRtn;
	}

	if( m_nLastRtn == 1 )
	{//当前文件完成，应该下一个文件
		IFileWriteDataStream * pTempDataStream = CreateNextFileStream();

		if( !pTempDataStream )
		{
			m_nLastRtn = -1;

			return m_nLastRtn;
		}

		m_pCurFileDataStream = pTempDataStream;

		m_nLastRtn = m_pCurFileDataStream->WritePacket(type,packet,index_array,try_nums);

		return m_nLastRtn;
	}

	ACE_ASSERT(false);

	m_nLastRtn = -1;

	return m_nLastRtn;
}

IFileWriteDataStream * CMultiFileWriteStream::CreateNextFileStream()
{
	IFileWriteDataStream * pTempDataStream = m_pCurFileDataStream->CloneAfterIni();

	std::string strnext_filename;
	get_next_filename(m_strOrgFilename.c_str(),strnext_filename);

	m_pCurFileDataStream->CloseDataStream();
	CDataSourceHelper::DestroyWriteDataStream(m_pCurFileDataStream);
	m_pCurFileDataStream = NULL;

	if( !pTempDataStream )
		return NULL;

	pTempDataStream->SetFileName(strnext_filename.c_str());

	if( 0 != pTempDataStream->OpenDataStream() )
	{
		delete pTempDataStream;
		return NULL;
	}

	return pTempDataStream;
}

void CMultiFileWriteStream::get_next_filename(const char * org_filename,
											  std::string& strnext_filename)
{
	const char * psuf = CPF::GetSufFromFileName(org_filename);

	if( psuf )
	{
		strnext_filename.assign(org_filename,psuf-1-org_filename);
	}
	else
	{
		strnext_filename = org_filename;
	}

	char buf[MAX_PATH];

	if(psuf)
	{
		sprintf(buf,"%s_e%04d.%s",strnext_filename.c_str(),
			m_pWriteFileCtrl->get_finished_nums(),
			psuf);
	}
	else
	{
		sprintf(buf,"%s_e%04d",strnext_filename.c_str(),
			m_pWriteFileCtrl->get_finished_nums());
	}	

	strnext_filename = buf;

	return;
}

int CMultiFileWriteStream::GetConnectState()
{
	if( m_pCurFileDataStream )
	{
		return m_pCurFileDataStream->GetConnectState();
	}

	return 0;
}


bool CMultiFileWriteStream::GetDataStreamName(std::string& name)
{
	if( m_pCurFileDataStream )
	{
		return m_pCurFileDataStream->GetDataStreamName(name);
	}

	return false;
}