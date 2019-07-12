//////////////////////////////////////////////////////////////////////////
//DataSourceHelper.cpp

#include "PacketIO/DataSourceHelper.h"

#ifdef OS_WINDOWS
#	include "PacketIO/WinpCapDataSource.h"
#	include "PacketIO/WinpCapSendPacket.h"
#endif//OS_WINDOWS
#include "PacketIO/EtherrealDataSource.h"
#include "PacketIO/EtherealWritefile.h"
#include "PacketIO/EtherioSendPacket.h"
#include "PacketIO/EtherioDataSource.h"

#include "PacketIO/VPKHelpDataSource.h"

#include "PacketIO/ZCCPTDataSource.h"
#include "PacketIO/CPTWriteDataStream.h"
#include "PacketIO/MemDataSource.h"

#include "PacketIO/BufferPktThreadDataSource.h"
#include "PacketIO/FuncExtDataSource.h"
#include "PacketIO/SpeedCtrlDataSource.h"
#include "PacketIO/Pagefile_Memory_Pool_DataStream.h"
#include "PacketIO/Pagefile_Memory_Pool_DataSource.h"

CDataSourceHelper::CDataSourceHelper(void)
{
}

CDataSourceHelper::~CDataSourceHelper(void)
{
}


IRecvDataSource * CDataSourceHelper::CreateOrignRecvDataSource(DRIVER_TYPE type)
{
	IRealDataSource * pRealRecvDataSource = NULL;

	switch(type)
	{
#ifdef OS_WINDOWS
	case DRIVER_TYPE_WINPCAP_NET:
		pRealRecvDataSource = new CSingleCardWinpCapDataSource;
		break;

	case DRIVER_TYPE_WINPCAP_TAP:
		pRealRecvDataSource = new CDoubleCardWinpCapDataSource;
		break;

#endif
	case DRIVER_TYPE_FILE_ETHEREAL:
		pRealRecvDataSource = new CEtherrealDataSource;
		break;

	case DRIVER_TYPE_ETHERIO:
		pRealRecvDataSource = new CEtherioDataSource;
		break;

	case DRIVER_TYPE_FILE_ZCCPT:
		pRealRecvDataSource = new CZCCPTDataSource;
		break;

	case DRIVER_TYPE_MEMORY:
		pRealRecvDataSource = new CMemDataSource;
		break;

	case DRIVER_TYPE_Pagefile_Memory_Pool:
		pRealRecvDataSource = new CPagefile_Memory_Pool_DataSource;
		break;


	default:
		ACE_ASSERT(FALSE);
		break;
	}

	if( pRealRecvDataSource )
	{
		pRealRecvDataSource->SetDrvType(type);
	}

	return pRealRecvDataSource;
}


IRecvDataSource * CDataSourceHelper::CreateRecvDataSource(DRIVER_TYPE type,
														  ACE_UINT64 speed,unsigned int uint_nums,
														  int adjusttype,const CTimeStamp32 * pAdjustTimeStamp,
														  bool bcopypkt,
														  unsigned int discard_rate,
														  bool bvpk,
														  size_t nloopnums,
														  unsigned int threadbuffersize)
{
	IRecvDataSource * pRealRecvDataSource = CreateOrignRecvDataSource(type);

	return CreateRecvDataSource(pRealRecvDataSource,
		speed,uint_nums,
		adjusttype,pAdjustTimeStamp,
		bcopypkt,discard_rate,
		bvpk,nloopnums,threadbuffersize,1);

}

