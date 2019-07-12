//////////////////////////////////////////////////////////////////////////
//TimeAdjustDataSource.h

#pragma once

#include "PacketIO/RecvDataSource.h"
#include "PacketIO/PacketIO.h"

class CTimeAdjustTool;

class PACKETIO_CLASS CTimeAdjustDataSource : public IExtDataSource
{
public:
	CTimeAdjustDataSource(void);
	virtual ~CTimeAdjustDataSource(void);

	//nAdjustType;����ʱ�����ͣ�Ϊ0��ʾ��������
	//  Ϊ1��ʾʹ��ϵͳ��ǰʱ��������������ÿ������ʱ��
	//	;Ϊ2��ʾʹ�þ���ʱ��m_strAdjustBeginTime��������Ŀǰ��֧�֣�
	//	;���ļ������ݵ�ʱ���µ�ÿ������ʱ�� = m_strAdjustBeginTime+(����ʱ��-��һ������ʱ�䣩
	//	;ȱʡΪ0

	//nAdjustType=2��ʱ��pAdjustTimeStamp��Ч:���ΪNULL����ʾ������
	//���pAdjustTimeStamp->sec,pAdjustTimeStamp->nsΪ0����ʾʹ�õ�ǰʱ��������
	//����ʹ��ָ����ʱ��������
	BOOL Open(IRecvDataSource * pRealDataSource,int delete_real_source,
		int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp)
	{
		init(pRealDataSource,delete_real_source,nAdjustType,pAdjustTimeStamp);

		return (OpenDataSource()==0);
	}

	void Close()
	{
		CloseDataSource();
		fini();
	}


	//nAdjustType=2��ʱ��pAdjustTimeStamp��Ч:���ΪNULL����ʾ������
	//����pAdjustTimeStamp->sec,pAdjustTimeStamp->nsΪ0����ʾʹ�õ�ǰʱ�������˵���
	//����ʹ��ָ����ʱ��������
	void init(IRecvDataSource * pRealDataSource,int delete_real_source,
		int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp);
	
	void fini();

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);


private:
	CTimeAdjustTool *	m_pTimeAdjustTool;
};
