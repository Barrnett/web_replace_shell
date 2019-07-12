//////////////////////////////////////////////////////////////////////////
//DataSourceMgr.cpp

#include "PacketIO/DataSourceMgr.h"
#include "PacketIO/EtherrealDataSource.h"
#include "PacketIO/WinpCapDataSource.h"
#include "PacketIO/LoopFileDataSource.h"
#include "PacketIO/ZCCPTDataSource.h"
#include "PacketIO/DataSourceHelper.h"
#include "PacketIO/MultiFileDataSource.h"
#include "PacketIO/MemDataSource.h"
#include "PacketIO/EtherioDataSource.h"
#include "DataSourceParamReader.h"
#include "PacketIO/Pagefile_Memory_Pool_DataSource.h"

CDataSourceMgr::CDataSourceMgr(void)
{
	m_pRecvDataSource = NULL;
	m_pLogInstance = NULL;

	m_pDataSourceParamReader = NULL;

}

CDataSourceMgr::~CDataSourceMgr(void)
{
	close();
}

BOOL CDataSourceMgr::init(const char * filename,int type,ACE_Log_Msg * pLogInstance)
{	
	close();

	if( type == 0 )
	{
		m_pDataSourceParamReader = new C_Ini_DataSourceParamReader;
	}
	else
	{
		//目前不支持
		ACE_ASSERT(FALSE);
		return false;
	}

	if( !m_pDataSourceParamReader )
	{
		return false;
	}

	if( !m_pDataSourceParamReader->Open(filename) )
	{
		delete m_pDataSourceParamReader;
		m_pDataSourceParamReader = NULL;

		return false;
	}

	if( !m_pDataSourceParamReader->Read(m_datasource_paraminfo) )
	{
		delete m_pDataSourceParamReader;
		m_pDataSourceParamReader = NULL;

		return false;
	}

	m_pLogInstance = pLogInstance;


	return TRUE;
}

IRecvDataSource * CDataSourceMgr::OpenRecvDataSource()
{
	CloseRecvDataSource();

	switch( m_datasource_paraminfo.m_drvType )
	{
#ifdef OS_WINDOWS
	case DRIVER_TYPE_WINPCAP_NET:
	case DRIVER_TYPE_WINPCAP_TAP:
		m_pRecvDataSource = OpenWinpCapDataSource(m_datasource_paraminfo.m_drvType);
		break;

#endif//OS_WINDOWS

	case DRIVER_TYPE_ETHERIO:
		m_pRecvDataSource = OpenEtherioDataSource(m_datasource_paraminfo.m_drvType);
		break;

	case DRIVER_TYPE_FILE_ETHEREAL:
	case DRIVER_TYPE_FILE_ZCCPT:
		m_pRecvDataSource = OpenMultiFilePlaybackDataSource(m_datasource_paraminfo.m_drvType);
		break;

	case DRIVER_TYPE_MEMORY:
		m_pRecvDataSource = OpenMemoryDataSource(m_datasource_paraminfo.m_drvType);
		break;

	case DRIVER_TYPE_Pagefile_Memory_Pool:
		m_pRecvDataSource = OpenShareMemDataSource(m_datasource_paraminfo.m_drvType);
		break;


	default:
		{
			m_pRecvDataSource = NULL;
			ACE_ASSERT(FALSE);
		}
		break;
	}

	return m_pRecvDataSource;
}

void CDataSourceMgr::CloseRecvDataSource()
{
	if( m_pRecvDataSource )
	{
		m_pRecvDataSource->CloseDataSource();
		CDataSourceHelper::DestroyRecvDataSource(m_pRecvDataSource);		
		m_pRecvDataSource = NULL;
	}
}

void CDataSourceMgr::close()
{
	CloseRecvDataSource();

	if( m_pDataSourceParamReader )
	{
		delete m_pDataSourceParamReader;
		m_pDataSourceParamReader = NULL;
	}

	return;
}

void CDataSourceMgr::writefile(const char * filename)
{
	if( m_pDataSourceParamReader )
	{
		m_pDataSourceParamReader->Write(m_datasource_paraminfo,filename);
	}

	return;

}

