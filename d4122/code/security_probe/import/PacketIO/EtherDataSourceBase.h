//////////////////////////////////////////////////////////////////////////
// EtherDataSourceBase.h 

#pragma once

#include "cpf/ostypedef.h"

//#ifdef OS_WINDOWS

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

class PACKETIO_CLASS CEtherDataSourceBase : public IRealDataSource
{
public:
	CEtherDataSourceBase(void);
	virtual ~CEtherDataSourceBase(void);

	//�൱��init+OpenDataSource
	BOOL Open(int drv_load_type,const char * sysfilename,const char *drivername,const char * macaddr,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code);

	BOOL Open(int drv_load_type,const char * sysfilename,const char *drivername,int macindex_array,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code);


	//�൱��CloseDataSource()+fini
	void Close();

	bool GetDataSourceName(std::string& name);

	virtual BOOL init(int drv_load_type,const char * sysfilename,const char *drivername,const char * macaddr,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code) = 0;

	virtual BOOL init(int drv_load_type,const char * sysfilename,const char *drivername,int macindex_array,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code) = 0;

	virtual void fini() = 0;

	//��������ڳ�ʼ��init(..)֮��,��fini(..)֮ǰ����OpenDataSource��CloseDataSource

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataSource() = 0;

	//�ر��豸
	virtual void CloseDataSource() = 0;


	//��������ڳ�ʼ��OpenDataSource(..)֮��,��StopRecving(..)֮ǰ����StartToRecv��StopRecving

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv() = 0;

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving() = 0;

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet) = 0;

	//ʹ����ӿ��ܹ�������ʱ��
	//������óɹ�������true�����򷵻�false.
	//ʹ���߱����ڿ�ʼ������ǰ��������������ڿ�ʼ�����Ժ����������
	BOOL EnableTimerPacket(BOOL bEnable);

	//ѯ������豸�Ƿ���Բ�����ʱ����
	BOOL IsEnableTimerPacket() const;

	//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ,-1��ʾ��Ч
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes) = 0;

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState() = 0;

	virtual int WaitForPacket(const ACE_Time_Value& sleep_time) = 0;

public:
	//	 ����ֵ
	//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
	//
	//	 ˵��
	//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�
	virtual ACE_UINT32 GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen) = 0;

protected:
	void GetSysAndDriverName(const char * sysfilename,const char *drivername);
	virtual const char* GetConstDriverName() = 0;

protected:
	int				m_indexarray;
	std::string		m_strSysFileName;
	std::string		m_strDriverName;
	std::string		m_strMacaddrInfo;

	unsigned int	m_nMemSize;
	BOOL			m_bEnableTimerPacket;
	int				m_nTimeStampType;
	BOOL			m_bErrorRecover;

	HANDLE			m_dwRecvHandle;
	int				m_nDevCount;//�����ĸ���

	PACKET_HEADER	m_packet_header;
};



//#endif//OS_WINDOWS

