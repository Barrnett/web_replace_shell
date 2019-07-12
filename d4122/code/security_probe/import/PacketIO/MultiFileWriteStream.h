//////////////////////////////////////////////////////////////////////////
//MultiFileWriteStream.h

#pragma once

#include "PacketIO/WriteDataStream.h"

class CWriteFileCtrl;

//ethereal��ʽ��cpt��ʽһ��ֻ��ѡ��һ��
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

	//�Ƿ�֧��д���ƣ�
	virtual BOOL IsSupportWriteCtrl() const
	{
		return true;
	}

	//cloneһ������Ҫ�󷵻صĶ����Ǹö���init֮��OpenDataStream֮ǰ��״̬
	//�����õ�clone���صĶ���󣬿���OpenDataStream,CloseDataStream,WritePacket.
	virtual IFileWriteDataStream * CloneAfterIni()
	{
		ACE_ASSERT(FALSE);
		return NULL;
	}
	
public:
	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataStream();

	//�ر��豸
	virtual void CloseDataStream();

	//0��ʾ����,-1��ʾ�豸����-2��ʾ�ռ��������-3��ʾ�ļ�д����ˡ�
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
