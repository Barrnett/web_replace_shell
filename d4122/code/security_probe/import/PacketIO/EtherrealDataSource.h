// EtherrealDataSource.h: interface for the CEtherrealDataSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERREALDATASOURCE_H__B86804C2_DC90_45B6_A94C_E36891A04B37__INCLUDED_)
#define AFX_ETHERREALDATASOURCE_H__B86804C2_DC90_45B6_A94C_E36891A04B37__INCLUDED_

#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

#include "pcap.h"


//////////////////////////////////////////////////////////////////////////
//CEtherrealDataSource
//////////////////////////////////////////////////////////////////////////
class PACKETIO_CLASS CEtherrealDataSource : public ISingleFileRecvDataSource  
{
public:
	CEtherrealDataSource();
	virtual ~CEtherrealDataSource();

	virtual bool GetDataSourceName(std::string& name);

public:
	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving();

	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin();

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState();

public:

	virtual int OpenDataSource();
	virtual void CloseDataSource();

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//ethereal的encap_type,影射成标准的协议id
#ifdef OS_WINDOWS
	static ACE_UINT32 EtherealEncapetype_Map_StdProtoId(ACE_UINT32 nStdProtoId);
#endif
private:
//	void *			m_fileHandle;
	pcap_t *			m_fileHandle;
	long			m_data_offset;
	PACKET_HEADER	m_headerInfo; 
	CTimeStamp32	m_lastPktStamp;

};



//////////////////////////////////////////////////////////////////////////
//CVPEtherrealDataSource
//////////////////////////////////////////////////////////////////////////
#include "VPKHelpDataSource.h"

class PACKETIO_CLASS CVPKEtherrealDataSource : public CVPKHelpDataSource
{
public:
	CVPKEtherrealDataSource();
	virtual ~CVPKEtherrealDataSource();

public:
	BOOL Open(const char *filename,BOOL bcur_systime);
	void Close();


private:
	CEtherrealDataSource * m_pEtherealDataSource;

};



#endif // !defined(AFX_ETHERREALDATASOURCE_H__B86804C2_DC90_45B6_A94C_E36891A04B37__INCLUDED_)
