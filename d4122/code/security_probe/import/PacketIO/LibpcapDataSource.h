//////////////////////////////////////////////////////////////////////////
//LibpcapDataSource.h

#pragma once

#include "cpf/ostypedef.h"

#ifdef OS_LINUX
//////////////////////////////////////////////////////////////////////////

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

#include <pcap.h>
#include <linux/if_ether.h>

class PACKETIO_CLASS CLibpcapDataSource : public IRealDataSource
{
public:
	CLibpcapDataSource(void);
	virtual ~CLibpcapDataSource(void);

public:
	//相当于init+OpenDataSource
	BOOL Open(const char * if_name);

	//相当于CloseDataSource()+fini
	void Close();

	virtual bool GetDataSourceName(std::string& name);

	BOOL init(const char * if_name);

	void fini(); 

	//程序可以在初始化init(..)之后,和fini(..)之前反复OpenDataSource和CloseDataSource

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataSource();

	//关闭设备
	virtual void CloseDataSource();

	//程序可以在初始化OpenDataSource(..)之后,和StopRecving(..)之前反复StartToRecv和StopRecving

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving();

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState();

private:
	std::string			m_strIfName;

	pcap_t *			m_pd_src;

	struct pcap_pkthdr	m_header;
	PACKET_HEADER		m_packet_header;

};

//////////////////////////////////////////////////////////////////////////
#endif//OS_LINUX

