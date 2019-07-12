//////////////////////////////////////////////////////////////////////////
//EtherioSendPacket.cpp

#include "cpf/ostypedef.h"


#include "PacketIO/EtherioSendPacket.h"
#include "cpf/TimeStamp32.h"
#include "cpf/path_tools.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/Common_Func_Macor.h"
#include "./etherio/EtherioDllExport.h"

//////////////////////////////////////////////////////////////////////////
//CEtherioSendPacket
//////////////////////////////////////////////////////////////////////////

CEtherioSendPacket::CEtherioSendPacket(void)
{

}

CEtherioSendPacket::~CEtherioSendPacket(void)
{
	Close();
}


BOOL CEtherioSendPacket::Open(int drv_load_type,unsigned int copy_data_to_send,
							  const char * sysfilename,const char * servername,
							  const char * macaddr,unsigned int send_type,unsigned int memsize)
{
	// 根据名字判断
	BOOL bStaticLoad = (drv_load_type != 0);

	m_nDevCount = EtherIO_InitializeEx(sysfilename,servername, bStaticLoad);

	if( m_nDevCount <= 0 )
		return FALSE;

	int indexarray = 0;

	if( strlen(macaddr) == 0 )
	{
		indexarray = -1;
	}
	else
	{
		indexarray = EtherIO_GetMultiIndexByShowNameOrMAC(macaddr);
	}

	if( ((int)indexarray) == 0 )
	{
		EtherIO_UnInitialize();
		return false;
	}

	return Inner_Open(m_nDevCount,copy_data_to_send,sysfilename,servername,indexarray,send_type,memsize);
}



BOOL CEtherioSendPacket::Open(int drv_load_type,unsigned int copy_data_to_send,
							  const char * sysfilename,const char * servername,
							  int macindex_array,unsigned int send_type,unsigned int memsize)
{
	// 根据名字判断
	BOOL bStaticLoad = (drv_load_type != 0);

	m_nDevCount = EtherIO_InitializeEx(sysfilename,servername, bStaticLoad);

	if( m_nDevCount <= 0 )
		return FALSE;

	if( ((int)macindex_array) == 0 )
	{
		EtherIO_UnInitialize();
		return false;
	}

	return Inner_Open(m_nDevCount,copy_data_to_send,sysfilename,servername,macindex_array,send_type,memsize);

}

BOOL CEtherioSendPacket::Inner_Open(int devcount,
									unsigned int copy_data_to_send,
									const char * sysfilename,const char * servername,
									int macindex_array,unsigned int send_type,unsigned int memsize)
{
	m_indexarray = macindex_array;

	m_nMTU = EtherIO_GetMaxFrameSize(0);

	if( m_nMTU <= 0 )
		m_nMTU = 1518;
	else
		m_nMTU += 4;//VLAN

	if( memsize == 0 )
		memsize = 20;

	m_send_type = send_type;


	for(int i = 0; i < GET_NUMS_OF_ARRAY(m_vtSendHandle) && i < devcount; ++i)
	{
		m_vtSendHandle[i] = 0;

		if( m_indexarray & (1<<i) )
		{
			SEND_CREATE_PARA	send_param;
			send_param.dwBufferLen = memsize*1024*1024;
			send_param.dwCardIndex = i;
			send_param.enSendMode = (EN_SEND_MODE)send_type;
			send_param.pUserAddress = NULL;
			send_param.dwDontLoopBack = true;
			send_param.bWaitPacketEvent = true;
			send_param.sendEventNotEmpty = NULL;

			ETHERIO_SEND_HANDLE handle = EtherIO_CreateSendObj(&send_param);

			if( handle )
			{
				m_vtSendHandle[i] = handle;
				++m_nNumsHandle;
			}
		}
	}

	if( 0 == m_nNumsHandle )
	{
		EtherIO_UnInitialize();

		return false;
	}

	return true;
}


void CEtherioSendPacket::Close()
{
	if( 0 != m_nNumsHandle )
	{
		for(size_t i = 0; i < GET_NUMS_OF_ARRAY(m_vtSendHandle); ++i)
		{
			if( m_vtSendHandle[i] )
			{
				EtherIO_ReleaseSendObj(m_vtSendHandle[i]);
				m_vtSendHandle[i] = 0;
			}			
		}

		m_nNumsHandle = 0;

		EtherIO_UnInitialize();
	}

	return;
}

