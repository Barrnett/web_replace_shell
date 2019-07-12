// EtherrealDataSource.h: interface for the CEtherrealDataSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERREALDATASOURCE_H__B86804C2_DC90_45B6_A94C_E36891A04B37__INCLUDED_)
#define AFX_ETHERREALDATASOURCE_H__B86804C2_DC90_45B6_A94C_E36891A04B37__INCLUDED_

#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"

#include "pcap.h"


//////////////////////////////////////////////////////////////////////////
//CEtherrealDataSource
//////////////////////////////////////////////////////////////////////////
class PACKETIO_CLASS CEtherrealDataSource : public ISingleFileRecvDataSource  
{
public:
	CEtherrealDataSource();
	virtual ~CEtherrealDataSource();

	virtual bool GetDataSourceName(std::string& name);

public:
	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();

	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin();

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState();

public:

	virtual int OpenDataSource();
	virtual void CloseDataSource();

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	//ethereal��encap_type,Ӱ��ɱ�׼��Э��id
#ifdef OS_WINDOWS
	static ACE_UINT32 EtherealEncapetype_Map_StdProtoId(ACE_UINT32 nStdProtoId);
#endif
private:
//	void *			m_fileHandle;
	pcap_t *			m_fileHandle;
	long			m_data_offset;
	PACKET_HEADER	m_headerInfo; 
	CTimeStamp32	m_lastPktStamp;

};



//////////////////////////////////////////////////////////////////////////
//CVPEtherrealDataSource
//////////////////////////////////////////////////////////////////////////
#include "VPKHelpDataSource.h"

class PACKETIO_CLASS CVPKEtherrealDataSource : public CVPKHelpDataSource
{
public:
	CVPKEtherrealDataSource();
	virtual ~CVPKEtherrealDataSource();

public:
	BOOL Open(const char *filename,BOOL bcur_systime);
	void Close();


private:
	CEtherrealDataSource * m_pEtherealDataSource;

};



#endif // !defined(AFX_ETHERREALDATASOURCE_H__B86804C2_DC90_45B6_A94C_E36891A04B37__INCLUDED_)
