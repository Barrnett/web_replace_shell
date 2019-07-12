//////////////////////////////////////////////////////////////////////////
//Pagefile_Memory_Pool_DataSource.h

//��Pagefile_Memory_Pool_DataStream���Ӧ

#pragma once

#include "PacketIO/RecvDataSource.h"
#include "cpf/CPF_ShareMem.h"
#include "cpf/CommonQueueManager.h"

class PACKETIO_CLASS CPagefile_Memory_Pool_DataSource : public IRealDataSource
{
public:
	CPagefile_Memory_Pool_DataSource(void);
	virtual ~CPagefile_Memory_Pool_DataSource(void);


	//�൱��init+OpenDataSource
	BOOL Open(const char * share_name,size_t memsize);

	//�൱��CloseDataSource()+fini
	void Close();

	virtual bool GetDataSourceName(std::string& name);

	BOOL init(const char * share_name,size_t memsize);

	void fini(); 

	//��������ڳ�ʼ��init(..)֮��,��fini(..)֮ǰ����OpenDataSource��CloseDataSource

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataSource();

	//�ر��豸
	virtual void CloseDataSource();


	//��������ڳ�ʼ��OpenDataSource(..)֮��,��StopRecving(..)֮ǰ����StartToRecv��StopRecving

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv()
	{
		return true;
	}

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving()
	{
		return;
	}

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//ѯ������豸�Ƿ���Բ�����ʱ����
	virtual BOOL IsEnableTimerPacket() const;

	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes);

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState();

public:
	//	 ����ֵ
	//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
	//
	//	 ˵��
	//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�
	ACE_UINT32 GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen);


private:
	std::string		m_strShareName;
	size_t			m_memsize;

private:
	CPF_ShareMem	m_cpf_share_mem;

	TAG_COMMOM_QUEUE_MANAGER	m_mem_que;

};
