//////////////////////////////////////////////////////////////////////////
// EtherDataSourceBase.h 

#pragma once

#include "cpf/ostypedef.h"

//#ifdef OS_WINDOWS

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

class PACKETIO_CLASS CEtherDataSourceBase : public IRealDataSource
{
public:
	CEtherDataSourceBase(void);
	virtual ~CEtherDataSourceBase(void);

	//相当于init+OpenDataSource
	BOOL Open(int drv_load_type,const char * sysfilename,const char *drivername,const char * macaddr,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code);

	BOOL Open(int drv_load_type,const char * sysfilename,const char *drivername,int macindex_array,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code);


	//相当于CloseDataSource()+fini
	void Close();

	bool GetDataSourceName(std::string& name);

	virtual BOOL init(int drv_load_type,const char * sysfilename,const char *drivername,const char * macaddr,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code) = 0;

	virtual BOOL init(int drv_load_type,const char * sysfilename,const char *drivername,int macindex_array,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code) = 0;

	virtual void fini() = 0;

	//程序可以在初始化init(..)之后,和fini(..)之前反复OpenDataSource和CloseDataSource

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataSource() = 0;

	//关闭设备
	virtual void CloseDataSource() = 0;


	//程序可以在初始化OpenDataSource(..)之后,和StopRecving(..)之前反复StartToRecv和StopRecving

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv() = 0;

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving() = 0;

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet) = 0;

	//使这个接口能够产生定时包
	//如果设置成功，返回true．否则返回false.
	//使用者必须在开始接收以前调用这个函数，在开始接收以后则不允许调用
	BOOL EnableTimerPacket(BOOL bEnable);

	//询问这个设备是否可以产生定时包。
	BOOL IsEnableTimerPacket() const;

	//得到因为上层处理来不及，底层丢包的数目,-1表示无效
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes) = 0;

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState() = 0;

	virtual int WaitForPacket(const ACE_Time_Value& sleep_time) = 0;

public:
	//	 返回值
	//		=0 - 缓冲区当前已经使用了的长度
	//
	//	 说明
	//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）
	virtual ACE_UINT32 GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen) = 0;

protected:
	void GetSysAndDriverName(const char * sysfilename,const char *drivername);
	virtual const char* GetConstDriverName() = 0;

protected:
	int				m_indexarray;
	std::string		m_strSysFileName;
	std::string		m_strDriverName;
	std::string		m_strMacaddrInfo;

	unsigned int	m_nMemSize;
	BOOL			m_bEnableTimerPacket;
	int				m_nTimeStampType;
	BOOL			m_bErrorRecover;

	HANDLE			m_dwRecvHandle;
	int				m_nDevCount;//网卡的个数

	PACKET_HEADER	m_packet_header;
};



//#endif//OS_WINDOWS

