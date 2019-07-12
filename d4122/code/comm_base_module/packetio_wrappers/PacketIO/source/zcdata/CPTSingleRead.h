// CPTSingleRead.h: interface for the CCPTSingleRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPTSINGLEREAD_H__6A7EADF8_46DD_4B55_AB55_8FD44D5A3B26__INCLUDED_)
#define AFX_CPTSINGLEREAD_H__6A7EADF8_46DD_4B55_AB55_8FD44D5A3B26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*******************************************************************************
  CPTSingleRead.h

	�� �� ��:	�����
	����ʱ��:	2003/05/22 
	����������  ����CPT�ļ����ࣺ
				
				CPT�ļ����Ϊ 4GB-CPTFILE_MIN_FREE
				�����ڽ��̼���CCPTSingleWrite�๲��ͬһ��CPT�ļ�.

	�޸ļ�¼��
	�޸�ʱ��	    �޸���		�޸�����

*********************************************************************************/

#include "CPTOPSTRUCT.h"

class CShareMem;
class CMapViewAccess;

class CCPTSingleRead  
{
public:
	CCPTSingleRead();
	virtual ~CCPTSingleRead();


	// ��ʼ����Ҫ��szFileName�в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
	BOOL Init(LPCSTR szFileName,DWORD dwBufSize = 1*1024*1024); //szFileNameΪ�ļ��ľ���·��

	void CloseAll();
	
	// �ж��Ƿ���һ������д���ļ�
	BOOL IsWritingFile(){return m_pShareOpBuf->m_bWriting;}
	// �ж��Ƿ��������ļ��е�һ��
	BOOL IsSerialFile(){return m_pShareOpBuf->m_filehead.bSerialFiles;}
	// ��ȡ��һ���ļ���
	LPCSTR GetNextFileName(){return m_pShareOpBuf->m_filehead.NextFileName;}
	
	// ��ȡ��������λ��,ֻ��û��д���ļ�ʱ�ſ��Ե���,����ļ���д������MAXDWORD��
	DWORD GetPktEndPos();
	
	// ��ȡ��¼�İ���
	inline DWORD GetTotalFrames(){return m_pShareOpBuf->m_dwPacketCount;}

    // ��ȡ�ļ���ǰ�Ĵ�С
    DWORD GetCurFileSize(){return m_pShareOpBuf->m_dwContentEnd;};
    
	// ͨ��֡�������ڲ���ȡָ�룬���ú󣬿�����ʹ��GetNextPacket��ȡ��������֡����GetPrevPacket
	// �ú���Ҫ��BuildIndex�Ѿ�����
	BOOL SetToIndexPacket(DWORD dwIndexFrame);
	
	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int GetNextPacket(PACKET_CONTEXT& theContext);
	
	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int GetPrevPacket(PACKET_CONTEXT& theContext);
	
	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int GetPacketFromIndex(DWORD dwIndexFrame,PACKET_CONTEXT& theContext);

	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	int GetPacketFromPos(DWORD dwPos,PACKET_CONTEXT& theContext);
	
protected:

	BOOL InitShareMem(CPT_OP_STRUCT *pOPStruct ,LPCSTR szFileName,DWORD dwFileSize);
	
	// �����ļ���Map���󣬲���ʼ��������
	BOOL CreateMapFile(BOOL bCreateFile,LPCSTR szFileName,DWORD * pdwFileSize,DWORD dwBufSize);
	
	inline void SetReadPos(DWORD dwPos){m_dwCurReadPos = dwPos;}
	inline DWORD GetReadPos(){return m_dwCurReadPos;}
	// ����,���ض�����ֽ�����-1����
	int Read(void * pData,int nSize);
	// ����,���ض�����ֽ�����-1����
	int Read(DWORD dwPos,void * pData,int nSize);
	// ���룬����ָ�����ݵ�ָ��
	void * Get(int nSize);
	// ����ǰ����ֽڣ�����ָ�����ݵ�ָ��,�ƶ���ǰ��ָ�뵽�ն���������ʼλ
	void * GetPrev(int nSize);

	BOOL IsValidCurIndex(){ return m_dwCurReadIndex>=m_pShareOpBuf->m_dwIndexCount?FALSE:TRUE;}
	
private:
	BOOL		m_bInit;						// �Ƿ��ʼ���ı�־
	BOOL		m_bCPTMem;						// �Ƿ����ڴ��ж�ȡ
	CPT_OP_STRUCT * m_pShareOpBuf;				// �������ݽṹ
	
	DWORD		m_dwCurReadPos;					// ��ָ��
	DWORD		m_dwCurReadIndex;				// �������ָ��
	
	FILE_ID			m_hFile;						// �ļ����
	ACE_HANDLE		m_hMap;							// �ļ��ڴ�ӳ����
	
	CShareMem	*m_pShareMem;					// �������ݵĹ����ڴ�
	CMapViewAccess *m_pReadBuf;					// ������

	DWORD		m_dwOpen;//����
	DWORD		m_dwFileSize;
	HEADER_INFO m_headInfo;
	
};

#endif // !defined(AFX_CPTSINGLEREAD_H__6A7EADF8_46DD_4B55_AB55_8FD44D5A3B26__INCLUDED_)
