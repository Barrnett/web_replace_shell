/*******************************************************************************
  CapDataRead.h 

	�� �� ��:	
	����ʱ��:	2002/8/16 
	����������  ���������ļ���ȡ��

				����֧��������¼�Ķ���ļ�������ȡ��

				�ṩ������ӿ������Ƶĺ����ӿ�
				�����������ԡ��ӿڹ�������ȡ���ݰ��Ľӿں������ĵ�

				��ȡ���ݵĽӿں�������
				//ֻ��ȡ��ͷ��Ϣ�����ݲ���ֱ��ָ��ԭ�����ݰ����ֵĻ�����
				int GetPacketHeaderInfo(PACKET_CONTEXT* pContext);

				��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��


	�޸ļ�¼��  

	�޸�ʱ��	2003.6.20
    �޸���		
    �޸�����    
                1.����GetCurSize()����
                2.�޸�GetTotalFrames������ΪGetCurTotalFrames

*********************************************************************************/


#if !defined(AFX_CAPDATAREAD3_H__D64BD075_5A76_4969_ACA3_51DA5844282C__INCLUDED_)
#define AFX_CAPDATAREAD3_H__D64BD075_5A76_4969_ACA3_51DA5844282C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cpf/Octets.h"
#include "CapFileHead.h"
#include "CPTSingleRead.h"
#include "CAPOPSTRUCT.h"


class CCapDataRead  
{
public:
	CCapDataRead();
	virtual ~CCapDataRead();

	LPCSTR GetOrgFile(){return m_strOrgFileName.GetString();}
	LPCSTR GetCurFile();

    // ��ʼ����Ҫ��szFileName�в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
    BOOL Init(LPCSTR szFileName,DWORD nReadNums,DWORD dwBufSize = 1*1024*1024);

	void CloseAll();
	
	// ��ȡ��ǰ��¼�İ���
	ULONGLONG GetCurTotalFrames();
    // ��ȡ��ǰ�Ĵ�С���ֽڣ�
    ULONGLONG GetCurTotalSize();
	
	// ͨ��֡�����õ�ǰ�����ڲ���ȡָ�룬���ú󣬿�����ʹ��GetNextPacket��ȡ��������֡����GetPrevPacket
	BOOL SetToIndexPacket(const ULONGLONG & ullIndexFrame);
	
	// ��ȡ��ǰ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ��������ƶ��ڲ���ȡָ�뵽��һ����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int GetNextPacket(PACKET_CONTEXT& theContext)
	{
		int nRes = InnerGetNextPacket(theContext);

		if( nRes == +1 )
		{
			m_buffer.SetData(theContext.pPacket,theContext.dwLengthPacket);
			theContext.pPacket = m_buffer.GetData();
		}

		return nRes;
		
	}
	
	// ��ȡ��ǰ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ��������ƶ��ڲ���ȡָ�뵽��һ����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int GetPrevPacket(PACKET_CONTEXT& theContext)
	{
		int nRes = InnerGetPrevPacket(theContext);
		
		if( nRes == +1 )
		{
			m_buffer.SetData(theContext.pPacket,theContext.dwLengthPacket);
			theContext.pPacket = m_buffer.GetData();
		}
		
		return nRes;
	}
	
	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����,�ƶ��ڲ���ȡָ�뵽��һ����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int GetPacketFromIndex(const ULONGLONG & ullIndexFrame,PACKET_CONTEXT& theContext)
	{
		int nRes = InnerGetPacketFromIndex(ullIndexFrame,theContext);
		
		if( nRes == +1 )
		{
			m_buffer.SetData(theContext.pPacket,theContext.dwLengthPacket);
			theContext.pPacket = m_buffer.GetData();
		}
		
		return nRes;
	}

	// ��ȡָ��λ�ð���Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ����������ú󣬵�ǰ��λ�÷�����ȷ���Ա仯��
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	// ע�⣬�ú������ú��ʹGetNextPacket��GetPrevPacket���ò�ȷ����
	// Ӧ���ٴ�ʹ��SetToIndexPacket����GetPacketFromIndex���ٵ���GetNextPacket��GetPrevPacket
	int GetPacketFromPos(const ULONGLONG & ullPos,PACKET_CONTEXT& theContext);
	
public:
	LPCSTR MakeFullPathName(LPCSTR szFileName,CStr& strFullName);

protected:
	// ��ȡ��ǰ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ��������ƶ��ڲ���ȡָ�뵽��һ����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int InnerGetNextPacket(PACKET_CONTEXT& theContext);
	
	// ��ȡ��ǰ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ��������ƶ��ڲ���ȡָ�뵽��һ����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int InnerGetPrevPacket(PACKET_CONTEXT& theContext);
	
	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����,�ƶ��ڲ���ȡָ�뵽��һ����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int InnerGetPacketFromIndex(const ULONGLONG & ullIndexFrame,PACKET_CONTEXT& theContext);
	
	BOOL InitShareMem(CAP_OP_STRUCT *pCapShareOpBuf);
	BOOL DetectPrevVer(LPCSTR szFileName);
protected:
	CCPFOctets	m_buffer;

	DWORD	m_nReadNums;//�������ļ���0��ʾ��������
	BOOL m_bInit;

	CStr m_strOrgFileName;			// ԭʼ���ļ�����
	int		m_nCurFile;					// ��ǰ���ŵ��ļ�

	DWORD m_dwBufSize;					// ������Ĵ�С
	CCPTSingleRead m_SingleReadObj;		// ��һ���ļ�

	CAP_OP_STRUCT * m_pCapShareOpBuf;	// �����ڴ�ָ��

	CShareMem	*m_pShareMem;				// �����ڴ����
};

#endif // !defined(AFX_CAPDATAREAD3_H__D64BD075_5A76_4969_ACA3_51DA5844282C__INCLUDED_)