//在Open之后，在发送以前先调用StartToSend
BOOL CEtherioSendPacket::StartToSend()
{
	for(size_t i = 0; i < GET_NUMS_OF_ARRAY(m_vtSendHandle); ++i)
	{
		if( m_vtSendHandle[i] )
		{
			BOOL bSendOK = EtherIO_StartSend(m_vtSendHandle[i]);

			if( !bSendOK )
			{
				return false;
			}
		}
	}

	return true;
}

//在发送结束后，Close之前调用StopSending
void CEtherioSendPacket::StopSending(BOOL bStopNow)
{
	if( 0 == m_nNumsHandle )
	{
		return;
	}

	if( !bStopNow )
	{
		DWORD total_len = 0;

		if( 1 == m_send_type )
		{
			while(1)
			{
				if( IsAllSendOver() )
				{//等待所有的数据被取出
					break;
				}
				else
				{
					ACE_OS::sleep(1);
				}
			}

		}

		//等待一段时间，网卡可以将所有的数据发送出去
		ACE_OS::sleep(2);

	}

	for(size_t i = 0; i < GET_NUMS_OF_ARRAY(m_vtSendHandle); ++i)
	{
		if( m_vtSendHandle[i] )
		{
			EtherIO_StopSend(m_vtSendHandle[i]);
		}
	}

	return;

}

BOOL CEtherioSendPacket::SendPacket(const char * pdata,unsigned int datalen,int index_array,int try_nums)
{
	if( m_nNumsHandle == 0 || index_array == 0 )
		return false;

	unsigned int send_len = datalen-4;

	if( (int)send_len < 0 )
		return false;

	//驱动底下的bug，不能小于60字节。如果驱动改好了，去掉mymax
	send_len = mymax(mymin((unsigned int)m_nMTU,send_len),60);

	index_array &= m_indexarray;
//	int aarray = index_array & m_indexarray;

	for(int i = 0; i < GET_NUMS_OF_ARRAY(m_vtSendHandle) && i < m_nDevCount; ++i)
	{
		if( m_vtSendHandle[i] && (index_array & (1<<i)) )
		{
			EtherIO_SendPacket(m_vtSendHandle[i],(unsigned char *)pdata,send_len,try_nums);
//printf("%d, %d, %d, --  %x\n", i, index_array, aarray, m_vtSendHandle[i]);	// debug
		}
	}

	return true;
}

BOOL CEtherioSendPacket::IsAllSendOver()
{
	DWORD total_len = 0;

	for(size_t i = 0; i < GET_NUMS_OF_ARRAY(m_vtSendHandle) && i < (size_t)m_nDevCount; ++i)
	{
		if( m_vtSendHandle[i] )
		{
			if( 0 != EtherIO_GetSendBufferUsed(m_vtSendHandle[i],&total_len) )
			{
				return false;
			}
		}
	}

	return true;
}

ACE_UINT32 CEtherioSendPacket::GetSendBufferUsed(OUT ACE_UINT32* pTotalLen)
{
	DWORD total_len = 0;
	DWORD used_len = 0;

	for(size_t i = 0; i < GET_NUMS_OF_ARRAY(m_vtSendHandle) && i < (size_t)m_nDevCount; ++i)
	{
		if( m_vtSendHandle[i] )
		{
			DWORD temp_total = 0;

			used_len += EtherIO_GetSendBufferUsed(m_vtSendHandle[i],&temp_total);

			total_len += temp_total;
		}
	}

	if( pTotalLen )
	{
		*pTotalLen = total_len;
	}

	return ACE_UINT32(used_len);


}

//////////////////////////////////////////////////////////////////////////
//CEtherioWriteDataStream
//////////////////////////////////////////////////////////////////////////

CEtherioWriteDataStream::CEtherioWriteDataStream()
{
	m_pEtherSendPacket = (CEtherSendPacketBase*	)new CEtherioSendPacket;
}

CEtherioWriteDataStream::~CEtherioWriteDataStream()
{
	Close();

}




//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
int CEtherioWriteDataStream::GetConnectState()
{
	int nIndex = EtherIO_GetOneIndexByMAC(m_strMacAddr.c_str());

	if( nIndex < 0 )
		return 0;

	int nSpeed = EtherIO_GetDeviceLinkSpeed(nIndex);

	if( nSpeed == 0 )
		return 0;

	if( nSpeed < 0 )
		return -1;

	return 1;
}


const char* CEtherioWriteDataStream::GetConstDriverName()
{
	return "etherio.sys";
}
