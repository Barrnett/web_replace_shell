//////////////////////////////////////////////////////////////////////////
//DataSendMgr.cpp

#include "PacketIO/DataSendMgr.h"
#include "PacketIO/DataSourceHelper.h"
#include "PacketIO/WinpCapSendPacket.h"
#include "PacketIO/EtherioSendPacket.h"
#include "PacketIO/EtherealWritefile.h"
#include "PacketIO/CPTWriteDataStream.h"
#include "PacketIO/MultiFileWriteStream.h"
#include "PacketIO/Pagefile_Memory_Pool_DataStream.h"

#include "DataSendParamReader.h"
#include "cpf/path_tools.h"


CDataSendMgr::CDataSendMgr(void)
{
	m_pLogInstance = NULL;
	m_pWriteDataSteam = NULL;
	m_pDataSendParamReader = NULL;
}

CDataSendMgr::~CDataSendMgr(void)
{
	close();
}

BOOL CDataSendMgr::init(const char * filename,int type,ACE_Log_Msg * pLogInstance)
{	
	close();

	if( type == 0 )
	{
		m_pDataSendParamReader = new C_Ini_DataSendParamReader;

	}
	else
	{
		ACE_ASSERT(false);
		return false;
	}

	if( !m_pDataSendParamReader )
		return false;

	if( !m_pDataSendParamReader->Open(filename) )
	{
		delete m_pDataSendParamReader;
		m_pDataSendParamReader = NULL;
		return FALSE;
	}

	if( !m_pDataSendParamReader->Read(m_datasend_param_info) )
	{
		delete m_pDataSendParamReader;
		m_pDataSendParamReader = NULL;
		return FALSE;
	}

	m_pLogInstance = pLogInstance;


	return TRUE;
}

IWriteDataStream * CDataSendMgr::OpenWriteDataStream()
{
	CloseWriteDataStream();

	switch( m_datasend_param_info.m_drvType )
	{
#ifdef OS_WINDOWS
	case DRIVER_TYPE_WINPCAP_NET:
		m_pWriteDataSteam = OpenSendWinpCapDataStream(m_datasend_param_info.m_drvType);
		break;
#endif//OS_WINDOWS

	case DRIVER_TYPE_ETHERIO:
		m_pWriteDataSteam = OpenSendEtherioDataStream(m_datasend_param_info.m_drvType);
		break;

	case DRIVER_TYPE_FILE_ETHEREAL:
		m_pWriteDataSteam = OpenEtherealDataStream(m_datasend_param_info.m_drvType);
		break;

	case DRIVER_TYPE_FILE_ZCCPT:
		m_pWriteDataSteam = OpenCptDataStream(m_datasend_param_info.m_drvType);
		break;

	case DRIVER_TYPE_Pagefile_Memory_Pool:
		m_pWriteDataSteam = OpenShareMemDataStream(m_datasend_param_info.m_drvType);
		break;

	case DRIVER_TYPE_NONE:
		m_pWriteDataSteam = NULL;
		break;

	case DRIVER_TYPE_MEMORY:
	case DRIVER_TYPE_WINPCAP_TAP:
		{
			m_pWriteDataSteam = NULL;
			ACE_ASSERT(FALSE);
		}
		break;

	default:
		{
			m_pWriteDataSteam = NULL;
			ACE_ASSERT(FALSE);
		}
		break;
	}

	return m_pWriteDataSteam;
}


void CDataSendMgr::CloseWriteDataStream()
{
	if( m_pWriteDataSteam )
	{
		m_pWriteDataSteam->CloseDataStream();
		CDataSourceHelper::DestroyWriteDataStream(m_pWriteDataSteam);
		m_pWriteDataSteam = NULL;
	}
}

void CDataSendMgr::close()
{
	CloseWriteDataStream();

	if( m_pDataSendParamReader )
	{
		delete m_pDataSendParamReader;
		m_pDataSendParamReader = NULL;
	}

	return;
}

void CDataSendMgr::writefile(const char * filename)
{
	if( m_pDataSendParamReader )
	{
		m_pDataSendParamReader->Write(m_datasend_param_info,filename);
	}
	
	return;

}

#ifdef OS_WINDOWS
IWriteDataStream * CDataSendMgr::OpenSendWinpCapDataStream(DRIVER_TYPE drvtype)
{
	CWinpcapWriteDataStream * pWriteDataStream = 
		(CWinpcapWriteDataStream *)CDataSourceHelper::CreateWriteDataStream(drvtype);

	if( pWriteDataStream )
	{
		if( !pWriteDataStream->Open(m_datasend_param_info.m_winpcap_drv_param.cardIndex) )
		{
			CDataSourceHelper::DestroyWriteDataStream(pWriteDataStream);
			pWriteDataStream = NULL;
		}
	}

	return pWriteDataStream;
}

#endif//OS_WINDOWS

