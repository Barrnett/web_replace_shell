//////////////////////////////////////////////////////////////////////////
//EtherealWritefile.cpp


#include "cpf/ostypedef.h"

//#ifdef OS_WINDOWS

#include "PacketIO/EtherealWritefile.h"


#ifdef  __cplusplus
extern "C" {
#endif

#	include "pcap.h"


#ifdef  __cplusplus
}
#endif


//////////////////////////////////////////////////////////////////////////
//CEtherealWritefile
//////////////////////////////////////////////////////////////////////////

CEtherealWritefile::CEtherealWritefile(void)
{
	m_wd = 0;
}

CEtherealWritefile::~CEtherealWritefile(void)
{
	Close();
}

BOOL CEtherealWritefile::CreateFile(const char *filename, int filetype, int encap,int snaplen)
{
	int err = 0;
	m_encap = encap;

// #ifdef OS_WINDOWS
// 	m_wd=(void *)wtap_dump_open(
// 		filename, 
// 		filetype,
// 		encap,
// 		snaplen, 
// 		&err);
// 
// 
// #else
//	pcap_dumper_t* dumpfile = NULL; 
	// 建立存储截获数据包的文件 
	pcap_t * cap_t = pcap_open_dead(1, 65535);
	m_wd = (void *)pcap_dump_open(cap_t, filename);     

// #endif

	if( err == 0 && m_wd )
		return true;
	return false;
}

BOOL CEtherealWritefile::WriteData(const char * pdata,int datalen,int caplen,int sec,int ns)
{

#if 1	//OS_LINUX
	pcap_pkthdr header;      // Header 
	header.ts.tv_sec = sec;
	header.ts.tv_usec = ns/1000;
	header.caplen = caplen;
	header.len = datalen;
	if( WTAP_ENCAP_ETHERNET == m_encap )
	{
		header.len = datalen-4;
		header.caplen = caplen-4;
	}
	else
	{
		header.len = datalen;
		header.caplen = caplen;
	}

	pcap_dump((unsigned char*)m_wd, &header, (unsigned char*)pdata);       

	return TRUE;
#else

	int err = 0;

	struct wtap_pkthdr			whdr;
	union wtap_pseudo_header	pseudo_header;

	whdr.ts.tv_sec=sec;
	whdr.ts.tv_usec=ns/1000;

	if( WTAP_ENCAP_ETHERNET == m_encap )
	{
		whdr.len = datalen-4;
		whdr.caplen = caplen-4;
	}
	else
	{
		whdr.len = datalen;
		whdr.caplen = caplen;
	}

	whdr.pkt_encap=m_encap;

	pseudo_header.x25.flags=0;

	return wtap_dump((wtap_dumper *)m_wd,&whdr,&pseudo_header,(const BYTE *)pdata,&err);
#endif

}

void CEtherealWritefile::Close()
{
	if( m_wd )
	{
		int err = 0;
// #ifdef OS_WINDOWS
// 		wtap_dump_close((wtap_dumper *)m_wd,&err);
// #else
		pcap_dump_close((pcap_dumper_t *)m_wd);
// #endif
		m_wd = 0;
	}

}

//////////////////////////////////////////////////////////////////////////
//CEtherealWriteDataStream
//////////////////////////////////////////////////////////////////////////

CEtherealWriteDataStream::CEtherealWriteDataStream()
{
	m_pEtherealWriteFile = NULL;
	m_filetype = -1;
	m_encap = -1;
	m_snaplen = -1;
}

CEtherealWriteDataStream::~CEtherealWriteDataStream()
{
	CloseDataStream();
}

IFileWriteDataStream * CEtherealWriteDataStream::CloneAfterIni()
{
	CEtherealWriteDataStream * pEtherealWriteDataStream = new CEtherealWriteDataStream;

	if( !pEtherealWriteDataStream )
		return NULL;

	if( !pEtherealWriteDataStream->init(GetFileName(),m_filetype,m_encap,m_snaplen) )
	{
		delete pEtherealWriteDataStream;
		return NULL;
	}

	return pEtherealWriteDataStream;
}

BOOL CEtherealWriteDataStream::init(LPCSTR szFileName,int filetype,int encap,int snaplen)
{
	SetFileName(szFileName);
	m_filetype = filetype;
	m_encap = encap;
	m_snaplen = snaplen;

	return true;
}

void CEtherealWriteDataStream::fini()
{
	return;
}

//相当于init+OpenDataStream
BOOL CEtherealWriteDataStream::Open(LPCSTR szFileName,int filetype,int encap,int snaplen)
{
	if(!init(szFileName,filetype,encap,snaplen))
		return false;

	return (OpenDataStream()==0);
}

//相当于CloseDataStream+fini
void CEtherealWriteDataStream::Close()
{
	CloseDataStream();

	fini();
}


//打开设备．这个函数应该在参数设置好之后调用
//成功返回0,否则返回-1
int CEtherealWriteDataStream::OpenDataStream()
{
	CloseDataStream();

	m_pEtherealWriteFile = new CEtherealWritefile;

	if( m_pEtherealWriteFile->CreateFile(
			GetFileName(),
			m_filetype,m_encap,m_snaplen) )
	{
		return 0;
	}

	delete m_pEtherealWriteFile;
	m_pEtherealWriteFile = NULL;

	return -1;

}

//关闭设备
void CEtherealWriteDataStream::CloseDataStream()
{
	if( m_pEtherealWriteFile )
	{
		m_pEtherealWriteFile->Close();
		delete m_pEtherealWriteFile;
		m_pEtherealWriteFile = NULL;
	}
}

//保存数据，成功返回0,失败返回-1
int CEtherealWriteDataStream::WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums)
{
	if( type != PACKET_OK )
		return -1;

	if( m_pEtherealWriteFile->WriteData(
			(const char *)packet.pPacket,
			packet.nPktlen,
			packet.nCaplen,
			packet.pHeaderinfo->stamp.sec,
			packet.pHeaderinfo->stamp.ns) )
	{
		return 0;
	}

	return -1;
}


int CEtherealWriteDataStream::GetConnectState()
{
	if( m_pEtherealWriteFile )
	{
		return 1;
	}

	return 0;
}

bool CEtherealWriteDataStream::GetDataStreamName(std::string& name)
{
	if( m_strFileName.empty() )
	{
		name = "ethereal file";
	}
	else
	{
		name = m_strFileName;
	}

	return true;
}


//#endif//OS_WINDOWS

