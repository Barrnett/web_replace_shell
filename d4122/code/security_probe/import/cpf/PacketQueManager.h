// PacketQueManager.h: interface for the CPacketQueManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKETQUEMANAGER_H__3286F4CE_2021_41E5_B4D4_F0DB5C155817__INCLUDED_)
#define AFX_PACKETQUEMANAGER_H__3286F4CE_2021_41E5_B4D4_F0DB5C155817__INCLUDED_


#pragma once


//�����ʵ�ְ����й�����
//��ʵ�ʵ�Ӧ���У�һ���̻߳��߽�����һ�����е�ĩβд����
//����һ���̻߳��߽��̴Ӷ���ͷ�����ݡ�
//��ο��ٵ�ͬ����һ������(Reader)��д��(Writer)��������ʵ��Ŀ��

//ǧ��ע�⣺�����ֻ������һ�����ߺ�һ��д�ߵ������
//�����κζ���>2����д��>2���Ǵ���ġ�

//�ڴ�Ľṹ��SHARE_BUFFER_HEADER�ǹ���ͷ���ǹ�������������Ĺ�������
//Data.......Data�Ǵ���û�ʵ�ʵ����ݲ��֡��û�һ��Ķ�д���ݾ�������һ��
//PACKET_HEADER��Ϊ�˵�ǰ�ռ��β�ڴ治������һ������ʱ�򣬺����ڴ��㹻����һ��PACKET_HEADER�ṹ��������Ϊ��Ч��
//DataHeader�����û�������ͷ����
//--------------------------------------------------------------
//|SHARE_BUFFER_HEADER|Data.......Data|PACKET_HEADER|DataHeader|
//--------------------------------------------------------------

#include "cpf/cpf.h"

class CPQMWriter;
class CPQMReader;


class CPF_CLASS CPacketQueManager 
{
private:
	//��������ͷ���ṹ
	typedef struct
	{
		UINT_PTR dwSizeBuffer;//ֻ�����������֡���ֵĴ�С��������SHARE_BUFFER_HEADER����
		UINT_PTR dwReadPos;
		UINT_PTR dwWritePos;
		UINT_PTR dwDataHeaderSize;//�û�����ͷ�Ĵ�С,���Ϊ0����ʾû������ͷ��dwDataHeaderOffset��Ч

		ACE_UINT64	u64WriteNums;
		ACE_UINT64	u64ReadNums;
	
	}SHARE_BUFFER_HEADER;

	//Ϊÿһ��������һ����ͷ
	typedef struct{

		UINT_PTR dwOrgSize;//ÿһ������ԭʼ��С

	}PACKET_HEADER;
	
public:
	CPacketQueManager();
	~CPacketQueManager();

public:	
	//����û��Ѿ������˻�������ͨ��pBuffer�����������pBuffer��0�����ڲ�������ɾ��
	//������ɹ�������NULL.����ɹ������ػ������ĵ�ַ
	void * CreatePQM(UINT_PTR dwSizeBuffer, UINT_PTR uDataHeadersize=0,void * pBuffer=NULL);

	void * CreatePQM(void * pBuffer);

	//�ͷ���Դ��׼���˳�
	//���뱣֤��ֹͣ�κζ���д�������µ���
	void DestroyPQM();

	//��CreatePQM֮�����
	//������ɹ�������NULL.����ɹ���CPQMWriter����ĵ�ַ
	CPQMWriter * CreateWritter();

	//��CreatePQM֮�����
	//������ɹ�������NULL.����ɹ���CPQMWriter����ĵ�ַ
	CPQMReader * CreateReader();

	
public:
	//�����������
	//���뱣֤��ֹͣ�κζ���д�������µ��á�
	//���ú�������֮�󣬲����ٴζ�д����
	void  ClearQueue();
		
	//�õ��û�ͷ����,ֻ�����û������ɹ��󣬲��ܷ���
	void * GetHeaderData(UINT_PTR* uHeaderLen=NULL) const;

	UINT_PTR GetPacketsInQue() const
	{	return UINT_PTR(m_pShareBufferHeader->u64WriteNums-m_pShareBufferHeader->u64ReadNums);	}

	ACE_UINT64 GetWriteNums() const
	{	return m_pShareBufferHeader->u64WriteNums;	}

	ACE_UINT64 GetReadNums() const
	{	return m_pShareBufferHeader->u64ReadNums;	}

