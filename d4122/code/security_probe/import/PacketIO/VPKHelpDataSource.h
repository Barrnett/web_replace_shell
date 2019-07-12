// VPKHelpDataSource.h: interface for the CVPKHelpDataSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VPKHELPDATASOURCE_H__0C76F3AD_E5AC_4D39_A628_025A9E2E5FEB__INCLUDED_)
#define AFX_VPKHELPDATASOURCE_H__0C76F3AD_E5AC_4D39_A628_025A9E2E5FEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"
#include "PacketIO/VPKGenerator.h"


class PACKETIO_CLASS CVPKHelpDataSource : public IExtDataSource  
{
public:
	CVPKHelpDataSource();
	virtual ~CVPKHelpDataSource();

	BOOL Open(IRecvDataSource * pRealDataSource,int delete_real_source = 0)
	{
		init(pRealDataSource,delete_real_source);

		return (OpenDataSource()==0);
	}
	void Close()
	{
		CloseDataSource();
		fini();
	}


	void init(IRecvDataSource * pRealDataSource,int delete_real_source = 0)
	{
		IExtDataSource::init(pRealDataSource,delete_real_source);
	}

	void fini()
	{
		IExtDataSource::fini();
	}

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();

	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin();

	//�̳��߲�Ҫʵ���������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//ʹ����ӿ��ܹ�������ʱ��
	virtual BOOL EnableTimerPacket(BOOL bEnable)
	{
		m_bEnableVirtualPacket = bEnable;
		return true;
	}
	//ѯ������豸�Ƿ���Բ�����ʱ����
	virtual BOOL IsEnableTimerPacket() const
	{
		return m_bEnableVirtualPacket;
	}
		
private:
	//��m_bEnableVirtualPacket=true�������������
	PACKET_LEN_TYPE	GetPacketWithVirtualPacket(RECV_PACKET& packet);

	inline BOOL IsBackupLastPkt() const
	{
		return (PACKET_EMPTY != m_bkType);
	}

private:
	CTimeStamp32	m_LastEffectivePacketStamp;
	PACKET_HEADER	m_virtualHeaderInfo;
	CVPKGenerator	m_vpkGenrator;
	
	//������һ�ζ����˰�������
	RECV_PACKET		m_bkRecvPacket;

	//������һ�ζ����˰������ݵĳ�������
	PACKET_LEN_TYPE	m_bkType;

	//�Ƿ����������ʱ��
	BOOL			m_bEnableVirtualPacket;

};

#endif // !defined(AFX_VPKHELPDATASOURCE_H__0C76F3AD_E5AC_4D39_A628_025A9E2E5FEB__INCLUDED_)
