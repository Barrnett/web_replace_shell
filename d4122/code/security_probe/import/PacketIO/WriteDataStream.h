//////////////////////////////////////////////////////////////////////////
//WriteDataStream.h

#pragma once

#include "PacketIO/RecvDataSource.h"

class PACKETIO_CLASS IBeforeSendPacketHandle
{
public:
	//�ڷ�������֮ǰ���������ݣ������޸�����
	virtual const char* BeforeSendPacket(int card_index_array,const char * pdata, unsigned int datalen, unsigned int& outdatalen) = 0;

};

class PACKETIO_CLASS IWriteDataStream
{
public:
	IWriteDataStream()
	{
		m_ullTotalSendPkts = 0;
		m_ullTotalFailedPkts = 0;
		m_pBeforeSendPacketHandle = NULL;
	}
	virtual ~IWriteDataStream()
	{

	}
public:
	//���豸���������Ӧ���ڲ������ú�֮�����
	//�ɹ�����0,���򷵻�-1
	virtual int OpenDataStream() = 0;

	//�ر��豸
	virtual void CloseDataStream()
	{
		return;
	}

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToWrite()
	{
		m_ullTotalSendPkts = 0;
		m_ullTotalFailedPkts = 0;

		return TRUE;
	}

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	//bStopNow: �Ƿ�����ֹͣ������ǣ��򻺳���ʣ������ݲ���Ҫ���ͣ��������ʣ�����ݷ�����Ϻ��ٽ���
	virtual void StopWriting(BOOL bStopNow=false)
	{
		return;
	}

	ACE_UINT64 GetTotalSendPkts() const
	{	return m_ullTotalSendPkts;	}
	ACE_UINT64 GetTotalFailedPkts() const
	{	return m_ullTotalFailedPkts;	}

	//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
	virtual int GetConnectState() = 0;

	//�������ݷ���Ŀ��ĵ�����
	virtual bool GetDataStreamName(std::string& name) = 0;

public:
	//0��ʾ����,-1��ʾ�豸����-2��ʾ�ռ��������-3��ʾ�ļ�д����ˡ�
	//try_nums:�������ʧ�ܣ����Է��Ͷ��ٴΡ�try_nums=-1��ʾ�����Σ�ֱ���ɹ�
	int WritePacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums)
	{
		++m_ullTotalSendPkts;

		if (m_pBeforeSendPacketHandle && PACKET_OK==type)
		{
			void * pNewData = packet.pPacket;
			unsigned int newDataLen = packet.nCaplen;
			pNewData = (void *)m_pBeforeSendPacketHandle->BeforeSendPacket(index_array,(const char*)pNewData, newDataLen, newDataLen);
			((RECV_PACKET*)&packet)->pPacket = pNewData;
			((RECV_PACKET*)&packet)->nCaplen = newDataLen;
			((RECV_PACKET*)&packet)->nPktlen = newDataLen;
		}

		int nrtn = WritePacketToDevWithPacket(type,packet,index_array,try_nums);

		if( 0 != nrtn )
		{
			++m_ullTotalFailedPkts;
		}

		return nrtn;
	}

	int WritePacket(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums)
	{
		++m_ullTotalSendPkts;

		if (m_pBeforeSendPacketHandle && PACKET_OK==type)
		{
			unsigned int newDataLen = datalen;

			const char* pNewData = m_pBeforeSendPacketHandle->BeforeSendPacket(index_array,pdata, datalen, newDataLen);
			
			pdata = pNewData;
			datalen = newDataLen;
		}

		int nrtn = WritePacketToDevWithData(type,pdata,datalen,index_array,try_nums);

		if( 0 != nrtn  )
		{
			++m_ullTotalFailedPkts;
		}

		return nrtn;
	}

public:
	inline void SetBeforeSendPacketHanlde(IBeforeSendPacketHandle * pBeforeSendPacketHandle)
	{
		m_pBeforeSendPacketHandle = pBeforeSendPacketHandle;
	}


protected:
	//0��ʾ����,-1��ʾ�豸����-2��ʾ�ռ��������-3��ʾ�ļ�д����ˡ�
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums) = 0;
	virtual int WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums) = 0;

protected:
	ACE_UINT64	m_ullTotalSendPkts;
	ACE_UINT64	m_ullTotalFailedPkts;

	IBeforeSendPacketHandle *	m_pBeforeSendPacketHandle;
};

//����д�ļ����������
class PACKETIO_CLASS IFileWriteDataStream : public IWriteDataStream
{
public:
	IFileWriteDataStream()
	{
		memset(&m_packet_header,0x00,sizeof(m_packet_header));
		m_packet_header.btHeaderLength = sizeof(m_packet_header);

		m_packet_header.btCardNo = 1;
		m_packet_header.btHardwareType = 0;
		m_packet_header.btInterfaceNo = 0;
		m_packet_header.btVersion = 1;
		m_packet_header.nFlag = 0;
		m_packet_header.dwError = 0;

	}
	virtual ~IFileWriteDataStream()
	{

	}

	//�Ƿ�֧��д���ƣ�
	virtual BOOL IsSupportWriteCtrl() const = 0;

	//cloneһ������Ҫ�󷵻صĶ����Ǹö���init֮��OpenDataStream֮ǰ��״̬
	//�����õ�clone���صĶ���󣬿���OpenDataStream,CloseDataStream,WritePacket.
	virtual IFileWriteDataStream * CloneAfterIni() = 0;

	const char * GetFileName() const
	{
		return m_strFileName.c_str();
	}

	void SetFileName(const char * filename)
	{
		ACE_ASSERT(filename&&filename[0] != 0);

		if( !filename || filename[0] == 0 )
		{
			m_strFileName.clear();
		}
		else
		{
			m_strFileName = filename;
		}
	}

public:
	virtual int WritePacketToDevWithData(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array,int try_nums)
	{
		RECV_PACKET packet;
		memset(&packet,0x00,sizeof(packet));

		packet.pHeaderinfo = &m_packet_header;

		m_packet_header.stamp.from_ace_timevalue(ACE_OS::gettimeofday());

		packet.pPacket = (void *)pdata;
		packet.nPktlen = packet.nCaplen = (int)datalen;

		return WritePacketToDevWithPacket(type,packet,index_array,try_nums);
	}

protected:
	PACKET_HEADER	m_packet_header;
	std::string		m_strFileName;

};

//���ڰ忨�ķ�������
class PACKETIO_CLASS IDevWriteDataStream : public IWriteDataStream
{
public:
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums)
	{
		if( type == PACKET_OK )
		{
			return this->WritePacketToDevWithData(type,(const char *)packet.pPacket,(unsigned int)packet.nPktlen,index_array,try_nums);
		}

		return -1;
	}
};
