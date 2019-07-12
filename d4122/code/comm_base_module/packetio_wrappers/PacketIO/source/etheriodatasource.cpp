//////////////////////////////////////////////////////////////////////////
//EtherioDataSource.cpp

#include "cpf/ostypedef.h"


#include "PacketIO/EtherioDataSource.h"
#include "./etherio/EtherioDllExport.h"
#include "cpf/path_tools.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/Common_Func_Macor.h"

CEtherioDataSource::CEtherioDataSource(void)
{
}

CEtherioDataSource::~CEtherioDataSource(void)
{
}


BOOL CEtherioDataSource::init(int drv_load_type,const char * sysfilename,const char *drivername,
							  const char * macaddr,unsigned int memsize,
							  BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
							  int * error_code)
{
	if( macaddr )
	{
		m_strMacaddrInfo = macaddr;
	}

	m_bEnableTimerPacket = bEnableTimer;
	m_nTimeStampType = nTimeStampType;
	m_bErrorRecover = bErrorRecover;

	GetSysAndDriverName(sysfilename,drivername);

	m_nMemSize = memsize;

	if( m_nMemSize == 0 )
	{
		m_nMemSize = 20;
	}

	memset(&m_packet_header,0x00,sizeof(m_packet_header));
	m_packet_header.btHeaderLength = sizeof(m_packet_header);
	m_packet_header.wdProto = 2;
	m_packet_header.dwError = 0;
	m_packet_header.btVersion = 1;

	BOOL bStaticLoad = (drv_load_type != 0);

	m_nDevCount = EtherIO_InitializeEx(m_strSysFileName.c_str(),m_strDriverName.c_str(), bStaticLoad);

	if( m_nDevCount <= 0 )
	{
		if( error_code )
		{
			*error_code = m_nDevCount;
		}

		return false;
	}
	else
	{
		if( error_code )
		{
			*error_code = 0;
		}
	}

	if( m_strMacaddrInfo.empty() )
	{
		m_indexarray = -1;
	}
	else
	{
		m_indexarray = EtherIO_GetMultiIndexByShowNameOrMAC(m_strMacaddrInfo.c_str());
	}	

	return (m_indexarray != 0);
}


BOOL CEtherioDataSource::init(int drv_load_type,const char * sysfilename,const char *drivername,int macindex_array,
		  unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		  int * error_code)
{
	m_indexarray = macindex_array;

	m_bEnableTimerPacket = bEnableTimer;
	m_nTimeStampType = nTimeStampType;
	m_bErrorRecover = bErrorRecover;

	GetSysAndDriverName(sysfilename,drivername);

	m_nMemSize = memsize;

	if( m_nMemSize == 0 )
	{
		m_nMemSize = 20;
	}

	memset(&m_packet_header,0x00,sizeof(m_packet_header));
	m_packet_header.btHeaderLength = sizeof(m_packet_header);
	m_packet_header.wdProto = 2;
	m_packet_header.dwError = 0;
	m_packet_header.btVersion = 1;

	// 根据名字判断
	BOOL bStaticLoad = (drv_load_type != 0);

	m_nDevCount = EtherIO_InitializeEx(m_strSysFileName.c_str(),m_strDriverName.c_str(), bStaticLoad);

	if( m_nDevCount <= 0 )
	{
		if( error_code )
		{
			*error_code = m_nDevCount;
		}

		return false;
	}
	else
	{
		if( error_code )
		{
			*error_code = 0;
		}
	}

	char mac_text[64*32] = {0};

	if( m_indexarray != -1 )
	{	
		//EtherIO_GetMacStringByMultiIndex(m_indexarray,mac_text,'-',true);
		EtherIO_GetShowNameByMultiIndex(m_indexarray,mac_text);

		m_strMacaddrInfo = mac_text;
	}

	return true;
}




void CEtherioDataSource::fini()
{
	EtherIO_UnInitialize();
	m_nDevCount = 0;
}

int CEtherioDataSource::OpenDataSource()
{
	if( m_nDevCount <= 0 )
		return -1;

	//m_dwRecvHandle = EtherIO_CreateReceiveObj(m_indexarray,m_nMemSize*1024*1024, 0); //末尾加参数0，是函数不等待，立即返回
	m_dwRecvHandle = EtherIO_CreateReceiveObj(m_indexarray,m_nMemSize*1024*1024);//阻塞

	if( m_dwRecvHandle == 0 )
		return -2;

	return 0;
}


