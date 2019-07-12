//////////////////////////////////////////////////////////////////////////
//MultiFileWriteStream.h

#pragma once

#include "PacketIO/WriteDataStream.h"

class CWriteFileCtrl;

//ethereal方式和cpt方式一次只能选择一种
class PACKETIO_CLASS CMultiFileWriteStream : public IFileWriteDataStream
{
public:
	CMultiFileWriteStream(void);
	virtual ~CMultiFileWriteStream(void);

	BOOL init(IFileWriteDataStream * pFirstFileDataStream,
			unsigned int numfiles,
			unsigned int one_filsize_mb,
			unsigned int captime);

	void fini();

	//是否支持写控制．
	virtual BOOL IsSupportWriteCtrl() const
	{
		return true;
	}

	//clone一个对象，要求返回的对象是该对象init之后，OpenDataStream之前的状态
	//这样得到clone返回的对象后，可以OpenDataStream,CloseDataStream,WritePacket.
	virtual IFileWriteDataStream * CloneAfterIni()
	{
		ACE_ASSERT(FALSE);
		return NULL;
	}
	
public:
	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataStream();

	//关闭设备
	virtual void CloseDataStream();

	//0表示正常,-1表示设备出错，-2表示空间磁盘满，-3表示文件写完成了。
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums);

	virtual int GetConnectState();

	virtual bool GetDataStreamName(std::string& name);

private:
	void get_next_filename(const char * org_filename,std::string& strnext_filename);
	IFileWriteDataStream * CreateNextFileStream();

private:
	std::string				m_strOrgFilename;
	IFileWriteDataStream *	m_pCurFileDataStream;
	CWriteFileCtrl *		m_pWriteFileCtrl;
	int						m_nLastRtn;
};
