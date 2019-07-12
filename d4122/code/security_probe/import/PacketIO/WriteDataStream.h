//////////////////////////////////////////////////////////////////////////
//WriteDataStream.h

#pragma once

#include "PacketIO/RecvDataSource.h"

class PACKETIO_CLASS IBeforeSendPacketHandle
{
public:
	//在发送数据之前，处理数据，可能修改数据
	virtual const char* BeforeSendPacket(int card_index_array,const char * pdata, unsigned int datalen, unsigned int& outdatalen) = 0;

};

class PACKETIO_CLASS IWriteDataStream
{
public:
	IWriteDataStream()
	{
		m_ullTotalSendPkts = 0;
		m_ullTotalFailedPkts = 0;
		m_pBeforeSendPacketHandle = NULL;
	}
	virtual ~IWriteDataStream()
	{

	}
public:
	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataStream() = 0;

	//关闭设备
	virtual void CloseDataStream()
	{
		return;
	}

	//开始发送数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToWrite()
	{
		m_ullTotalSendPkts = 0;
		m_ullTotalFailedPkts = 0;

		return TRUE;
	}

	//停止发送数据，注意不是关闭设备，可能有些子类没有具体实现。
	//bStopNow: 是否立即停止。如果是，则缓冲区剩余的数据不需要发送，如果否，则将剩余数据发送完毕后再结束
	virtual void StopWriting(BOOL bStopNow=false)
	{
		return;
	}

	ACE_UINT64 GetTotalSendPkts() const
	{	return m_ullTotalSendPkts;	}
	ACE_UINT64 GetTotalFailedPkts() const
	{	return m_ullTotalFailedPkts;	}

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState() = 0;

	//返回数据发送目标的的名字
	virtual bool GetDataStreamName(std::string& name) = 0;

public:
	//0表示正常,-1表示设备出错，-2表示空间磁盘满，-3表示文件写完成了。
	//try_nums:如果发送失败，尝试发送多少次。try_nums=-1表示无数次，直到成功
	int WritePacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums)
	{
		++m_ullTotalSendPkts;

		if (m_pBeforeSendPacketHandle && PACKET_OK==type)
		{
			void * pNewData = packet.pPacket;
			unsigned int newDataLen = packet.nCaplen;
			pNewData = (void *)m_pBeforeSendPacketHandle->BeforeSendPacket(index_array,(const char*)pNewData, newDataLen, newDataLen);
			((RECV_PACKET*)&packet)->pPacket = pNewData;
			((RECV_PACKET*)&packet)->nCaplen = newDataLen;
			((RECV_PACKET*)&packet)->nPktlen = newDataLen;
		}

		int nrtn = WritePacketToDevWithPacket(type,packet,index_array,try_nums);

		if( 0 != nrtn )
		{
			++m_ullTotalFailedPkts;
		}

		return nrtn;
	}

	int WritePacket(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums)
	{
		++m_ullTotalSendPkts;

		if (m_pBeforeSendPacketHandle && PACKET_OK==type)
		{
			unsigned int newDataLen = datalen;

			const char* pNewData = m_pBeforeSendPacketHandle->BeforeSendPacket(index_array,pdata, datalen, newDataLen);
			
			pdata = pNewData;
			datalen = newDataLen;
		}

		int nrtn = WritePacketToDevWithData(type,pdata,datalen,index_array,try_nums);

		if( 0 != nrtn  )
		{
			++m_ullTotalFailedPkts;
		}

		return nrtn;
	}

public:
	inline void SetBeforeSendPacketHanlde(IBeforeSendPacketHandle * pBeforeSendPacketHandle)
	{
		m_pBeforeSendPacketHandle = pBeforeSendPacketHandle;
	}


protected:
	//0表示正常,-1表示设备出错，-2表示空间磁盘满，-3表示文件写完成了。
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums) = 0;
	virtual int WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums) = 0;

protected:
	ACE_UINT64	m_ullTotalSendPkts;
	ACE_UINT64	m_ullTotalFailedPkts;

	IBeforeSendPacketHandle *	m_pBeforeSendPacketHandle;
};

//基于写文件的输出数据
class PACKETIO_CLASS IFileWriteDataStream : public IWriteDataStream
{
public:
	IFileWriteDataStream()
	{
		memset(&m_packet_header,0x00,sizeof(m_packet_header));
		m_packet_header.btHeaderLength = sizeof(m_packet_header);

		m_packet_header.btCardNo = 1;
		m_packet_header.btHardwareType = 0;
		m_packet_header.btInterfaceNo = 0;
		m_packet_header.btVersion = 1;
		m_packet_header.nFlag = 0;
		m_packet_header.dwError = 0;

	}
	virtual ~IFileWriteDataStream()
	{

	}

	//是否支持写控制．
	virtual BOOL IsSupportWriteCtrl() const = 0;

	//clone一个对象，要求返回的对象是该对象init之后，OpenDataStream之前的状态
	//这样得到clone返回的对象后，可以OpenDataStream,CloseDataStream,WritePacket.
	virtual IFileWriteDataStream * CloneAfterIni() = 0;

	const char * GetFileName() const
	{
		return m_strFileName.c_str();
	}

	void SetFileName(const char * filename)
	{
		ACE_ASSERT(filename&&filename[0] != 0);

		if( !filename || filename[0] == 0 )
		{
			m_strFileName.clear();
		}
		else
		{
			m_strFileName = filename;
		}
	}

public:
	virtual int WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums)
	{
		RECV_PACKET packet;
		memset(&packet,0x00,sizeof(packet));

		packet.pHeaderinfo = &m_packet_header;

		m_packet_header.stamp.from_ace_timevalue(ACE_OS::gettimeofday());

		packet.pPacket = (void *)pdata;
		packet.nPktlen = packet.nCaplen = (int)datalen;

		return WritePacketToDevWithPacket(type,packet,index_array,try_nums);
	}

protected:
	PACKET_HEADER	m_packet_header;
	std::string		m_strFileName;

};

//基于板卡的发送数据
class PACKETIO_CLASS IDevWriteDataStream : public IWriteDataStream
{
public:
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums)
	{
		if( type == PACKET_OK )
		{
			return this->WritePacketToDevWithData(type,(const char *)packet.pPacket,(unsigned int)packet.nPktlen,index_array,try_nums);
		}

		return -1;
	}
};
