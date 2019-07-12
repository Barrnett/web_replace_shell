//////////////////////////////////////////////////////////////////////////
//RecvDataSource.h

#ifndef __RECV_DATA_SOURCE_H_2006_4_26
#define __RECV_DATA_SOURCE_H_2006_4_26
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "PacketIO/PacketIO.h"

class PACKETIO_CLASS IRecvDataSource
{
public:
	IRecvDataSource()
	{

	}
	virtual ~IRecvDataSource()
	{

	}

	virtual DRIVER_TYPE GetDrvType() const = 0;

	BOOL IsPlaybackDataSource() const;

public:
	//��������ڳ�ʼ��init(..)֮��,��fini(..)֮ǰ����OpenDataSource��CloseDataSource

	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataSource() = 0;
	
	//�ر��豸
	virtual void CloseDataSource()
	{
		return;
	}

	//��������Դ�ĵ�����
	virtual bool GetDataSourceName(std::string& name) = 0;

	//�������Դͳ���ж��ٸ��������ٸ��ֽ�,-1��ʾ��Ч
	//�ú���������OpenDataSource���سɹ�����ܵ���

	//һ������ļ����͵�����Դ��������
	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops)
	{	pkts = bytes = -1;	nloops = 1;}


	//��������ڳ�ʼ��OpenDataSource(..)֮��,��StopRecving(..)֮ǰ����StartToRecv��StopRecving

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv()
	{
		return true;
	}
	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving()
	{
		return;
	}
	
	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet)=0;
	
	//ʹ����ӿ��ܹ�������ʱ��
	//������óɹ�������true�����򷵻�false.
	//ʹ���߱����ڿ�ʼ������ǰ��������������ڿ�ʼ�����Ժ����������
	virtual BOOL EnableTimerPacket(BOOL bEnable)
	{
		return false;
	}
	//ѯ������豸�Ƿ���Բ�����ʱ����
	virtual BOOL IsEnableTimerPacket() const
	{
		return false;
	}

	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin()
	{
		return false;
	}

	//��������λ��ָ������ĵط���
	virtual BOOL SeekToEnd()
	{
		return false;
	}

	//��������λ��ָ������ĵط���
	virtual BOOL SeekToOffset(ACE_INT64 offset)
	{
		return false;
	}

	//��û�л�ȡ�������ȴ����ݰ�
	//����0��ʾ�����ݰ�������1��ʾ��ʱ,-1��ʾ����
	//����usͬWaitForSingleObject()�еĲ��������������us
	virtual int WaitForPacket(const ACE_Time_Value& sleep_time)
	{
		ACE_OS::sleep(sleep_time);

		return 1;
	}

	//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ,-1��ʾ��Ч
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes) = 0;

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState() = 0;
};

//////////////////////////////////////////////////////////////////////////
//IExtDataSource
//������չװ��ģʽ���ܵ���չ����

class PACKETIO_CLASS IExtDataSource : public IRecvDataSource
{
public:
	IRecvDataSource * GetRealDataSource() const
	{
		return m_pRealDataSource;
	}

	virtual DRIVER_TYPE GetDrvType() const
	{
		if( m_pRealDataSource )
		{
			return m_pRealDataSource->GetDrvType();
		}

		return DRIVER_TYPE_NONE;
	}

public:
	virtual int OpenDataSource();

	virtual void CloseDataSource();


	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes);

	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops);

	//ʹ����ӿ��ܹ�������ʱ��
	//������óɹ�������true�����򷵻�false.
	//ʹ���߱����ڿ�ʼ������ǰ��������������ڿ�ʼ�����Ժ����������
	virtual BOOL EnableTimerPacket(BOOL bEnable)
	{
		return m_pRealDataSource->EnableTimerPacket(bEnable);
	}

	//ѯ������豸�Ƿ���Բ�����ʱ����
	virtual BOOL IsEnableTimerPacket() const
	{
		return m_pRealDataSource->IsEnableTimerPacket();
	}

	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin();

	virtual bool GetDataSourceName(std::string& name)
	{
		if( m_pRealDataSource )
		{
			return m_pRealDataSource->GetDataSourceName(name);
		}

		return false;		
	}

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState()
	{
		if( m_pRealDataSource )
		{
			return m_pRealDataSource->GetConnectState();
		}

		return 0;
	}

	virtual int WaitForPacket(const ACE_Time_Value& sleep_time)
	{
		if( m_pRealDataSource )
		{
			return m_pRealDataSource->WaitForPacket(sleep_time);
		}

		return -1;
	}

protected:
	IExtDataSource()
	{
		m_pRealDataSource = NULL;
		m_delete_real_source = 0;

	}
	virtual ~IExtDataSource()
	{
		fini();
	}

	int init(IRecvDataSource * pRealDataSource,bool bdelete)
	{
		ACE_ASSERT(pRealDataSource);

		m_pRealDataSource = pRealDataSource;

		m_delete_real_source = (int)bdelete;

		return 0;
	}

	void fini()
	{
		if( m_delete_real_source && m_pRealDataSource )
		{
			delete m_pRealDataSource;
		}

		m_pRealDataSource = NULL;
	}


	BOOL StartToRecv();
	void StopRecving();

protected:
	IRecvDataSource *	m_pRealDataSource;
	int					m_delete_real_source;



};

//////////////////////////////////////////////////////////////////////////
//IRealDataSource
//ʵ�ʹ��ܵ�����Դ

class PACKETIO_CLASS IRealDataSource : public IRecvDataSource
{
public:
	virtual DRIVER_TYPE GetDrvType() const
	{
		return m_drv_type;
	}

	void SetDrvType(DRIVER_TYPE drv_type)
	{
		m_drv_type = drv_type;
	}

protected:
	IRealDataSource()
	{

	}
	virtual ~IRealDataSource()
	{
	}


protected:
	DRIVER_TYPE	m_drv_type;
};

//////////////////////////////////////////////////////////////////////////
//IFileRecvDataSource

//���ڵ����ļ�������Դ
//������cpt��ʽ�ļ���˵����Ϊ�����������ļ��ģ�Ҳ��������һ���ļ�
//����������߶������ص��������ļ��������ɶ���ļ�
class PACKETIO_CLASS ISingleFileRecvDataSource : public IRealDataSource
{
public:
	ISingleFileRecvDataSource()
	{
		m_bcur_systime = false;
	}

	virtual ~ISingleFileRecvDataSource()
	{

	}

public:
	virtual BOOL Open(const char * filename,BOOL bcur_systime)
	{
		if( init(filename,bcur_systime) == FALSE )
			return false;

		return (OpenDataSource() == 0);
	}

	virtual void Close()
	{
		CloseDataSource();
		fini();
	}

	virtual bool GetDataSourceName(std::string& name);

	virtual BOOL init(const char * filename,BOOL bcur_systime);

	virtual void fini();

	//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ
	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
	{
		pkts = 0;
		bytes = 0;
	}

public:
	static std::string GetFileOtherInfo(const char * fullname,const Time_Stamp * pcur_time);

protected:
	void AdjustToCurSystime(PACKET_LEN_TYPE lentype,RECV_PACKET& packet)
	{
		if( m_bcur_systime &&
			(PACKET_OK == lentype || PACKET_TIMER == lentype) )
		{
			packet.pHeaderinfo->stamp.from_ace_timevalue(ACE_OS::gettimeofday());
		}
	}

protected:
	std::string m_strFileName;
	BOOL		m_bcur_systime;

};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//__RECV_DATA_SOURCE_H_2006_4_26
