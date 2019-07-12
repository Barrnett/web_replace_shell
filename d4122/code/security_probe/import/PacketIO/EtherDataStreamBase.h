//////////////////////////////////////////////////////////////////////////
// EtherDataStreamBase.h 


#pragma once


#include "PacketIO/PacketIO.h"
#include "PacketIO/WriteDataStream.h"

class PACKETIO_CLASS CEtherSendPacketBase
{
public:
	CEtherSendPacketBase(void);
	virtual ~CEtherSendPacketBase(void);

public:
	virtual BOOL Open(int drv_load_type,
		unsigned int copy_data_to_send,
		const char * sysfilename=NULL,
		const char * drivername=NULL,
		const char * macaddr=NULL,
		unsigned int send_type=1,
		unsigned int memsize = 20) = 0;

	virtual BOOL Open(int drv_load_type,
		unsigned int copy_data_to_send,
		const char * sysfilename=NULL,
		const char * drivername=NULL,
		int macindex_array=-1,
		unsigned int send_type=1,
		unsigned int memsize = 20) = 0;

	virtual void Close() = 0;

	//��Open֮���ڷ�����ǰ�ȵ���StartToSend
	virtual BOOL StartToSend() = 0;
	//�ڷ��ͽ�����Close֮ǰ����StopSending
	//bStopNow: �Ƿ�����ֹͣ������ǣ��򻺳���ʣ������ݲ���Ҫ���ͣ��������ʣ�����ݷ�����Ϻ��ٽ���
	virtual void StopSending(BOOL bStopNow) = 0;

	virtual BOOL SendPacket(const char * pdata,unsigned int datalen,int index_array,int try_nums) = 0;

public:
	//	 ����ֵ
	//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
	//
	//	 ˵��
	//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�

	virtual ACE_UINT32 GetSendBufferUsed(OUT ACE_UINT32* pTotalLen) = 0;


/*
protected:
	virtual BOOL Inner_Open(int devcount,
		const char * sysfilename,
		const char * drivername,
		int macindex_array,
		unsigned int send_type,
		unsigned int memsize) = 0;

	virtual BOOL IsAllSendOver() = 0;
*/

protected:
	int					m_nNumsHandle;
	void*				m_vtSendHandle[32];
	int					m_indexarray;

	int					m_nDevCount;

	int				m_nMTU;
	unsigned int	m_send_type;
};


class PACKETIO_CLASS CEtherDataStreamBase : public IDevWriteDataStream
{
public:
	CEtherDataStreamBase();
	virtual ~CEtherDataStreamBase();

public:
	//�൱��init+OpenDataStream
	virtual BOOL Open(int drv_load_type,unsigned int copy_data_to_send,const char * sysfilename=NULL,const char *drivername=NULL,
		const char * macaddr=NULL,unsigned int send_type=1,unsigned int memsize=20);

	virtual BOOL Open(int drv_load_type,unsigned int copy_data_to_send,const char * sysfilename=NULL,const char *drivername=NULL,
		int macindex_array=-1,unsigned int send_type=1,unsigned int memsize=20);

	//�൱��CloseDataStream+fini
	virtual void Close();

	virtual BOOL init(int drv_load_type,unsigned int copy_data_to_send,const char * sysfilename=NULL,const char *drivername=NULL,
		const char * macaddr=NULL,unsigned int send_type=1,unsigned int memsize=20);

	virtual BOOL init(int drv_load_type,unsigned int copy_data_to_send,const char * sysfilename=NULL,const char *drivername=NULL,
		int macindex_array=-1,unsigned int send_type=1,unsigned int memsize=20);

	virtual void fini();

	virtual int OpenDataStream();

	//�ر��豸
	virtual void CloseDataStream();


	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToWrite();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopWriting(BOOL bStopNow=false);

	virtual int WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums);

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
//	virtual int GetConnectState();

	virtual bool GetDataStreamName(std::string& name);

public:
	//	 ����ֵ
	//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
	//
	//	 ˵��
	//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�

	ACE_UINT32 GetSendBufferUsed(OUT ACE_UINT32* pTotalLen);

protected:
	void GetSysAndDriverName(const char * sysfilename,const char *drivername);
	virtual const char* GetConstDriverName() = 0;

protected:
	CEtherSendPacketBase*	m_pEtherSendPacket;

	int					m_drv_load_type;
	unsigned int		m_copy_data_to_send;
	unsigned int		m_nMemSize;
	unsigned int		m_send_type;

	int					m_indexarray;
	std::string			m_strMacAddr;
	std::string			m_strSysFileName;
	std::string			m_strDriverName;
};

