//////////////////////////////////////////////////////////////////////////
//RecvDataSource.h

#ifndef __RECV_DATA_SOURCE_H_2006_4_26
#define __RECV_DATA_SOURCE_H_2006_4_26
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "PacketIO/PacketIO.h"

class PACKETIO_CLASS IRecvDataSource
{
public:
	IRecvDataSource()
	{

	}
	virtual ~IRecvDataSource()
	{

	}

	virtual DRIVER_TYPE GetDrvType() const = 0;

	BOOL IsPlaybackDataSource() const;

public:
	//程序可以在初始化init(..)之后,和fini(..)之前反复OpenDataSource和CloseDataSource

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataSource() = 0;
	
	//关闭设备
	virtual void CloseDataSource()
	{
		return;
	}

	//返回数据源的的名字
	virtual bool GetDataSourceName(std::string& name) = 0;

	//这个数据源统共有多少个包，多少个字节,-1表示无效
	//该函数必须在OpenDataSource返回成功后才能调用

	//一般对于文件类型的数据源才有意义
	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops)
	{	pkts = bytes = -1;	nloops = 1;}


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
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet)=0;
	
	//使这个接口能够产生定时包
	//如果设置成功，返回true．否则返回false.
	//使用者必须在开始接收以前调用这个函数，在开始接收以后则不允许调用
	virtual BOOL EnableTimerPacket(BOOL bEnable)
	{
		return false;
	}
	//询问这个设备是否可以产生定时包。
	virtual BOOL IsEnableTimerPacket() const
	{
		return false;
	}

	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin()
	{
		return false;
	}

	//将读数据位置指向结束的地方。
	virtual BOOL SeekToEnd()
	{
		return false;
	}

	//将读数据位置指向结束的地方。
	virtual BOOL SeekToOffset(ACE_INT64 offset)
	{
		return false;
	}

	//当没有获取到包，等待数据包
	//返回0表示有数据包，返回1表示超时,-1表示错误，
	//参数us同WaitForSingleObject()中的参数，不过变成了us
	virtual int WaitForPacket(const ACE_Time_Value& sleep_time)
	{
		ACE_OS::sleep(sleep_time);

		return 1;
	}

	//得到因为上层处理来不及，底层丢包的数目,-1表示无效
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes) = 0;

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState() = 0;
};

//////////////////////////////////////////////////////////////////////////
//IExtDataSource
//用于扩展装饰模式功能的扩展基类

class PACKETIO_CLASS IExtDataSource : public IRecvDataSource
{
public:
	IRecvDataSource * GetRealDataSource() const
	{
		return m_pRealDataSource;
	}

	virtual DRIVER_TYPE GetDrvType() const
	{
		if( m_pRealDataSource )
		{
			return m_pRealDataSource->GetDrvType();
		}

		return DRIVER_TYPE_NONE;
	}

public:
	virtual int OpenDataSource();

	virtual void CloseDataSource();


	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes);

	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops);

	//使这个接口能够产生定时包
	//如果设置成功，返回true．否则返回false.
	//使用者必须在开始接收以前调用这个函数，在开始接收以后则不允许调用
	virtual BOOL EnableTimerPacket(BOOL bEnable)
	{
		return m_pRealDataSource->EnableTimerPacket(bEnable);
	}

	//询问这个设备是否可以产生定时包。
	virtual BOOL IsEnableTimerPacket() const
	{
		return m_pRealDataSource->IsEnableTimerPacket();
	}

	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin();

	virtual bool GetDataSourceName(std::string& name)
	{
		if( m_pRealDataSource )
		{
			return m_pRealDataSource->GetDataSourceName(name);
		}

		return false;		
	}

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState()
	{
		if( m_pRealDataSource )
		{
			return m_pRealDataSource->GetConnectState();
		}

		return 0;
	}

	virtual int WaitForPacket(const ACE_Time_Value& sleep_time)
	{
		if( m_pRealDataSource )
		{
			return m_pRealDataSource->WaitForPacket(sleep_time);
		}

		return -1;
	}

protected:
	IExtDataSource()
	{
		m_pRealDataSource = NULL;
		m_delete_real_source = 0;

	}
	virtual ~IExtDataSource()
	{
		fini();
	}

	int init(IRecvDataSource * pRealDataSource,bool bdelete)
	{
		ACE_ASSERT(pRealDataSource);

		m_pRealDataSource = pRealDataSource;

		m_delete_real_source = (int)bdelete;

		return 0;
	}

	void fini()
	{
		if( m_delete_real_source && m_pRealDataSource )
		{
			delete m_pRealDataSource;
		}

		m_pRealDataSource = NULL;
	}


	BOOL StartToRecv();
	void StopRecving();

protected:
	IRecvDataSource *	m_pRealDataSource;
	int					m_delete_real_source;



};

//////////////////////////////////////////////////////////////////////////
//IRealDataSource
//实际功能的数据源

class PACKETIO_CLASS IRealDataSource : public IRecvDataSource
{
public:
	virtual DRIVER_TYPE GetDrvType() const
	{
		return m_drv_type;
	}

	void SetDrvType(DRIVER_TYPE drv_type)
	{
		m_drv_type = drv_type;
	}

protected:
	IRealDataSource()
	{

	}
	virtual ~IRealDataSource()
	{
	}


protected:
	DRIVER_TYPE	m_drv_type;
};

//////////////////////////////////////////////////////////////////////////
//IFileRecvDataSource

//基于单个文件的数据源
//，对于cpt格式文件来说，因为本身是连续文件的，也被看成是一个文件
//如果两个或者多个不相关的连续的文件，被看成多个文件
class PACKETIO_CLASS ISingleFileRecvDataSource : public IRealDataSource
{
public:
	ISingleFileRecvDataSource()
	{
		m_bcur_systime = false;
	}

	virtual ~ISingleFileRecvDataSource()
	{

	}

public:
	virtual BOOL Open(const char * filename,BOOL bcur_systime)
	{
		if( init(filename,bcur_systime) == FALSE )
			return false;

		return (OpenDataSource() == 0);
	}

	virtual void Close()
	{
		CloseDataSource();
		fini();
	}

	virtual bool GetDataSourceName(std::string& name);

	virtual BOOL init(const char * filename,BOOL bcur_systime);

	virtual void fini();

	//得到因为上层处理来不及，底层丢包的数目
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
	{
		pkts = 0;
		bytes = 0;
	}

public:
	static std::string GetFileOtherInfo(const char * fullname,const Time_Stamp * pcur_time);

protected:
	void AdjustToCurSystime(PACKET_LEN_TYPE lentype,RECV_PACKET& packet)
	{
		if( m_bcur_systime &&
			(PACKET_OK == lentype || PACKET_TIMER == lentype) )
		{
			packet.pHeaderinfo->stamp.from_ace_timevalue(ACE_OS::gettimeofday());
		}
	}

protected:
	std::string m_strFileName;
	BOOL		m_bcur_systime;

};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//__RECV_DATA_SOURCE_H_2006_4_26
