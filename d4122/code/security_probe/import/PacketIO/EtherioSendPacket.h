//////////////////////////////////////////////////////////////////////////
//EtherioSendPacket.h


#pragma once


#include "PacketIO/PacketIO.h"
#include "PacketIO/EtherDataStreamBase.h"

class PACKETIO_CLASS CEtherioSendPacket : CEtherSendPacketBase
{
public:
	CEtherioSendPacket(void);
	virtual ~CEtherioSendPacket(void);

public:
	BOOL Open(int drv_load_type,
		unsigned int copy_data_to_send,
		const char * sysfilename=NULL,
		const char * drivername=NULL,
		const char * macaddr=NULL,
		unsigned int send_type=1,
		unsigned int memsize = 20);

	BOOL Open(int drv_load_type,
		unsigned int copy_data_to_send,
		const char * sysfilename=NULL,
		const char * drivername=NULL,
		int macindex_array=-1,
		unsigned int send_type=1,
		unsigned int memsize = 20);

	void Close();

	//在Open之后，在发送以前先调用StartToSend
	BOOL StartToSend();
	//在发送结束后，Close之前调用StopSending
	//bStopNow: 是否立即停止。如果是，则缓冲区剩余的数据不需要发送，如果否，则将剩余数据发送完毕后再结束
	void StopSending(BOOL bStopNow);

	BOOL SendPacket(const char * pdata,unsigned int datalen,int index_array,int try_nums);

public:
	//	 返回值
	//		=0 - 缓冲区当前已经使用了的长度
	//
	//	 说明
	//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）

	ACE_UINT32 GetSendBufferUsed(OUT ACE_UINT32* pTotalLen);


private:
	BOOL Inner_Open(int devcount,
		unsigned int copy_data_to_send,
		const char * sysfilename,
		const char * drivername,
		int macindex_array,
		unsigned int send_type,
		unsigned int memsize);

private:
	BOOL IsAllSendOver();

/*
private:
	int					m_nNumsHandle;
	DWORD				m_vtSendHandle[32];
	int					m_indexarray;

	int					m_nDevCount;

	int				m_nMTU;
	unsigned int	m_send_type;
	*/
};


class PACKETIO_CLASS CEtherioWriteDataStream : public CEtherDataStreamBase
{
public:
	CEtherioWriteDataStream();
	virtual ~CEtherioWriteDataStream();

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState();

protected:
	virtual const char* GetConstDriverName();

};

