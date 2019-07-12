//////////////////////////////////////////////////////////////////////////
//MemDataSource.h

//用于从内存中生成一些长度的数据包。主要用于进行性能测试的需要。

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"


class PACKETIO_CLASS CMemDataSource : public IRealDataSource
{
public:
	CMemDataSource(void);
	virtual ~CMemDataSource(void);

public:
	//相当于init+OpenDataSource
	BOOL Open(unsigned int minlen,unsigned int maxlen,WORD wdProto=2,
		const unsigned char * pModelData=NULL,unsigned int modeldata_len=0);

	//相当于CloseDataSource()+fini
	void Close();

	virtual bool GetDataSourceName(std::string& name);

	//希望产生数据的长度范围[minlen,maxlen]。如果数据的长度是从[minlen,maxlen]滚动递增1。
	//生成数据的协议号码(比如MAC协议=2)
	//希望生成数据的模版，如果没有数据生成将是随机数据。
	void init(unsigned int minlen,unsigned int maxlen,WORD wdProto=2,
		const unsigned char * pModelData=NULL,unsigned int modeldata_len=0);

	void fini();

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataSource();

	//关闭设备
	virtual void CloseDataSource();

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);


	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin()
	{
		return TRUE;
	}

	//将读数据位置指向结束的地方。
	virtual BOOL SeekToEnd()
	{
		return TRUE;
	}

	//将读数据位置指向结束的地方。
	virtual BOOL SeekToOffset(ACE_INT64 offset)
	{
		return false;
	}

	//得到因为上层处理来不及，底层丢包的数目,-1表示无效
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes);

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState();


private:
	//如果两个相等，表示产生的包需要同一个大小
	unsigned int	m_nMinLen;//需要产生包的最小长度
	unsigned int	m_nMaxLen;//需要产生包的最大长度

	unsigned char *	m_pModelData;

	PACKET_HEADER	m_packetheader;
	unsigned int	m_nCurLen;
};