void CEtherioDataSource::CloseDataSource()
{
	if( m_dwRecvHandle != 0 )
	{
		EtherIO_ReleaseReceiveObj(m_dwRecvHandle);

		m_dwRecvHandle = 0;
	}

	return;
}

BOOL CEtherioDataSource::StartToRecv()
{
	if( m_dwRecvHandle == 0 )
	{
//		ACE_ASSERT(false);
		return false;
	}

	return (BOOL)EtherIO_StartReceive(m_dwRecvHandle,m_bEnableTimerPacket,(EN_TIME_MODE)m_nTimeStampType);
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CEtherioDataSource::StopRecving()
{
	if( m_dwRecvHandle != 0)
	{
		EtherIO_StopReceive(m_dwRecvHandle);
	}

}

//接收数据
PACKET_LEN_TYPE CEtherioDataSource::GetPacket(RECV_PACKET& packet)
{
	if( m_dwRecvHandle == 0 )
		return PACKET_EMPTY;

	unsigned char * pPacket = NULL;

	DWORD		car_index = 0;
	DWORD		dwOriginalLen = 0;
	EIO_TIME	stamp;

	int iPketLen = EtherIO_GetNextPacket(m_dwRecvHandle,&pPacket,&stamp,&car_index,&dwOriginalLen);

	m_packet_header.stamp.sec = stamp.sec;
	m_packet_header.stamp.ns = stamp.ns;

	if( iPketLen > 0 )
	{
		packet.nCaplen = iPketLen;
		packet.nPktlen = (int)dwOriginalLen;

		packet.nOffsetBit = packet.nReserved = 0;
		packet.nPktAddiBitLen = packet.nCapAddiBitLen = 0;

		packet.pPacket = pPacket;

		ACE_ASSERT(IS_ALLIGN_PTR(pPacket));

		m_packet_header.btCardNo = (BYTE)(car_index&0xff);
		
		packet.pHeaderinfo = &m_packet_header;

		return PACKET_OK;
	}
	else if ( -2 == iPketLen )
	{
		packet.pHeaderinfo = &m_packet_header;

		packet.nPktlen = packet.nCaplen = 0;

		packet.nOffsetBit = packet.nReserved = 0;
		packet.nPktAddiBitLen = packet.nCapAddiBitLen = 0;

		return PACKET_TIMER;
	}
	else if( 0 == iPketLen )
	{
		return PACKET_EMPTY;
	}
	else
	{
		if( m_bErrorRecover )
		{
			StopRecving();

			if( !StartToRecv() )
			{
				return PACKET_DRV_ERROR;
			}
			else
			{
				return PACKET_CALL_NEXT;
			}
		}
		else
		{
			return PACKET_DRV_ERROR;
		}
	}

	ACE_ASSERT(FALSE);

	return PACKET_CALL_NEXT;
}

//得到因为上层处理来不及，底层丢包的数目,-1表示无效
void CEtherioDataSource::GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
{
	if( m_dwRecvHandle )
	{
		DRIVER_STSS_INFO	drv_stss_info;

		EtherIO_GetRecvStss(m_dwRecvHandle,&drv_stss_info);

		bytes = drv_stss_info.lostBytes;
		pkts = drv_stss_info.lostFrames;
	}
	else
	{
		pkts = bytes = -1;
	}

	return;
}

//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
int CEtherioDataSource::GetConnectState()
{
	if( m_strDriverName.empty() 
		|| m_dwRecvHandle == 0 
		|| m_indexarray == 0 )
	{
		return 0;
	}

	for( int i = 0; i < mymin(32,m_nDevCount); ++i)
	{
		if( m_indexarray & (1<<i) )
		{
			int nrtn = EtherIO_GetDeviceLinkSpeed(i);

			if( -1 == nrtn )
			{
				continue;
			}

			if( 0 == nrtn )
			{
				return 0;
			}
		}
	}

	return 1;
}


const char* CEtherioDataSource::GetConstDriverName()
{
	return "etherio.sys";
}
ACE_UINT32 CEtherioDataSource::GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen)
{
	if( m_dwRecvHandle == 0 )
	{
		if( pTotalLen )
		{
			*pTotalLen = 0;
		}

		return 0;
	}

	return EtherIO_GetRecvBufferUsed(m_dwRecvHandle,(DWORD *)pTotalLen);
}

int CEtherioDataSource::WaitForPacket(const ACE_Time_Value& sleep_time)
{
	if( m_dwRecvHandle )
	{
		return EtherIO_WaitForRecvPacket(m_dwRecvHandle,sleep_time.get_msec());
	}

	return -1;
}


//#endif//OS_WINDOWS

