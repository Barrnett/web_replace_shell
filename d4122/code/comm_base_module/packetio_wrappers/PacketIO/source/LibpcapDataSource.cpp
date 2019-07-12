////////////////////////////////////////////////////////////////////////////
//LibpcapDataSource.cpp

#include "cpf/ostypedef.h"

#ifdef OS_LINUX
//////////////////////////////////////////////////////////////////////////

#include "PacketIO/LibpcapDataSource.h"

CLibpcapDataSource::CLibpcapDataSource(void)
{
	m_pd_src = NULL;
}

CLibpcapDataSource::~CLibpcapDataSource(void)
{
}


//相当于init+OpenDataSource
BOOL CLibpcapDataSource::Open(const char * if_name)
{
	if( !this->init(if_name) )
	{
		return false;
	}

	return (this->OpenDataSource() == 0);
}

//相当于CloseDataSource()+fini
void CLibpcapDataSource::Close()
{
	this->CloseDataSource();

	this->fini();

	return;
}

bool CLibpcapDataSource::GetDataSourceName(std::string& name)
{
	name = m_strIfName;

	return true;		
}


BOOL CLibpcapDataSource::init(const char * if_name)
{
	m_strIfName = if_name;	

	return true;
}


void CLibpcapDataSource::fini()
{
	m_strIfName.clear();
}

int CLibpcapDataSource::OpenDataSource()
{
	const int errorlen = 1600;
	char ebuf[errorlen];

	m_pd_src = pcap_open_live(m_strIfName.c_str(),1600,1,1,ebuf);

	if( !m_pd_src )
	{
		return -1;
	}

	return 0;;
}


void CLibpcapDataSource::CloseDataSource()
{
	if( m_pd_src )
	{
		pcap_close(m_pd_src);
		m_pd_src = NULL;
	}
	return;
}

BOOL CLibpcapDataSource::StartToRecv()
{
	return false;
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CLibpcapDataSource::StopRecving()
{
	return;

}

//接收数据
PACKET_LEN_TYPE CLibpcapDataSource::GetPacket(RECV_PACKET& packet)
{
	const u_char * pdata = pcap_next(m_pd_src,&m_header);

	if( !pdata )
	{
		return PACKET_EMPTY;
	}

	packet.pPacket = (BYTE *)pdata;
	packet.nPktlen = (m_header.caplen+4);

	m_packet_header.stamp.sec = m_header.ts.tv_sec;
	m_packet_header.stamp.ns = m_header.ts.tv_usec*1000;

	packet.pHeaderinfo = &m_packet_header;

	m_packet_header.btCardNo = 0;

	return PACKET_OK;
}

//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
int CLibpcapDataSource::GetConnectState()
{
	return -1;
}

//////////////////////////////////////////////////////////////////////////
#endif//OS_LINUX
