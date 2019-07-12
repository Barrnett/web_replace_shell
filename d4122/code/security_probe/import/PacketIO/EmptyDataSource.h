//////////////////////////////////////////////////////////////////////////
//EmptyDataSource.h

#pragma once

#include "cpf/ostypedef.h"

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"


class PACKETIO_CLASS CEmptyDataSource : public IRealDataSource
{
public:
	CEmptyDataSource(void);
	virtual ~CEmptyDataSource(void);

	//��������ڳ�ʼ��init(..)֮��,��fini(..)֮ǰ����OpenDataSource��CloseDataSource

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataSource()
	{
		return 0;
	}

	//�ر��豸
	virtual void CloseDataSource()
	{
		return;
	}

	//��������Դ�ĵ�����
	virtual bool GetDataSourceName(std::string& name);

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv()
	{
		memset(&m_packet_header,0x00,sizeof(m_packet_header));

		m_start_time = ACE_OS::gettimeofday();

		return true;
	}
	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving()
	{
		return;
	}

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//ʹ����ӿ��ܹ�������ʱ��
	//������óɹ�������true�����򷵻�false.
	//ʹ���߱����ڿ�ʼ������ǰ��������������ڿ�ʼ�����Ժ����������
	virtual BOOL EnableTimerPacket(BOOL bEnable)
	{
		return true;
	}

	//ѯ������豸�Ƿ���Բ�����ʱ����
	virtual BOOL IsEnableTimerPacket() const
	{
		return true;
	}

	//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ,-1��ʾ��Ч
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
	{
		pkts = 0;
		bytes = 0;
	}

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState()
	{
		return 1;
	}

private:
	ACE_Time_Value m_start_time;

	PACKET_HEADER	m_packet_header;

};


