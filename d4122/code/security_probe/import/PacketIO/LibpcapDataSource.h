//////////////////////////////////////////////////////////////////////////
//LibpcapDataSource.h

#pragma once

#include "cpf/ostypedef.h"

#ifdef OS_LINUX
//////////////////////////////////////////////////////////////////////////

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

#include <pcap.h>
#include <linux/if_ether.h>

class PACKETIO_CLASS CLibpcapDataSource : public IRealDataSource
{
public:
	CLibpcapDataSource(void);
	virtual ~CLibpcapDataSource(void);

public:
	//�൱��init+OpenDataSource
	BOOL Open(const char * if_name);

	//�൱��CloseDataSource()+fini
	void Close();

	virtual bool GetDataSourceName(std::string& name);

	BOOL init(const char * if_name);

	void fini(); 

	//��������ڳ�ʼ��init(..)֮��,��fini(..)֮ǰ����OpenDataSource��CloseDataSource

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataSource();

	//�ر��豸
	virtual void CloseDataSource();

	//��������ڳ�ʼ��OpenDataSource(..)֮��,��StopRecving(..)֮ǰ����StartToRecv��StopRecving

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState();

private:
	std::string			m_strIfName;

	pcap_t *			m_pd_src;

	struct pcap_pkthdr	m_header;
	PACKET_HEADER		m_packet_header;

};

//////////////////////////////////////////////////////////////////////////
#endif//OS_LINUX

