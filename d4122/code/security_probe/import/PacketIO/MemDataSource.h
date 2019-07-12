//////////////////////////////////////////////////////////////////////////
//MemDataSource.h

//���ڴ��ڴ�������һЩ���ȵ����ݰ�����Ҫ���ڽ������ܲ��Ե���Ҫ��

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"


class PACKETIO_CLASS CMemDataSource : public IRealDataSource
{
public:
	CMemDataSource(void);
	virtual ~CMemDataSource(void);

public:
	//�൱��init+OpenDataSource
	BOOL Open(unsigned int minlen,unsigned int maxlen,WORD wdProto=2,
		const unsigned char * pModelData=NULL,unsigned int modeldata_len=0);

	//�൱��CloseDataSource()+fini
	void Close();

	virtual bool GetDataSourceName(std::string& name);

	//ϣ���������ݵĳ��ȷ�Χ[minlen,maxlen]��������ݵĳ����Ǵ�[minlen,maxlen]��������1��
	//�������ݵ�Э�����(����MACЭ��=2)
	//ϣ���������ݵ�ģ�棬���û���������ɽ���������ݡ�
	void init(unsigned int minlen,unsigned int maxlen,WORD wdProto=2,
		const unsigned char * pModelData=NULL,unsigned int modeldata_len=0);

	void fini();

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataSource();

	//�ر��豸
	virtual void CloseDataSource();

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);


	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin()
	{
		return TRUE;
	}

	//��������λ��ָ������ĵط���
	virtual BOOL SeekToEnd()
	{
		return TRUE;
	}

	//��������λ��ָ������ĵط���
	virtual BOOL SeekToOffset(ACE_INT64 offset)
	{
		return false;
	}

	//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ,-1��ʾ��Ч
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes);

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState();


private:
	//���������ȣ���ʾ�����İ���Ҫͬһ����С
	unsigned int	m_nMinLen;//��Ҫ����������С����
	unsigned int	m_nMaxLen;//��Ҫ����������󳤶�

	unsigned char *	m_pModelData;

	PACKET_HEADER	m_packetheader;
	unsigned int	m_nCurLen;
};
