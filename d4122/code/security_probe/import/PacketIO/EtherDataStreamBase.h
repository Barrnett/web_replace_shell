//////////////////////////////////////////////////////////////////////////
// EtherDataStreamBase.h 


#pragma once


#include "PacketIO/PacketIO.h"
#include "PacketIO/WriteDataStream.h"

class PACKETIO_CLASS CEtherSendPacketBase
{
public:
	CEtherSendPacketBase(void);
	virtual ~CEtherSendPacketBase(void);

public:
	virtual BOOL Open(int drv_load_type,
		unsigned int copy_data_to_send,
		const char * sysfilename=NULL,
		const char * drivername=NULL,
		const char * macaddr=NULL,
		unsigned int send_type=1,
		unsigned int memsize = 20) = 0;

	virtual BOOL Open(int drv_load_type,
		unsigned int copy_data_to_send,
		const char * sysfilename=NULL,
		const char * drivername=NULL,
		int macindex_array=-1,
		unsigned int send_type=1,
		unsigned int memsize = 20) = 0;

	virtual void Close() = 0;

	//在Open之后，在发送以前先调用StartToSend
	virtual BOOL StartToSend() = 0;
	//在发送结束后，Close之前调用StopSending
	//bStopNow: 是否立即停止。如果是，则缓冲区剩余的数据不需要发送，如果否，则将剩余数据发送完毕后再结束
	virtual void StopSending(BOOL bStopNow) = 0;

	virtual BOOL SendPacket(const char * pdata,unsigned int datalen,int index_array,int try_nums) = 0;

public:
	//	 返回值
	//		=0 - 缓冲区当前已经使用了的长度
	//
	//	 说明
	//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）

	virtual ACE_UINT32 GetSendBufferUsed(OUT ACE_UINT32* pTotalLen) = 0;


/*
protected:
	virtual BOOL Inner_Open(int devcount,
		const char * sysfilename,
		const char * drivername,
		int macindex_array,
		unsigned int send_type,
		unsigned int memsize) = 0;

	virtual BOOL IsAllSendOver() = 0;
*/

protected:
	int					m_nNumsHandle;
	void*				m_vtSendHandle[32];
	int					m_indexarray;

	int					m_nDevCount;

	int				m_nMTU;
	unsigned int	m_send_type;
};


class PACKETIO_CLASS CEtherDataStreamBase : public IDevWriteDataStream
{
public:
	CEtherDataStreamBase();
	virtual ~CEtherDataStreamBase();

public:
	//相当于init+OpenDataStream
	virtual BOOL Open(int drv_load_type,unsigned int copy_data_to_send,const char * sysfilename=NULL,const char *drivername=NULL,
		const char * macaddr=NULL,unsigned int send_type=1,unsigned int memsize=20);

	virtual BOOL Open(int drv_load_type,unsigned int copy_data_to_send,const char * sysfilename=NULL,const char *drivername=NULL,
		int macindex_array=-1,unsigned int send_type=1,unsigned int memsize=20);

	//相当于CloseDataStream+fini
	virtual void Close();

	virtual BOOL init(int drv_load_type,unsigned int copy_data_to_send,const char * sysfilename=NULL,const char *drivername=NULL,
		const char * macaddr=NULL,unsigned int send_type=1,unsigned int memsize=20);

	virtual BOOL init(int drv_load_type,unsigned int copy_data_to_send,const char * sysfilename=NULL,const char *drivername=NULL,
		int macindex_array=-1,unsigned int send_type=1,unsigned int memsize=20);

	virtual void fini();

	virtual int OpenDataStream();

	//关闭设备
	virtual void CloseDataStream();


	//开始发送数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToWrite();

	//停止发送数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopWriting(BOOL bStopNow=false);

	virtual int WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums);

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
//	virtual int GetConnectState();

	virtual bool GetDataStreamName(std::string& name);

public:
	//	 返回值
	//		=0 - 缓冲区当前已经使用了的长度
	//
	//	 说明
	//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）

	ACE_UINT32 GetSendBufferUsed(OUT ACE_UINT32* pTotalLen);

protected:
	void GetSysAndDriverName(const char * sysfilename,const char *drivername);
	virtual const char* GetConstDriverName() = 0;

protected:
	CEtherSendPacketBase*	m_pEtherSendPacket;

	int					m_drv_load_type;
	unsigned int		m_copy_data_to_send;
	unsigned int		m_nMemSize;
	unsigned int		m_send_type;

	int					m_indexarray;
	std::string			m_strMacAddr;
	std::string			m_strSysFileName;
	std::string			m_strDriverName;
};