	ACE_UINT32 GetBufferUsed(OUT ACE_UINT32* pTotalLen) const;

private:
	CPacketQueManager(const CPacketQueManager& pqm);
	const CPacketQueManager& operator = (const CPacketQueManager& pqm);

private:
	void * FormatShareBuffer(UINT_PTR dwSizeBuffer, UINT_PTR uDataHeadersize=0,void * pBuffer=NULL);

private:	
	CPacketQueManager::SHARE_BUFFER_HEADER *	m_pShareBufferHeader;

	BYTE *		m_pInfoBuffer;//�洢�û���д���ݵ������ָ��
	UINT_PTR	m_dwEndPos;


	BOOL		m_bCreateBuffer;//�Ƿ����ڲ�����������

private:
	CPQMReader	*			m_pqmReader;
	CPQMWriter	*			m_pqmWriter;


	friend class CPQMReader;
	friend class CPQMWriter;


};

class CPF_CLASS CPQMWriter
{

public:
	BOOL  WritePacket(const void * packet,UINT_PTR dwPacketSize);

	//ReserveBufferWritePacket��WriteReservedBuffer����ɶ�ʹ��
	//���ReserveBufferWritePacketû�з��ؿգ���ô�û����������ݺ󣬱������WriteReservedBuffer
	//����û�����WriteReservedBufferUseRealSize,���뱣֤dwRealPacketSize<dwPacketSize.

	//������������֮�䲻�������ReserveBufferWritePacket����WritePacket��
	BYTE *  ReserveBufferWritePacket(UINT_PTR dwPacketSize);

	BOOL	WriteReservedBuffer()
	{	return WriteReservedBufferUseRealSize(m_dwLastReservePackLen);}

	BOOL	WriteReservedBufferUseRealSize(UINT_PTR dwRealPacketSize);

	CPacketQueManager * GetPQM()
	{	return m_pqm;	}


public:
	//�����ڴ�ֱ���ɹ������˳�
	//֮����ʹ��ָ�룬����Ϊ����߳̿��Ը����˳���־�����������Բ��񵽱仯��
	inline BOOL  WritePacket(const void * packet,UINT_PTR dwPacketSize,const INT_PTR * quitflag)
	{
		BOOL bOK = FALSE;

		while( bOK && !(*quitflag) )
		{
			bOK = WritePacket(packet,dwPacketSize);

			if( bOK )
				break;
			else
				ACE_OS::sleep(ACE_Time_Value(0,5000));
		}

		return bOK;
	}
		

	//�����ڴ�ֱ���ɹ������˳�
	//֮����ʹ��ָ�룬����Ϊ����߳̿��Ը����˳���־�����������Բ��񵽱仯��
	inline BYTE *  ReserveBufferWritePacket(UINT_PTR dwPacketSize,const INT_PTR * quitflag)
	{
		BYTE * pdata = NULL;

		while( !pdata && !(*quitflag) )
		{
			pdata = ReserveBufferWritePacket(dwPacketSize);

			if( pdata )
				break;
			else
				ACE_OS::sleep(ACE_Time_Value(0,5000));
		}

		return pdata;
	}

private:
	CPQMWriter();
	~CPQMWriter();

	void Create(CPacketQueManager * pqm);
	void Destroy();

	void  ClearQueue()
	{
		m_dwNowWritePos = 0;
	}

private:
	UINT_PTR		m_dwLastReservePackLen;//��һ���û�ԤԼ�����ݳ���


	UINT_PTR		m_dwNowWritePos;


	CPacketQueManager * m_pqm;

	friend class CPacketQueManager;
};

class CPF_CLASS CPQMReader
{
public:
	//���������ذ��Ĵ�С���ڲ���ppPakcet���ذ���ָ��
	//����ֵ��0--û��ȡ��������Ϣ
	//        ���ݰ�����--ȡ����ǰ����Ϣ�����lenΪ0���ð�����Ч��
	//ֻ����m_dwUserType&READER != 0ʱ���ܵ���
	UINT_PTR ReadNextPacket(void*& ppPakcet);

	CPacketQueManager * GetPQM()
	{	return m_pqm;	}

private:
	CPQMReader();
	~CPQMReader();

	void Create(CPacketQueManager * pqm);
	void Destroy();

	void SkipToNextPacket();

	void  ClearQueue()
	{
		m_dwLastReadedPacketLen = 0;
	}

private:
	UINT_PTR	m_dwLastReadedPacketLen;//��һ�ζ������ݵĳ���

	CPacketQueManager * m_pqm;

	friend class CPacketQueManager;

};

#endif // !defined(AFX_PACKETQUEMANAGER_H__3286F4CE_2021_41E5_B4D4_F0DB5C155817__INCLUDED_)