IRecvDataSource * CDataSourceHelper::CreateRecvDataSource(IRecvDataSource * pRealDataSource,
														  ACE_UINT64 speed,unsigned int uint_nums,
														  int adjusttype,const CTimeStamp32 * pAdjustTimeStamp,
														  bool bcopypkt,
														  unsigned int discard_rate,
														  bool bvpk,
														  size_t nloopnums,
														  unsigned int threadbuffersize,
														  int delete_real_source)
{
	if( pRealDataSource == NULL )
	{
		ACE_ASSERT(false);
		return NULL;
	}

	IRecvDataSource * pCurDatasource = pRealDataSource;

	{
		if( speed > 0 && speed != (ACE_UINT64)-1 )
		{
			CSpeedCtrlDataSource * pSpeedCtrlDataSource = new CSpeedCtrlDataSource;

			if( !delete_real_source && (pCurDatasource == pRealDataSource) )
			{
				pSpeedCtrlDataSource->init(pCurDatasource,0,speed,uint_nums);
			}
			else
			{
				pSpeedCtrlDataSource->init(pCurDatasource,1,speed,uint_nums);
			}

			pCurDatasource = pSpeedCtrlDataSource;
		}
	}

	{
		if( nloopnums != 1 
			|| adjusttype != 0 
			|| bcopypkt	
			|| discard_rate )
		{
			CFuncExtDataSource * pFuncDataSource = new CFuncExtDataSource;

			if( !delete_real_source && (pCurDatasource == pRealDataSource) )
			{
				pFuncDataSource->init(pCurDatasource,0,
					adjusttype,pAdjustTimeStamp,
					bcopypkt,discard_rate,
					nloopnums,1);
			}
			else
			{
				pFuncDataSource->init(pCurDatasource,1,
					adjusttype,pAdjustTimeStamp,
					bcopypkt,discard_rate,
					nloopnums,1);
			}

			pCurDatasource = pFuncDataSource;
		}
	}

	{
		if( bvpk )
		{
			if( !pRealDataSource->IsEnableTimerPacket() )
			{
				CVPKHelpDataSource * pvpkDataSource = new CVPKHelpDataSource;

				if( !delete_real_source && (pCurDatasource == pRealDataSource) )
					pvpkDataSource->init(pCurDatasource,0);
				else
					pvpkDataSource->init(pCurDatasource,1);

				pCurDatasource = pvpkDataSource;
			}
		}
		else
		{
			pRealDataSource->EnableTimerPacket(false);
		}
	}

	{
		if( threadbuffersize > 0 )
		{
			CBufferPktThreadDataSource * pBufferThreadDataSource = new CBufferPktThreadDataSource;

			if( !delete_real_source && (pCurDatasource == pRealDataSource) )
				pBufferThreadDataSource->init(pCurDatasource,0,threadbuffersize,ACE_Time_Value(0,0));
			else
				pBufferThreadDataSource->init(pCurDatasource,1,threadbuffersize,ACE_Time_Value(0,0));

			pCurDatasource = pBufferThreadDataSource;
		}
	}

	return pCurDatasource;

}


void CDataSourceHelper::DestroyRecvDataSource(IRecvDataSource * pDataSource)
{
	if( pDataSource )
	{
		delete pDataSource;
	}

	return;
}


IWriteDataStream * CDataSourceHelper::CreateWriteDataStream(DRIVER_TYPE type)
{
	IWriteDataStream * pWriteDataStream = NULL;

	switch(type)
	{
#ifdef OS_WINDOWS
	case DRIVER_TYPE_WINPCAP_NET:
		pWriteDataStream = new CWinpcapWriteDataStream;
		break;
#endif//OS_WINDOWS


	case DRIVER_TYPE_ETHERIO:
		pWriteDataStream = new CEtherioWriteDataStream;
		break;

	case DRIVER_TYPE_WINPCAP_TAP:
		return NULL;
		break;

	case DRIVER_TYPE_FILE_ETHEREAL:
		pWriteDataStream = new CEtherealWriteDataStream;
		break;

	case DRIVER_TYPE_FILE_ZCCPT:
		pWriteDataStream = new CCPTWriteDataStream;
		break;

	case DRIVER_TYPE_Pagefile_Memory_Pool:
		pWriteDataStream = new CPagefile_Memory_Pool_DataStream;
		break;

	default:
		ACE_ASSERT(FALSE);
		return NULL;
		break;
	}

	return pWriteDataStream;
}

void CDataSourceHelper::DestroyWriteDataStream(IWriteDataStream * pWriteDataStream)
{
	if( pWriteDataStream )
	{
		delete pWriteDataStream;
	}

	return;
}

