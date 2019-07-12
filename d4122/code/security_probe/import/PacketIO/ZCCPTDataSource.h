//////////////////////////////////////////////////////////////////////////
//ZCCPTDataSource.h

#pragma once


#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

class CCapDataRead;

class PACKETIO_CLASS CZCCPTDataSource : public ISingleFileRecvDataSource
{
public:
	CZCCPTDataSource(void);
	virtual ~CZCCPTDataSource(void);

public:
	virtual BOOL Open(const char * filename,BOOL bcur_systime,
		unsigned int nReadNums,unsigned int bufferSizeMB)
	{
		m_nReadNums = nReadNums;
		m_nBufferSizeMB = bufferSizeMB;

		return ISingleFileRecvDataSource::Open(filename,bcur_systime);
	}

	//�������Դͳ���ж��ٸ��������ٸ��ֽ�,-1��ʾ��Ч
	//�ú���������OpenDataSource���سɹ�����ܵ���

	//һ������ļ����͵�����Դ��������
	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops);


	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();

	virtual bool GetDataSourceName(std::string& name);

	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin();

	virtual int GetConnectState();

public:
	virtual int OpenDataSource();
	virtual void CloseDataSource();

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

private:
	CCapDataRead *	m_pCapDataRead;
	PACKET_HEADER	m_headerInfo; 
	CTimeStamp32	m_lastPktStamp;

	unsigned int	m_nReadNums;
	unsigned int	m_nBufferSizeMB;


};


//////////////////////////////////////////////////////////////////////////
//CVPKZCCPTDataSource
//////////////////////////////////////////////////////////////////////////
#include "VPKHelpDataSource.h"

class PACKETIO_CLASS CVPKZCCPTDataSource : public CVPKHelpDataSource
{
public:
	CVPKZCCPTDataSource();
	virtual ~CVPKZCCPTDataSource();

public:
	BOOL Open(const char *filename,BOOL bcur_systime);
	void Close();

private:
	CZCCPTDataSource *	m_pZCCPTDataSource;

};

