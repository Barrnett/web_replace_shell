//////////////////////////////////////////////////////////////////////////
//MultiFileDataSource.h

#pragma once


#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"
#include <utility>
#include "ace/Recursive_Thread_Mutex.h"

class PACKETIO_CLASS CMultiFileDataSource : public IRecvDataSource
{
public:
	CMultiFileDataSource(void);
	virtual ~CMultiFileDataSource(void);

	virtual DRIVER_TYPE GetDrvType() const
	{
		return DRIVER_TYPE_MULTI_FILE;
	}

public:
	//不能多次调用,相当于init+AddMultiFileName+OpenDataSource
	//re_calculate_time=1表示多个文件的时间进行排序。这样保证第二个文件包的时间肯定比前一个文件大。
	//re_calculate_time=0表示使用每个文件包里面的时间,这样就可能出现后面包的时间小于前面包的时间。
	//bcur_systime:每个包的时间是使用文件中包的时间还是使用当前系统时间,如果bcur_systime=true,则re_calculate_time无效
	BOOL Open_EthrealFiles(const CStdStringArray& vtFilename,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false);

	BOOL Open_CptFiles(const CStdStringArray& vtFilename,
		unsigned int nReadNums,unsigned int bufsizeMB,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false);


	//不能多次调用,相当于init+AddFileName+OpenDataSource
	BOOL Open_EthrealFile(const char * filename,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false);

	BOOL Open_CptFile(const char * filename,
		unsigned int nReadNums,unsigned int bufsizeMB,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false);

	void Close()
	{
		CloseDataSource();
		fini();
	}

	virtual bool GetDataSourceName(std::string& name);

	void init_Cpt(unsigned int nReadNums,unsigned int bufsizeMB,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false)
	{
		m_nReadNums = nReadNums;
		m_nBufferSizeMB = bufsizeMB;

		m_re_calculate_time = (int)re_calculate_time;
		m_bcur_systime = (int)bcur_systime;
		m_bskip_error_file = (int)skip_error_file;
	}

	void init_Ethreal(bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false)
	{
		m_re_calculate_time = (int)re_calculate_time;
		m_bcur_systime = (int)bcur_systime;
		m_bskip_error_file = (int)skip_error_file;
	}

	//一次调用传入多个文件，可以多次调用。文件会累加
	void AddMultiFileName(DRIVER_TYPE type,const CStdStringArray& vtFilename);

	//可以多次调用AddFileName。文件会累加
	void AddFileName(DRIVER_TYPE type,const char * filename);

	void fini();

	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops);

	//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
	virtual BOOL StartToRecv();

	//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
	virtual void StopRecving();

	virtual int OpenDataSource();
	virtual void CloseDataSource();

	//接收数据
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
	{
		pkts = bytes = 0;
		return;
	}


	//将读数据位置指向开始的地方。
	virtual BOOL SeekToBegin();

	virtual int GetConnectState();
	
private:
	//转到下一个文件，并且读一个数据包
	//返回是否结束，如果返回FALSE，表示所有文件都读完了，返回的数据无效。
	//如果返回TRUE表示，表示数据有效
	PACKET_LEN_TYPE TurnToNextFileAndRead(RECV_PACKET& packet);

	ISingleFileRecvDataSource * CreateSingleFileRecvDataSource(DRIVER_TYPE type);
	void CalculateTotalPkts(ISingleFileRecvDataSource * pFileDataSource);

private:
	ISingleFileRecvDataSource * OpenByIndex(int index);

private:
	typedef std::pair<DRIVER_TYPE,std::string>	FILE_ITEM;

	std::vector<FILE_ITEM>	m_vtFileItem;

	ISingleFileRecvDataSource *	m_pCurDataSource;
	int							m_nCurIndex;//已经放到第几个文件

	ACE_UINT64					m_totalpkts;//数据源的总包数
	ACE_UINT64					m_totalbytes;//数据源的总字节数

	//当前包的时间是（基准时间+当前包的文件时间-当前文件第一个包的时间）
	CTimeStamp32			m_nCurFileFirstTime;//当前文件的第一个包的时间
	CTimeStamp32			m_nCurFileLastTime;//当前文件最后一个包的时间
	CTimeStamp32			m_nBaseTime;//基准时间

	int						m_re_calculate_time;//是否要重新计算时间
	int						m_bcur_systime;
	int						m_bskip_error_file;

	//对于cpt文件格式
	unsigned int	m_nReadNums;
	unsigned int	m_nBufferSizeMB;

	ACE_Recursive_Thread_Mutex	m_lock;



};


//////////////////////////////////////////////////////////////////////////
//CVPKMultiFileDataSource
//////////////////////////////////////////////////////////////////////////
#include "VPKHelpDataSource.h"

class PACKETIO_CLASS CVPKMultiFileDataSource : public CVPKHelpDataSource
{
public:
	CVPKMultiFileDataSource();
	virtual ~CVPKMultiFileDataSource();

	//不能多次调用,相当于init+AddMultiFileName+OpenDataSource
	BOOL Open(DRIVER_TYPE type,const CStdStringArray& vtFilename,int re_calculate_time=1);

	//不能多次调用,相当于init+AddFileName+OpenDataSource
	BOOL Open(DRIVER_TYPE type,const char * filename,int re_calculate_time=1);

	//相当于CloseDataSource+fini
	void Close();

	void init(int re_calculate_time=1);

	void fini();

	//一次调用传入多个文件，可以多次调用。文件会累加
	void AddMultiFileName(DRIVER_TYPE type,const CStdStringArray& vtFilename);

	//可以多次调用AddFileName。文件会累加
	void AddFileName(DRIVER_TYPE type,const char * filename);

private:
	CMultiFileDataSource *	m_pMultifileDataSource;
};
