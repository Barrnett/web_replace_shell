// EtherrealDataSource.cpp: implementation of the CEtherrealDataSource class.
//
//////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"


#include "PacketIO/EtherrealDataSource.h"
#include "PacketIO/MyProtocolID.h"

#ifdef  __cplusplus
	extern "C" {
#endif

#	include "pcap.h"


#ifdef  __cplusplus
	}
#endif


#if defined (OS_WINDOWS) && (_WIN64)
#	pragma comment( lib, "./winpcap/lib_x64/packet.lib")
#	pragma comment( lib, "./winpcap/lib_x64/wpcap.lib")
#else
#	pragma comment( lib, "./winpcap/lib_win32/packet.lib")
#	pragma comment( lib, "./winpcap/lib_win32/wpcap.lib")
#endif



//////////////////////////////////////////////////////////////////////
// CEtherrealDataSource
//////////////////////////////////////////////////////////////////////

#define wtap_pointer(p)	((struct wtap*)p)

CEtherrealDataSource::CEtherrealDataSource()
{
	memset(&m_headerInfo,0x00,sizeof(m_headerInfo));

	m_headerInfo.btHeaderLength = sizeof(m_headerInfo);
	m_headerInfo.btVersion = 1;
	m_headerInfo.btCardNo = 1;
	m_headerInfo.btInterfaceNo = 1;

	m_lastPktStamp.m_ts.sec = 0;
	m_lastPktStamp.m_ts.ns = 0;

	m_fileHandle = NULL;
	m_data_offset = 0;

}

CEtherrealDataSource::~CEtherrealDataSource()
{
	
}

int CEtherrealDataSource::OpenDataSource()
{
	CloseDataSource();

	int err = 0;

// #ifdef OS_WINDOWS
// 	gchar * err_info = NULL;
// 	m_fileHandle = (void *)wtap_open_offline(m_strFileName.c_str(), &err, &err_info, FALSE);
// 
// 	if( m_fileHandle == NULL )
// 		return -1;
// 
// 	m_headerInfo.wdProto = (WORD)EtherealEncapetype_Map_StdProtoId(
// 		wtap_file_encap(wtap_pointer(m_fileHandle)));
// 
// #else
	char errBuff[PCAP_ERRBUF_SIZE];
	pcap_t *handle = NULL;

	m_fileHandle = pcap_open_offline(m_strFileName.c_str(), errBuff);
	if (NULL == m_fileHandle)
		return -1;

	m_headerInfo.wdProto = MY_PROTOCOL_MAC;
// #endif
	m_data_offset = 0;
	m_lastPktStamp = CTimeStamp32::zero;

	return 0;

}

void CEtherrealDataSource::CloseDataSource()
{
	if( m_fileHandle )
	{
// #ifdef OS_WINDOWS
// 		wtap_close(wtap_pointer(m_fileHandle));
// #else
		pcap_close(m_fileHandle);
// #endif
		m_fileHandle = NULL;
	}

	return;
}