IRecvDataSource * CDataSourceMgr::OpenMultiFilePlaybackDataSource(DRIVER_TYPE type)
{
	Playback_Param_Info * pCommonPlayback_Param = NULL;

	if( type == DRIVER_TYPE_FILE_ZCCPT )
	{
		pCommonPlayback_Param = &m_datasource_paraminfo.m_cpt_playback_param.m_playback_info;
	}
	else
	{
//#ifdef OS_WINDOWS
		pCommonPlayback_Param = &m_datasource_paraminfo.m_ethreal_playback_param.m_playback_info;
//#endif//OS_WINDOWS
	}

	if( pCommonPlayback_Param->m_vtFileName.size() == 0 )
	{
		MY_ACE_DEBUG(m_pLogInstance,(LM_ERROR,ACE_TEXT("[Err][%D]can not get filename in ini_file\n")));

		return NULL;
	}

	CTimeStamp32 adjusttime;
	int adjusttype = pCommonPlayback_Param->GetAdjustBeginTime(adjusttime);

	CMultiFileDataSource * pMultiDataSource = new CMultiFileDataSource;

	bool skip_error_file = (pCommonPlayback_Param->m_skip_error_file==1);
	bool re_calculate_time = (pCommonPlayback_Param->m_re_calculate_time==1);

	if( adjusttype == 1 )
	{//如果调整为当前时间，则不需要计算时间
		re_calculate_time = false;
	}

	if( type == DRIVER_TYPE_FILE_ZCCPT )
	{
		if( !pMultiDataSource->Open_CptFiles(
			pCommonPlayback_Param->m_vtFileName,
			m_datasource_paraminfo.m_cpt_playback_param.m_nReadNums,
			m_datasource_paraminfo.m_cpt_playback_param.m_nBufferSizeMB,
			re_calculate_time,
			false,
			skip_error_file) )
		{
			delete pMultiDataSource;

			return  NULL;
		}
	}
	else
	{
		if( !pMultiDataSource->Open_EthrealFiles(
			pCommonPlayback_Param->m_vtFileName,
			re_calculate_time,
			false,
			skip_error_file) )
		{
			delete pMultiDataSource;

			return  NULL;
		}
	}

	return CDataSourceHelper::CreateRecvDataSource(
		pMultiDataSource,
		m_datasource_paraminfo.m_nbps,
		m_datasource_paraminfo.m_unit_nums,
		adjusttype,&adjusttime,
		m_datasource_paraminfo.m_bCopyPkt,
		m_datasource_paraminfo.m_nDiscartRate,
		m_datasource_paraminfo.m_bEnableTimerPacket,
		pCommonPlayback_Param->m_nloopnums,
		m_datasource_paraminfo.m_nThreadBufferSize*1024*1024,
		1);

}

IRecvDataSource * CDataSourceMgr::OpenMemoryDataSource(DRIVER_TYPE type)
{
	CMemDataSource * pMemSource = 
		(CMemDataSource *)CDataSourceHelper::CreateOrignRecvDataSource(type);

	if( !pMemSource->Open(m_datasource_paraminfo.m_memParamInfo.m_minlen,
		m_datasource_paraminfo.m_memParamInfo.m_maxlen,
		m_datasource_paraminfo.m_memParamInfo.m_wProto,
		m_datasource_paraminfo.m_memParamInfo.m_pModelData,
		m_datasource_paraminfo.m_memParamInfo.m_nModelDataLen) )
	{
		CDataSourceHelper::DestroyRecvDataSource(pMemSource);

		return  NULL;
	}

	return CDataSourceHelper::CreateRecvDataSource(
		pMemSource,
		m_datasource_paraminfo.m_nbps,
		m_datasource_paraminfo.m_unit_nums,
		0,NULL,
		m_datasource_paraminfo.m_bCopyPkt,
		m_datasource_paraminfo.m_nDiscartRate,
		m_datasource_paraminfo.m_bEnableTimerPacket,
		1,
		m_datasource_paraminfo.m_nThreadBufferSize*1024*1024,
		1);
}

