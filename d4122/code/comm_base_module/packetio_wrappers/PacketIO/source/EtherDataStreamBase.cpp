//////////////////////////////////////////////////////////////////////////
// EtherDataStreamBase.cpp

#include "cpf/ostypedef.h"


#include "PacketIO/EtherDataStreamBase.h"
#include "cpf/TimeStamp32.h"
#include "cpf/path_tools.h"
#include "cpf/Common_Func_Macor.h"

//////////////////////////////////////////////////////////////////////////
//CEtherSendPacketBase
//////////////////////////////////////////////////////////////////////////

CEtherSendPacketBase::CEtherSendPacketBase(void)
:m_nNumsHandle(0),m_indexarray(0)
,m_nMTU(1518),m_send_type(1),m_nDevCount(0)
{
	memset(m_vtSendHandle,0x00,sizeof(m_vtSendHandle));
}

CEtherSendPacketBase::~CEtherSendPacketBase(void)
{
//	Close();
}


//////////////////////////////////////////////////////////////////////////
//CEtherDataStreamBase
//////////////////////////////////////////////////////////////////////////

CEtherDataStreamBase::CEtherDataStreamBase()
:m_nMemSize(20),m_indexarray(0)
{
	// 在子类的构造里边new
	m_pEtherSendPacket = NULL;

	m_drv_load_type = -1;

	m_copy_data_to_send = 1;
}

CEtherDataStreamBase::~CEtherDataStreamBase()
{
}

//相当于init+OpenDataStream
BOOL CEtherDataStreamBase::Open(int drv_load_type,unsigned int copy_data_to_send,
								const char * sysfilename,const char * drivername,
								const char * macaddr,unsigned int send_type,
								unsigned int memsize)
{
	if( init(drv_load_type,copy_data_to_send,sysfilename,drivername,macaddr,send_type,memsize) == FALSE )
		return false;

	if( 0 == OpenDataStream() )
		return TRUE;

	return FALSE;

}


//相当于init+OpenDataStream
BOOL CEtherDataStreamBase::Open(int drv_load_type,unsigned int copy_data_to_send,
								const char * sysfilename,const char * drivername,
								int macindex_array,unsigned int send_type,
								unsigned int memsize)
{
	if( init(drv_load_type,copy_data_to_send,sysfilename,drivername,macindex_array,send_type,memsize) == FALSE )
		return false;

	if( 0 == OpenDataStream() )
		return TRUE;

	return FALSE;

}


//相当于CloseDataStream+fini
void CEtherDataStreamBase::Close()
{
	if (m_pEtherSendPacket)
	{
		CloseDataStream();

		fini();

		delete m_pEtherSendPacket;
		m_pEtherSendPacket = NULL;
	}

	return;
}


BOOL CEtherDataStreamBase::init(int drv_load_type,unsigned int copy_data_to_send,
								const char * sysfilename,const char * drivername,
								const char * macaddr,unsigned int send_type,unsigned int memsize)
{
	if(macaddr)
	{
		m_strMacAddr = macaddr;
	}

	m_indexarray = 0;

	GetSysAndDriverName(sysfilename,drivername);

	m_drv_load_type = drv_load_type;
	m_nMemSize = memsize;
	m_send_type = send_type;
	m_copy_data_to_send = copy_data_to_send;

	return TRUE;
}

BOOL CEtherDataStreamBase::init(int drv_load_type,unsigned int copy_data_to_send,
								const char * sysfilename,const char * drivername,
								int macindex_array,unsigned int send_type,unsigned int memsize)
{
	m_indexarray = macindex_array;
	m_strMacAddr.clear();

	GetSysAndDriverName(sysfilename,drivername);

	m_drv_load_type = drv_load_type;
	m_nMemSize = memsize;
	m_send_type = send_type;
	m_copy_data_to_send = copy_data_to_send;

	return true;
}



void CEtherDataStreamBase::fini()
{
	return;
}

int CEtherDataStreamBase::OpenDataStream()
{
	if( m_indexarray == 0 )
	{
		if( m_pEtherSendPacket->Open(
			m_drv_load_type,
			m_copy_data_to_send,
			m_strSysFileName.c_str(),
			m_strDriverName.c_str(),
			m_strMacAddr.c_str(),
			m_send_type,
			m_nMemSize) )
		{
			return 0;
		}
	}
	else
	{
		if( m_pEtherSendPacket->Open(
			m_drv_load_type,
			m_copy_data_to_send,
			m_strSysFileName.c_str(),
			m_strDriverName.c_str(),
			m_indexarray,
			m_send_type,
			m_nMemSize) )
		{
			return 0;
		}
	}


	return -1;
}

//关闭设备
void CEtherDataStreamBase::CloseDataStream()
{
	m_pEtherSendPacket->Close();
}

BOOL CEtherDataStreamBase::StartToWrite()
{
	IWriteDataStream::StartToWrite();

	return m_pEtherSendPacket->StartToSend();

}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CEtherDataStreamBase::StopWriting(BOOL bStopNow)
{
	m_pEtherSendPacket->StopSending(bStopNow);
}

int CEtherDataStreamBase::WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums)
{
	if( type != PACKET_OK )
		return -1;

	if( TRUE == m_pEtherSendPacket->SendPacket(pdata,datalen,index_array,try_nums) )
		return 0;

	return -1;
}


ACE_UINT32 CEtherDataStreamBase::GetSendBufferUsed(OUT ACE_UINT32* pTotalLen)
{
	return m_pEtherSendPacket->GetSendBufferUsed(pTotalLen);
}


void CEtherDataStreamBase::GetSysAndDriverName(const char * sysfilename,const char *drivername)
{
	if( sysfilename&&strlen(sysfilename)>0 )
	{
		if( CPF::IsFullPathName(sysfilename) )
			m_strSysFileName = sysfilename;
		else
			m_strSysFileName = CPF::GetModuleFullFileName(sysfilename);
	}
	else
	{
		m_strSysFileName = CPF::GetModuleFullFileName(GetConstDriverName());
	}

	if(drivername&&strlen(drivername)>0)
	{
		m_strDriverName = drivername;
	}
	else
	{
		std::string tempfilename = CPF::GetFileNameFromFullName(m_strSysFileName.c_str());

		char * ptemp = (char *)strchr(tempfilename.c_str(),'.');

		if( ptemp )
			*ptemp = 0;

		m_strDriverName = tempfilename;		
	}

	return;
}

bool CEtherDataStreamBase::GetDataStreamName(std::string& name)
{
	char buf[256] = {0};

	if( m_indexarray == -1 )
	{
		sprintf(buf,"%s %s",m_strDriverName.c_str(),"all card(s)");
	}
	else
	{
		sprintf(buf,"%s %s",m_strDriverName.c_str(),m_strMacAddr.c_str());
	}

	name = buf;

	return true;
}
