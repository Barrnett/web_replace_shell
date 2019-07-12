//////////////////////////////////////////////////////////////////////////
//FileDataSourceHelper

#pragma once

#include "PacketIO/RecvDataSource.h"
#include "PacketIO/PacketIO.h"

//class IFileRecvDataSource;

class PACKETIO_CLASS CFileDataSourceHelper : public IRecvDataSource
{
public:
	CFileDataSourceHelper(void);
	virtual ~CFileDataSourceHelper(void);

public:
	BOOL Open(const char * filename,DRIVER_TYPE type)
	{
		if( init(filename,type) == FALSE )
			return false;

		return OpenDataSource() == 0;
	}

	void Close()
	{
		CloseDataSource();
		fini();
	}

	//type��ʾ�ļ����ͣ�0��ʾ��ethreal������ʶ�ĸ�ʽ��1��cpt
	BOOL init(const char * filename,DRIVER_TYPE type);
	void fini();

	virtual int OpenDataSource();

	//�ر��豸
	virtual void CloseDataSource();


	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv()
	{
		if( m_pFileRecvDataSource )
			return m_pFileRecvDataSource->StartToRecv();

		return false;
	}
	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving()
	{
		if( m_pFileRecvDataSource )
			return m_pFileRecvDataSource->StopRecving();

	}

	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin()
	{
		if( m_pFileRecvDataSource )
			return m_pFileRecvDataSource->SeekToBegin();

		return false;
	}

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet)
	{
		return m_pFileRecvDataSource->GetPacket(packet);
	}

	//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
	{
		if( m_pFileRecvDataSource )
		{
			m_pFileRecvDataSource->GetDroppedPkts(pkts,bytes);
		}
		else
		{
			pkts = bytes = 0;
		}
		
	}

private:
	IFileRecvDataSource *	m_pFileRecvDataSource;

};
