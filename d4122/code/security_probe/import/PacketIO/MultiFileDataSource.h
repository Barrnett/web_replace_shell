//////////////////////////////////////////////////////////////////////////
//MultiFileDataSource.h

#pragma once


#include "PacketIO/PacketIO.h"
#include "PacketIO/RecvDataSource.h"
#include <utility>
#include "ace/Recursive_Thread_Mutex.h"

class PACKETIO_CLASS CMultiFileDataSource : public IRecvDataSource
{
public:
	CMultiFileDataSource(void);
	virtual ~CMultiFileDataSource(void);

	virtual DRIVER_TYPE GetDrvType() const
	{
		return DRIVER_TYPE_MULTI_FILE;
	}

public:
	//���ܶ�ε���,�൱��init+AddMultiFileName+OpenDataSource
	//re_calculate_time=1��ʾ����ļ���ʱ���������������֤�ڶ����ļ�����ʱ��϶���ǰһ���ļ���
	//re_calculate_time=0��ʾʹ��ÿ���ļ��������ʱ��,�����Ϳ��ܳ��ֺ������ʱ��С��ǰ�����ʱ�䡣
	//bcur_systime:ÿ������ʱ����ʹ���ļ��а���ʱ�仹��ʹ�õ�ǰϵͳʱ��,���bcur_systime=true,��re_calculate_time��Ч
	BOOL Open_EthrealFiles(const CStdStringArray& vtFilename,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false);

	BOOL Open_CptFiles(const CStdStringArray& vtFilename,
		unsigned int nReadNums,unsigned int bufsizeMB,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false);


	//���ܶ�ε���,�൱��init+AddFileName+OpenDataSource
	BOOL Open_EthrealFile(const char * filename,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false);

	BOOL Open_CptFile(const char * filename,
		unsigned int nReadNums,unsigned int bufsizeMB,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false);

	void Close()
	{
		CloseDataSource();
		fini();
	}

	virtual bool GetDataSourceName(std::string& name);

	void init_Cpt(unsigned int nReadNums,unsigned int bufsizeMB,
		bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false)
	{
		m_nReadNums = nReadNums;
		m_nBufferSizeMB = bufsizeMB;

		m_re_calculate_time = (int)re_calculate_time;
		m_bcur_systime = (int)bcur_systime;
		m_bskip_error_file = (int)skip_error_file;
	}

	void init_Ethreal(bool re_calculate_time=true,bool bcur_systime=false,bool skip_error_file=false)
	{
		m_re_calculate_time = (int)re_calculate_time;
		m_bcur_systime = (int)bcur_systime;
		m_bskip_error_file = (int)skip_error_file;
	}

	//һ�ε��ô������ļ������Զ�ε��á��ļ����ۼ�
	void AddMultiFileName(DRIVER_TYPE type,const CStdStringArray& vtFilename);

	//���Զ�ε���AddFileName���ļ����ۼ�
	void AddFileName(DRIVER_TYPE type,const char * filename);

	void fini();

	virtual void GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops);

	//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
	virtual BOOL StartToRecv();

	//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
	virtual void StopRecving();

	virtual int OpenDataSource();
	virtual void CloseDataSource();

	//��������
	virtual PACKET_LEN_TYPE GetPacket(RECV_PACKET& packet);

	virtual void GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
	{
		pkts = bytes = 0;
		return;
	}


	//��������λ��ָ��ʼ�ĵط���
	virtual BOOL SeekToBegin();

	virtual int GetConnectState();
	
private:
	//ת����һ���ļ������Ҷ�һ�����ݰ�
	//�����Ƿ�������������FALSE����ʾ�����ļ��������ˣ����ص�������Ч��
	//�������TRUE��ʾ����ʾ������Ч
	PACKET_LEN_TYPE TurnToNextFileAndRead(RECV_PACKET& packet);

	ISingleFileRecvDataSource * CreateSingleFileRecvDataSource(DRIVER_TYPE type);
	void CalculateTotalPkts(ISingleFileRecvDataSource * pFileDataSource);

private:
	ISingleFileRecvDataSource * OpenByIndex(int index);

private:
	typedef std::pair<DRIVER_TYPE,std::string>	FILE_ITEM;

	std::vector<FILE_ITEM>	m_vtFileItem;

	ISingleFileRecvDataSource *	m_pCurDataSource;
	int							m_nCurIndex;//�Ѿ��ŵ��ڼ����ļ�

	ACE_UINT64					m_totalpkts;//����Դ���ܰ���
	ACE_UINT64					m_totalbytes;//����Դ�����ֽ���

	//��ǰ����ʱ���ǣ���׼ʱ��+��ǰ�����ļ�ʱ��-��ǰ�ļ���һ������ʱ�䣩
	CTimeStamp32			m_nCurFileFirstTime;//��ǰ�ļ��ĵ�һ������ʱ��
	CTimeStamp32			m_nCurFileLastTime;//��ǰ�ļ����һ������ʱ��
	CTimeStamp32			m_nBaseTime;//��׼ʱ��

	int						m_re_calculate_time;//�Ƿ�Ҫ���¼���ʱ��
	int						m_bcur_systime;
	int						m_bskip_error_file;

	//����cpt�ļ���ʽ
	unsigned int	m_nReadNums;
	unsigned int	m_nBufferSizeMB;

	ACE_Recursive_Thread_Mutex	m_lock;



};


//////////////////////////////////////////////////////////////////////////
//CVPKMultiFileDataSource
//////////////////////////////////////////////////////////////////////////
#include "VPKHelpDataSource.h"

class PACKETIO_CLASS CVPKMultiFileDataSource : public CVPKHelpDataSource
{
public:
	CVPKMultiFileDataSource();
	virtual ~CVPKMultiFileDataSource();

	//���ܶ�ε���,�൱��init+AddMultiFileName+OpenDataSource
	BOOL Open(DRIVER_TYPE type,const CStdStringArray& vtFilename,int re_calculate_time=1);

	//���ܶ�ε���,�൱��init+AddFileName+OpenDataSource
	BOOL Open(DRIVER_TYPE type,const char * filename,int re_calculate_time=1);

	//�൱��CloseDataSource+fini
	void Close();

	void init(int re_calculate_time=1);

	void fini();

	//һ�ε��ô������ļ������Զ�ε��á��ļ����ۼ�
	void AddMultiFileName(DRIVER_TYPE type,const CStdStringArray& vtFilename);

	//���Զ�ε���AddFileName���ļ����ۼ�
	void AddFileName(DRIVER_TYPE type,const char * filename);

private:
	CMultiFileDataSource *	m_pMultifileDataSource;
};
