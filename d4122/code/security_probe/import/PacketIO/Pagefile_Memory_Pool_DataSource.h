//////////////////////////////////////////////////////////////////////////
//Pagefile_Memory_Pool_DataSource.h

//和Pagefile_Memory_Pool_DataStream相对应

#pragma once

#include "PacketIO/RecvDataSource.h"
#include "cpf/CPF_ShareMem.h"
#include "cpf/CommonQueueManager.h"

class PACKETIO_CLASS CPagefile_Memory_Pool_DataSource : public IRealDataSource
{
public:
	CPagefile_Memory_Pool_DataSource(void);
	virtual ~CPagefile_Memory_Pool_DataSource(void);


	//相当于init+OpenDataSource
	BOOL Open(const char * share_name,size_t memsize);

	//相当于CloseDataSource()+fini
	void Close();

	virtual bool GetDataSourceName(std::string& name);

	BOOL init(const char * share_name,size_t memsize);

	void fini(); 

	//程序可以在初始化init(..)之后,和fini(..)之前反复OpenDataSource和CloseDataSource

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataSource();

	//关闭设备
	virtual void CloseDataSource();


	//程序可以在初始化OpenDataSource(..)之后,和StopRecving(..)之前反复StartToRecv和StopRecving

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv()
	{
		return true;
	}

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving()
	{
		return;
	}

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//询问这个设备是否可以产生定时包。
	virtual BOOL IsEnableTimerPacket() const;

	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes);

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState();

public:
	//	 返回值
	//		=0 - 缓冲区当前已经使用了的长度
	//
	//	 说明
	//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）
	ACE_UINT32 GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen);


private:
	std::string		m_strShareName;
	size_t			m_memsize;

private:
	CPF_ShareMem	m_cpf_share_mem;

	TAG_COMMOM_QUEUE_MANAGER	m_mem_que;

};