//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
BOOL CEtherrealDataSource::StartToRecv()
{
	m_data_offset = 0;
	m_lastPktStamp = CTimeStamp32::zero;

	return true;
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CEtherrealDataSource::StopRecving()
{

}

bool CEtherrealDataSource::GetDataSourceName(std::string& name)
{
	std::string otherinfo = GetFileOtherInfo(m_strFileName.c_str(),&m_lastPktStamp.m_ts);

	name = m_strFileName + otherinfo;

	return true;
}


#if 0 //def OS_WINDOWS

PACKET_LEN_TYPE CEtherrealDataSource::GetPacket(RECV_PACKET& packet)
{
	int err = 0;
	gchar * err_info = NULL;

	if( !wtap_read(wtap_pointer(m_fileHandle), &err, &err_info,&m_data_offset) )
	{
		if( err == 0 )
			return PACKET_NO_MORE;
		else
			return PACKET_DRV_ERROR;
	}
	
    wtap_pkthdr * phdr = wtap_phdr(wtap_pointer(m_fileHandle));

	union wtap_pseudo_header * pseudo_header = 
		wtap_pseudoheader(wtap_pointer(m_fileHandle));

#ifdef OS_WINDOWS
	m_headerInfo.stamp.from_timeval(phdr->ts);
#else
	struct timeval time_temp;
	time_temp.tv_sec = phdr->ts.secs;
	time_temp.tv_usec = phdr->ts.nsecs;
	m_headerInfo.stamp.from_timeval(time_temp);
#endif

	if( CTimeStamp32(m_headerInfo.stamp) < m_lastPktStamp )
	{//如果时间小于上一个包的时间，调整当前包的时间
		m_headerInfo.stamp = m_lastPktStamp.m_ts;
	}
	else
	{
		m_lastPktStamp.m_ts = m_headerInfo.stamp;
	}

	packet.pHeaderinfo = &m_headerInfo;

	if( MY_PROTOCOL_MAC == m_headerInfo.wdProto )
	{
		packet.nCaplen = phdr->caplen+4;
		packet.nPktlen = phdr->len+4;
	}

	packet.pPacket = wtap_buf_ptr(wtap_pointer(m_fileHandle));
	packet.nPktAddiBitLen = 0;
	packet.nOffsetBit = 0;
	packet.nCapAddiBitLen = 0;

	AdjustToCurSystime(PACKET_OK,packet);

	return PACKET_OK; 
}


#else
PACKET_LEN_TYPE CEtherrealDataSource::GetPacket(RECV_PACKET& packet)
{
	int err = 0;
//	gchar * err_info = NULL;


	pcap_pkthdr *pktHeader;
	int status = -2;
	const u_char *pktData;

/*
	intpcap_next_ex(pcap_t *p,struct pcap_pkthdr** pkt_header, constu_char** pkt_data); 读取下一包, 返回值含义如下
		1 if the packet was read without problems,

		0  if packets are being read from a live capture, and the timeout expired

		-1 if an error occurred while reading the packet

		-2  if  packets  are being  read  from a ``savefile'', and there are no more packets to read from the savefile. 

		If -1 is returned, pcap_geterr()  or  pcap_perror() may be called with p as an argument to fetch or display the error text.
*/
	status = pcap_next_ex(m_fileHandle, &pktHeader, &pktData );

	switch (status)
	{
	case 0:		return PACKET_TIMER;
	case -1:	return PACKET_DRV_ERROR;
	case -2:	return PACKET_NO_MORE;
	case 1:	// success
		break;
	}

	m_headerInfo.stamp.from_timeval(pktHeader->ts);

	if( CTimeStamp32(m_headerInfo.stamp) < m_lastPktStamp )
	{//如果时间小于上一个包的时间，调整当前包的时间
		m_headerInfo.stamp = m_lastPktStamp.m_ts;
	}
	else
	{
		m_lastPktStamp.m_ts = m_headerInfo.stamp;
	}

	packet.pHeaderinfo = &m_headerInfo;

	if( MY_PROTOCOL_MAC == m_headerInfo.wdProto )
	{
		packet.nCaplen = pktHeader->caplen+4;
		packet.nPktlen = pktHeader->len+4;
	}

	packet.pPacket = (void *)pktData;
	packet.nPktAddiBitLen = 0;
	packet.nOffsetBit = 0;
	packet.nCapAddiBitLen = 0;

	AdjustToCurSystime(PACKET_OK,packet);

	return PACKET_OK; 
}



#endif

BOOL CEtherrealDataSource::SeekToBegin()
{
	CloseDataSource();

	return OpenDataSource();
}


//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
int CEtherrealDataSource::GetConnectState()
{
	if( m_fileHandle )
	{
		return 1;
	}

	return 0;

}

/*
#ifdef OS_WINDOWS

//ethereal的encap_type,影射成标准的协议id
ACE_UINT32 CEtherrealDataSource::EtherealEncapetype_Map_StdProtoId(ACE_UINT32 encap_type)
{
	switch(encap_type)
	{
	case WTAP_ENCAP_UNKNOWN:
	case WTAP_ENCAP_PER_PACKET:
		return MY_PROTOCOL_NONE;
		break;

	case WTAP_ENCAP_ETHERNET:
	case WTAP_ENCAP_BLUETOOTH_H4:
		return MY_PROTOCOL_MAC;
		break;

	case WTAP_ENCAP_PPP:
		return MY_PROTOCOL_PPP;
		break;

	case WTAP_ENCAP_RAW_IP:
		return MY_PROTOCOL_IPv4;
		break;

	case WTAP_ENCAP_FRELAY:
		return MY_PROTOCOL_FR;
		break;

	default:
		ACE_ASSERT(false);
		return MY_PROTOCOL_NONE;
		break;
	}

	return MY_PROTOCOL_NONE;
}
#endif
*/


//////////////////////////////////////////////////////////////////////////
//CVPKEtherrealDataSource
//////////////////////////////////////////////////////////////////////////


CVPKEtherrealDataSource::CVPKEtherrealDataSource()
{
	m_pEtherealDataSource = NULL;

}

CVPKEtherrealDataSource::~CVPKEtherrealDataSource()
{
	Close();
}

BOOL CVPKEtherrealDataSource::Open(const char *filename,BOOL bcur_systime)
{
	m_pEtherealDataSource = new CEtherrealDataSource;

	if( !m_pEtherealDataSource )
		return false;
	
	if( !m_pEtherealDataSource->Open(filename,bcur_systime) )
	{
		CloseDataSource();
		return false;
	}

	CVPKHelpDataSource::init(m_pEtherealDataSource,1);

	return true;
}

void CVPKEtherrealDataSource::Close()
{
	CloseDataSource();

	CVPKHelpDataSource::fini();

}

//#endif//OS_WINDOWS

