//////////////////////////////////////////////////////////////////////////
//WinpCapDataSource.h

#ifndef WIN_PCAP_DATA_SOURCE_H_2006_04_27
#define WIN_PCAP_DATA_SOURCE_H_2006_04_27
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS


#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"


class adapterobj;

typedef struct tagWINPCAP_RecvDRV_PARAM
{
	int		cardIndex;
	int filterMode;
	int driveBuffer;
	int readBuffer;

	int minToCopy;
	int readTimeout;

}WINPCAP_RecvDRV_PARAM;


//////////////////////////////////////////////////////////////////////////
//CWinpCapDataSource
//////////////////////////////////////////////////////////////////////////
class PACKETIO_CLASS CWinpCapDataSource : public IRealDataSource  
{
public:
	CWinpCapDataSource();
	virtual ~CWinpCapDataSource();

public:
	virtual BOOL Open(const WINPCAP_RecvDRV_PARAM& param)
	{
		if( init(param) == FALSE )
			return false;

		return (OpenDataSource() == 0);
	}

	virtual void Close()
	{
		CloseDataSource();
		fini();
	}

	virtual BOOL init(const WINPCAP_RecvDRV_PARAM& param);
	virtual void fini();

	virtual int OpenDataSource();
	virtual void CloseDataSource();

	
public:
		//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();
	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();
	
	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet) = 0;

	//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes);

	virtual int GetConnectState();
	
protected:
	void *			m_pPacket;
	PACKET_HEADER	m_headerInfo;

	BOOL	ReadDataFromHW();
	void	FreePacket();
	
protected:
	adapterobj * m_pAdapterObj;
	WINPCAP_RecvDRV_PARAM	m_winpcap_drv_param;

	ACE_UINT64		m_ullDroppedPkts;
	ACE_UINT64		m_ullDroppedBytes;
};

//////////////////////////////////////////////////////////////////////////
//CSingleCardWinpCapDataSource
//////////////////////////////////////////////////////////////////////////

class PACKETIO_CLASS CSingleCardWinpCapDataSource : public CWinpCapDataSource
{
public:
	CSingleCardWinpCapDataSource();
	virtual ~CSingleCardWinpCapDataSource();

public:
	virtual BOOL init(const WINPCAP_RecvDRV_PARAM& param);
	virtual void fini();

	virtual bool GetDataSourceName(std::string& name);


	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

private:
	BOOL SingleCard_ReadDataFromHW();

	ULONG	m_ulOff;
	ULONG	m_ulBytesReceived;


};

//////////////////////////////////////////////////////////////////////////
//CDoubleCardWinpCapDataSource
//////////////////////////////////////////////////////////////////////////

class PACKETIO_CLASS CDoubleCardWinpCapDataSource : public CWinpCapDataSource
{
public:
	CDoubleCardWinpCapDataSource();
	virtual ~CDoubleCardWinpCapDataSource();

public:
	virtual BOOL init(const WINPCAP_RecvDRV_PARAM& param);
	virtual void fini();

	virtual bool GetDataSourceName(std::string& name);


	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);


private:
	BOOL DoubleCard_ReadDataFromHW();

private:
	int		m_nPacketNums;
	char *	m_pCurBuffer;

	
};

//////////////////////////////////////////////////////////////////////////
//CVPKWinpCapDataSource
//////////////////////////////////////////////////////////////////////////
#include "VPKHelpDataSource.h"

class PACKETIO_CLASS CVPKWinpCapDataSource : public CVPKHelpDataSource
{
public:
	CVPKWinpCapDataSource();
	virtual ~CVPKWinpCapDataSource();

	BOOL Open(int nopen,int type,const WINPCAP_RecvDRV_PARAM& param);
	void Close();

private:
	CWinpCapDataSource *	m_pWinpcapDataSource;
};

#endif//OS_WINDOWS

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//WIN_PCAP_DATA_SOURCE_H_2006_04_27



