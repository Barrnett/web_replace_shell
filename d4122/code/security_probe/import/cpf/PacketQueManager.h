// PacketQueManager.h: interface for the CPacketQueManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKETQUEMANAGER_H__3286F4CE_2021_41E5_B4D4_F0DB5C155817__INCLUDED_)
#define AFX_PACKETQUEMANAGER_H__3286F4CE_2021_41E5_B4D4_F0DB5C155817__INCLUDED_


#pragma once


//这个是实现包队列管理器
//在实际的应用中，一个线程或者进程向一个队列的末尾写数据
//另外一个线程或者进程从队列头都数据。
//如何快速的同步这一个读者(Reader)和写者(Writer)是这个类的实现目的

//千万注意：这个类只能用于一个读者和一个写者的情况。
//对于任何读者>2或者写者>2都是错误的。

//内存的结构：SHARE_BUFFER_HEADER是共享头，是管理这个数据区的管理数据
//Data.......Data是存放用户实际的数据部分。用户一般的读写数据就是在这一段
//PACKET_HEADER是为了当前空间结尾内存不够放下一个包的时候，后面内存足够放下一个PACKET_HEADER结构的数据置为无效。
//DataHeader，是用户的数据头部分
//--------------------------------------------------------------
//|SHARE_BUFFER_HEADER|Data.......Data|PACKET_HEADER|DataHeader|
//--------------------------------------------------------------

#include "cpf/cpf.h"

class CPQMWriter;
class CPQMReader;


class CPF_CLASS CPacketQueManager 
{
private:
	//共享缓冲区头部结构
	typedef struct
	{
		UINT_PTR dwSizeBuffer;//只包括存放数据帧部分的大小，不包括SHARE_BUFFER_HEADER本身
		UINT_PTR dwReadPos;
		UINT_PTR dwWritePos;
		UINT_PTR dwDataHeaderSize;//用户数据头的大小,如果为0，表示没有数据头，dwDataHeaderOffset无效

		ACE_UINT64	u64WriteNums;
		ACE_UINT64	u64ReadNums;
	
	}SHARE_BUFFER_HEADER;

	//为每一个包定义一个包头
	typedef struct{

		UINT_PTR dwOrgSize;//每一个包的原始大小

	}PACKET_HEADER;
	
public:
	CPacketQueManager();
	~CPacketQueManager();

public:	
	//如果用户已经开辟了缓冲区，通过pBuffer传进来，如果pBuffer＝0，由内部创建和删除
	//如果不成功，返回NULL.如果成功，返回缓冲区的地址
	void * CreatePQM(UINT_PTR dwSizeBuffer, UINT_PTR uDataHeadersize=0,void * pBuffer=NULL);

	void * CreatePQM(void * pBuffer);

	//释放资源，准备退出
	//必须保证在停止任何读，写的条件下调用
	void DestroyPQM();

	//在CreatePQM之后调用
	//如果不成功，返回NULL.如果成功，CPQMWriter对象的地址
	CPQMWriter * CreateWritter();

	//在CreatePQM之后调用
	//如果不成功，返回NULL.如果成功，CPQMWriter对象的地址
	CPQMReader * CreateReader();

	
public:
	//清除所有数据
	//必须保证在停止任何读，写的条件下调用。
	//调用函数返回之后，才能再次读写数据
	void  ClearQueue();
		
	//得到用户头数据,只有在用户创建成功后，才能访问
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

	BYTE *		m_pInfoBuffer;//存储用户读写数据的区域的指针
	UINT_PTR	m_dwEndPos;


	BOOL		m_bCreateBuffer;//是否由内部来创建数据

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

	//ReserveBufferWritePacket和WriteReservedBuffer必须成对使用
	//如果ReserveBufferWritePacket没有返回空，那么用户在添完数据后，必须调用WriteReservedBuffer
	//如果用户调用WriteReservedBufferUseRealSize,必须保证dwRealPacketSize<dwPacketSize.

	//而且两个函数之间不允许调用ReserveBufferWritePacket或者WritePacket。
	BYTE *  ReserveBufferWritePacket(UINT_PTR dwPacketSize);

	BOOL	WriteReservedBuffer()
	{	return WriteReservedBufferUseRealSize(m_dwLastReservePackLen);}

	BOOL	WriteReservedBufferUseRealSize(UINT_PTR dwRealPacketSize);

	CPacketQueManager * GetPQM()
	{	return m_pqm;	}


public:
	//申请内存直到成功或者退出
	//之所以使用指针，是因为别的线程可以更改退出标志，而函数可以捕获到变化。
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
		

	//申请内存直到成功或者退出
	//之所以使用指针，是因为别的线程可以更改退出标志，而函数可以捕获到变化。
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
	UINT_PTR		m_dwLastReservePackLen;//上一次用户预约的数据长度


	UINT_PTR		m_dwNowWritePos;


	CPacketQueManager * m_pqm;

	friend class CPacketQueManager;
};

class CPF_CLASS CPQMReader
{
public:
	//读包，返回包的大小。在参数ppPakcet返回包的指针
	//返回值：0--没有取到数据信息
	//        数据包长度--取到当前包信息，如果len为0，该包是无效的
	//只有在m_dwUserType&READER != 0时才能调用
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
	UINT_PTR	m_dwLastReadedPacketLen;//上一次读到数据的长度

	CPacketQueManager * m_pqm;

	friend class CPacketQueManager;

};

#endif // !defined(AFX_PACKETQUEMANAGER_H__3286F4CE_2021_41E5_B4D4_F0DB5C155817__INCLUDED_)


