//////////////////////////////////////////////////////////////////////////
//CPTWriteDataStream.h


#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/WriteDataStream.h"

class CCapSave;

class PACKETIO_CLASS CCPTWriteDataStream : public IFileWriteDataStream
{
public:
	CCPTWriteDataStream(void);
	virtual ~CCPTWriteDataStream(void);

	//相当于init+OpenDataStream
	BOOL Open(LPCSTR szFileName,
		DWORD dwMaxFileLengthMb					= 50,
		DWORD dwBufSize							= 1*1024*1024,	// 缓冲大小
		int multifiles							= 1,
		unsigned int ncaptime					= 0
		);

	//相当于CloseDataStream+fini
	void Close();

	// 初始化，要求szFileName中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
	//init之后调用OpenDataStream
	BOOL init(LPCSTR szFileName,
		DWORD dwMaxFileLengthMb					= 50,
		DWORD dwBufSize							= 1*1024*1024,	// 缓冲大小
		int multifiles							= 1,
		unsigned int ncaptime					= 0
		);
	void fini();


	//是否支持写控制．
	virtual BOOL IsSupportWriteCtrl() const;

	virtual IFileWriteDataStream * CloneAfterIni();

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataStream();

	//关闭设备
	virtual void CloseDataStream();

	//保存数据，成功返回0,失败返回-1
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums);

	//获取设备的连接信息，1表示连接，0表示没有连接，-1表示未知
	virtual int GetConnectState();

	virtual bool GetDataStreamName(std::string& name);

private:
	DWORD		m_dwMaxFileLengthMb;
	DWORD		m_dwBufSize;
	int			m_nMultiFiles;
	unsigned int	m_ncaptime;
	CCapSave *	m_pCapSave;
	void *		m_pHeaderInfo;

};
