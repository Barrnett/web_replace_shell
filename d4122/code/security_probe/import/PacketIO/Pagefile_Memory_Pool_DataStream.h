//////////////////////////////////////////////////////////////////////////
//Pagefile_Memory_Pool_DataStream.h

//��Pagefile_Memory_Pool_DataSource���Ӧ

#pragma once

#include "PacketIO/WriteDataStream.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/CPF_ShareMem.h"
#include "cpf/CommonQueueManager.h"

class PACKETIO_CLASS CPagefile_Memory_Pool_DataStream : public IFileWriteDataStream
{
public:
	CPagefile_Memory_Pool_DataStream(void);
	virtual ~CPagefile_Memory_Pool_DataStream(void);


	//�൱��init+OpenDataStream
	BOOL Open(LPCSTR sharename,DWORD memsize);

	//�൱��CloseDataStream+fini
	void Close();

	BOOL init(LPCSTR sharename,DWORD memsize);

	void fini();


	//�Ƿ�֧��д���ƣ�
	virtual BOOL IsSupportWriteCtrl() const
	{
		return false;
	}

	virtual IFileWriteDataStream * CloneAfterIni();

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataStream();

	//�ر��豸
	virtual void CloseDataStream();

	//�������ݣ��ɹ�����0,ʧ�ܷ���-1
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums);

	//�������ݣ��ɹ�����0,ʧ�ܷ���-1
	virtual int WritePacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,
		IN const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		int attach_len,const void * attach_data,
		int index_array,int try_nums);

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState();

	virtual bool GetDataStreamName(std::string& name);

public:
	ACE_UINT32 GetBufferUsed(OUT ACE_UINT32 * total=NULL) const;

private:
	CPF_ShareMem	m_cpf_share_mem;

private:
	size_t			m_memsize;
	std::string		m_strShareName;

	TAG_COMMOM_QUEUE_MANAGER	m_mem_que;

};


