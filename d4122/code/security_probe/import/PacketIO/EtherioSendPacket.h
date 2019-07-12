//////////////////////////////////////////////////////////////////////////
//EtherioSendPacket.h


#pragma once


#include "PacketIO/PacketIO.h"
#include "PacketIO/EtherDataStreamBase.h"

class PACKETIO_CLASS CEtherioSendPacket : CEtherSendPacketBase
{
public:
	CEtherioSendPacket(void);
	virtual ~CEtherioSendPacket(void);

public:
	BOOL Open(int drv_load_type,
		unsigned int copy_data_to_send,
		const char * sysfilename=NULL,
		const char * drivername=NULL,
		const char * macaddr=NULL,
		unsigned int send_type=1,
		unsigned int memsize = 20);

	BOOL Open(int drv_load_type,
		unsigned int copy_data_to_send,
		const char * sysfilename=NULL,
		const char * drivername=NULL,
		int macindex_array=-1,
		unsigned int send_type=1,
		unsigned int memsize = 20);

	void Close();

	//��Open֮���ڷ�����ǰ�ȵ���StartToSend
	BOOL StartToSend();
	//�ڷ��ͽ�����Close֮ǰ����StopSending
	//bStopNow: �Ƿ�����ֹͣ������ǣ��򻺳���ʣ������ݲ���Ҫ���ͣ��������ʣ�����ݷ�����Ϻ��ٽ���
	void StopSending(BOOL bStopNow);

	BOOL SendPacket(const char * pdata,unsigned int datalen,int index_array,int try_nums);

public:
	//	 ����ֵ
	//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
	//
	//	 ˵��
	//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�

	ACE_UINT32 GetSendBufferUsed(OUT ACE_UINT32* pTotalLen);


private:
	BOOL Inner_Open(int devcount,
		unsigned int copy_data_to_send,
		const char * sysfilename,
		const char * drivername,
		int macindex_array,
		unsigned int send_type,
		unsigned int memsize);

private:
	BOOL IsAllSendOver();

/*
private:
	int					m_nNumsHandle;
	DWORD				m_vtSendHandle[32];
	int					m_indexarray;

	int					m_nDevCount;

	int				m_nMTU;
	unsigned int	m_send_type;
	*/
};


class PACKETIO_CLASS CEtherioWriteDataStream : public CEtherDataStreamBase
{
public:
	CEtherioWriteDataStream();
	virtual ~CEtherioWriteDataStream();

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState();

protected:
	virtual const char* GetConstDriverName();

};