IRecvDataSource * CDataSourceMgr::OpenShareMemDataSource(DRIVER_TYPE type)
{
	CPagefile_Memory_Pool_DataSource * pShareMemSource = 
		(CPagefile_Memory_Pool_DataSource *)CDataSourceHelper::CreateOrignRecvDataSource(type);

	if( !pShareMemSource->Open(
		m_datasource_paraminfo.m_ShareMemParamInfo.m_strShareName.c_str(),
		m_datasource_paraminfo.m_ShareMemParamInfo.m_mem_size_mb*1024*1024 ) )
	{
		CDataSourceHelper::DestroyRecvDataSource(pShareMemSource);

		return  NULL;
	}

	return CDataSourceHelper::CreateRecvDataSource(
		pShareMemSource,
		m_datasource_paraminfo.m_nbps,
		m_datasource_paraminfo.m_unit_nums,
		0,NULL,
		m_datasource_paraminfo.m_bCopyPkt,
		m_datasource_paraminfo.m_nDiscartRate,
		m_datasource_paraminfo.m_bEnableTimerPacket,
		1,
		m_datasource_paraminfo.m_nThreadBufferSize*1024*1024,
		1);
}

#ifdef OS_WINDOWS
IRecvDataSource * CDataSourceMgr::OpenWinpCapDataSource(DRIVER_TYPE type)
{
	CWinpCapDataSource * pWinpCap = 
		(CWinpCapDataSource *)CDataSourceHelper::CreateOrignRecvDataSource(type);

	if(pWinpCap->Open(m_datasource_paraminfo.m_winpcap_drv_param)==FALSE)
	{
		MY_ACE_DEBUG(m_pLogInstance,
			(LM_ERROR,ACE_TEXT("[Err][%D]open winpcap drv type = %d,index=%d  failed\n"),
			m_datasource_paraminfo.m_drvType,
			m_datasource_paraminfo.m_winpcap_drv_param.cardIndex));

		CDataSourceHelper::DestroyRecvDataSource(pWinpCap);

		//DUMPSTATE("加载编号为%d的适配器出错!\r\n", open);
		return  NULL;
	}

	return CDataSourceHelper::CreateRecvDataSource(
		pWinpCap,
		m_datasource_paraminfo.m_nbps,
		m_datasource_paraminfo.m_unit_nums,
		0,NULL,
		m_datasource_paraminfo.m_bCopyPkt,
		m_datasource_paraminfo.m_nDiscartRate,
		m_datasource_paraminfo.m_bEnableTimerPacket,
		1,
		m_datasource_paraminfo.m_nThreadBufferSize*1024*1024,
		1);
}

#endif//OS_WINDOWS


IRecvDataSource * CDataSourceMgr::OpenEtherioDataSource(DRIVER_TYPE type)
{
	CEtherioDataSource * pEtherioDatasource
		= (CEtherioDataSource *)CDataSourceHelper::CreateOrignRecvDataSource(type);

	int error_code = 0;

	if( !pEtherioDatasource->Open(
		m_datasource_paraminfo.m_etherioParamInfo.m_drv_load_type,
		m_datasource_paraminfo.m_etherioParamInfo.m_strSysFileName.c_str(),
		m_datasource_paraminfo.m_etherioParamInfo.m_strDriverName.c_str(),
		m_datasource_paraminfo.m_etherioParamInfo.m_strMacAddr.c_str(),
		m_datasource_paraminfo.m_etherioParamInfo.m_nMemSize,
		m_datasource_paraminfo.m_etherioParamInfo.m_bEtherioEnableTimerPacket,
		m_datasource_paraminfo.m_etherioParamInfo.m_nTimeStampType,
		m_datasource_paraminfo.m_etherioParamInfo.m_bErrorRecover,&error_code) )
	{
		CDataSourceHelper::DestroyRecvDataSource(pEtherioDatasource);

		return  NULL;
	}

	return CDataSourceHelper::CreateRecvDataSource(
		pEtherioDatasource,
		m_datasource_paraminfo.m_nbps,
		m_datasource_paraminfo.m_unit_nums,
		0,NULL,
		m_datasource_paraminfo.m_bCopyPkt,
		m_datasource_paraminfo.m_nDiscartRate,
		m_datasource_paraminfo.m_bEnableTimerPacket,
		1,
		m_datasource_paraminfo.m_nThreadBufferSize*1024*1024,
		1);
}


//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
int CDataSourceMgr::GetConnectState()
{
	if( m_pRecvDataSource )
	{
		return m_pRecvDataSource->GetConnectState();
	}

	return 0;
}


