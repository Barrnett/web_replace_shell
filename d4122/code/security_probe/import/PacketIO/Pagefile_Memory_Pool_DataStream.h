//////////////////////////////////////////////////////////////////////////
//Pagefile_Memory_Pool_DataStream.h

//和Pagefile_Memory_Pool_DataSource相对应

#pragma once

#include "PacketIO/WriteDataStream.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/CPF_ShareMem.h"
#include "cpf/CommonQueueManager.h"

class PACKETIO_CLASS CPagefile_Memory_Pool_DataStream : public IFileWriteDataStream
{
public:
	CPagefile_Memory_Pool_DataStream(void);
	virtual ~CPagefile_Memory_Pool_DataStream(void);


	//相当于init+OpenDataStream
	BOOL Open(LPCSTR sharename,DWORD memsize);

	//相当于CloseDataStream+fini
	void Close();

	BOOL init(LPCSTR sharename,DWORD memsize);

	void fini();


	//是否支持写控制．
	virtual BOOL IsSupportWriteCtrl() const
	{
		return false;
	}

	virtual IFileWriteDataStream * CloneAfterIni();

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataStream();

	//关闭设备
	virtual void CloseDataStream();

	//保存数据，成功返回0,失败返回-1
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums);

	//保存数据，成功返回0,失败返回-1
	virtual int WritePacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,
		IN const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		int attach_len,const void * attach_data,
		int index_array,int try_nums);

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState();

	virtual bool GetDataStreamName(std::string& name);

public:
	ACE_UINT32 GetBufferUsed(OUT ACE_UINT32 * total=NULL) const;

private:
	CPF_ShareMem	m_cpf_share_mem;

private:
	size_t			m_memsize;
	std::string		m_strShareName;

	TAG_COMMOM_QUEUE_MANAGER	m_mem_que;

};


