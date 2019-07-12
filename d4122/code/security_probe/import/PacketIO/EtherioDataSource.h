//////////////////////////////////////////////////////////////////////////
//EtherioDataSource.h

#pragma once

#include "cpf/ostypedef.h"

//#ifdef OS_WINDOWS

#include "PacketIO/PacketIO.h"
#include "PacketIO/EtherDataSourceBase.h"

class PACKETIO_CLASS CEtherioDataSource : public CEtherDataSourceBase
{
public:
	CEtherioDataSource(void);
	virtual ~CEtherioDataSource(void);


	BOOL init(int drv_load_type,const char * sysfilename,const char *drivername,const char * macaddr,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code);

	BOOL init(int drv_load_type,const char * sysfilename,const char *drivername,int macindex_array,
		unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		int * error_code);

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

	//ʹ����ӿ��ܹ�������ʱ��
	//������óɹ�������true�����򷵻�false.
	//ʹ���߱����ڿ�ʼ������ǰ��������������ڿ�ʼ�����Ժ����������
//	virtual BOOL EnableTimerPacket(BOOL bEnable);

	//ѯ������豸�Ƿ���Բ�����ʱ����
//	virtual BOOL IsEnableTimerPacket() const;

	//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ,-1��ʾ��Ч
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes);

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState();

	virtual int WaitForPacket(const ACE_Time_Value& sleep_time);

public:
	//	 ����ֵ
	//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
	//
	//	 ˵��
	//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�
	ACE_UINT32 GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen);

protected:
	virtual const char* GetConstDriverName();

};


//#endif//OS_WINDOWS

