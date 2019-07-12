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

	//�൱��init+OpenDataStream
	BOOL Open(LPCSTR szFileName,
		DWORD dwMaxFileLengthMb					= 50,
		DWORD dwBufSize							= 1*1024*1024,	// �����С
		int multifiles							= 1,
		unsigned int ncaptime					= 0
		);

	//�൱��CloseDataStream+fini
	void Close();

	// ��ʼ����Ҫ��szFileName�в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
	//init֮�����OpenDataStream
	BOOL init(LPCSTR szFileName,
		DWORD dwMaxFileLengthMb					= 50,
		DWORD dwBufSize							= 1*1024*1024,	// �����С
		int multifiles							= 1,
		unsigned int ncaptime					= 0
		);
	void fini();


	//�Ƿ�֧��д���ƣ�
	virtual BOOL IsSupportWriteCtrl() const;

	virtual IFileWriteDataStream * CloneAfterIni();

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataStream();

	//�ر��豸
	virtual void CloseDataStream();

	//�������ݣ��ɹ�����0,ʧ�ܷ���-1
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums);

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
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