IWriteDataStream * CDataSendMgr::OpenSendEtherioDataStream(DRIVER_TYPE drvtype)
{
	CEtherioWriteDataStream * pWriteDataStream = 
		(CEtherioWriteDataStream *)CDataSourceHelper::CreateWriteDataStream(drvtype);

	if( pWriteDataStream )
	{
		if( !pWriteDataStream->Open(
			m_datasend_param_info.m_etherioParamInfo.m_drv_load_type,
			m_datasend_param_info.m_etherioParamInfo.m_copy_data_to_send,
			m_datasend_param_info.m_etherioParamInfo.m_strSysFileName.c_str(),
			m_datasend_param_info.m_etherioParamInfo.m_strDriverName.c_str(),
			m_datasend_param_info.m_etherioParamInfo.m_strMacAddr.c_str(),
			m_datasend_param_info.m_etherioParamInfo.m_sendtype,
			m_datasend_param_info.m_etherioParamInfo.m_nMemSize) )
		{
			CDataSourceHelper::DestroyWriteDataStream(pWriteDataStream);
			pWriteDataStream = NULL;
		}
	}

	return pWriteDataStream;
}

IWriteDataStream * CDataSendMgr::OpenEtherealDataStream(DRIVER_TYPE drvtype)
{
	CEtherealWriteDataStream * pWriteDataStream = 
		(CEtherealWriteDataStream *)CDataSourceHelper::CreateWriteDataStream(drvtype);

	if( !pWriteDataStream )
		return NULL;

	CPF::CreateFullDirecory(m_datasend_param_info.m_ethereal_write_param.strPath.c_str());

	const char * fullname = CPF::JoinPathToPath(
		m_datasend_param_info.m_ethereal_write_param.strPath.c_str(),
		m_datasend_param_info.m_ethereal_write_param.strFileName.c_str());

	if( !pWriteDataStream->Open(fullname,WTAP_FILE_PCAP,WTAP_ENCAP_ETHERNET,2000) )
	{
		CDataSourceHelper::DestroyWriteDataStream(pWriteDataStream);
		pWriteDataStream = NULL;
		return NULL;
	}

	CMultiFileWriteStream * pMultiFileWriteStream = new CMultiFileWriteStream;

	if( !pMultiFileWriteStream->init(pWriteDataStream,
			m_datasend_param_info.m_ethereal_write_param.num_files,
			m_datasend_param_info.m_ethereal_write_param.one_filesize_mb,
			m_datasend_param_info.m_ethereal_write_param.captime*60) )
	{
		CDataSourceHelper::DestroyWriteDataStream(pWriteDataStream);
		delete pMultiFileWriteStream;
		pMultiFileWriteStream = NULL;

		return NULL;
	}
	
	return pMultiFileWriteStream;
}

IWriteDataStream * CDataSendMgr::OpenCptDataStream(DRIVER_TYPE drvtype)
{
	CCPTWriteDataStream * pWriteDataStream = 
		(CCPTWriteDataStream *)CDataSourceHelper::CreateWriteDataStream(drvtype);

	if( pWriteDataStream )
	{
		CPF::CreateFullDirecory(m_datasend_param_info.m_cpt_write_param.strPath.c_str());

		const char * fullname = CPF::JoinPathToPath(
			m_datasend_param_info.m_cpt_write_param.strPath.c_str(),
			m_datasend_param_info.m_cpt_write_param.strFileName.c_str());

		if( !pWriteDataStream->Open(fullname,
			(m_datasend_param_info.m_cpt_write_param.one_filesize_mb?m_datasend_param_info.m_cpt_write_param.one_filesize_mb:1024*4),
			m_datasend_param_info.m_cpt_write_param.buffer_size_mb*1024*1024,
			m_datasend_param_info.m_cpt_write_param.num_files,
			m_datasend_param_info.m_cpt_write_param.captime*60) )
		{
			CDataSourceHelper::DestroyWriteDataStream(pWriteDataStream);
			pWriteDataStream = NULL;
		}
	}

	return pWriteDataStream;
}

IWriteDataStream * CDataSendMgr::OpenShareMemDataStream(DRIVER_TYPE drvtype)
{
	CPagefile_Memory_Pool_DataStream * pWriteDataStream = 
		(CPagefile_Memory_Pool_DataStream *)CDataSourceHelper::CreateWriteDataStream(drvtype);

	if( pWriteDataStream )
	{
		if( !pWriteDataStream->Open(
			m_datasend_param_info.m_sharemem_write_param.m_strShareName.c_str(),
			m_datasend_param_info.m_sharemem_write_param.m_mem_size_mb*1024*1024
			) )
		{
			CDataSourceHelper::DestroyWriteDataStream(pWriteDataStream);
			pWriteDataStream = NULL;
		}
	}

	return pWriteDataStream;
}


//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
int CDataSendMgr::GetConnectState()
{
	if( m_pWriteDataSteam )
	{
		return m_pWriteDataSteam->GetConnectState();
	}

	return 0;
}

bool CDataSendMgr::GetDataStreamName(std::string& name)
{
	if( m_pWriteDataSteam )
	{
		return m_pWriteDataSteam->GetDataStreamName(name);
	}

	return false;
}
