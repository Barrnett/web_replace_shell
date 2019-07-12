// CPTSingleWrite.h: interface for the CCPTSingleWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPTSINGLEWRITE_H__4F620C2F_AEE6_4762_A12D_60D550F0A748__INCLUDED_)
#define AFX_CPTSINGLEWRITE_H__4F620C2F_AEE6_4762_A12D_60D550F0A748__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*******************************************************************************
  CPTSingleWrite.h

	�� �� ��:	�����
	����ʱ��:	2003/05/22 
	����������  ����CPT�ļ���д�ࣺ
				
				CPT�ļ����Ϊ 4GB-CPTFILE_MIN_FREE
				֧�ֶ���д�ӿڣ������ڽ��̼���CCPTSingleRead�๲��ͬһ��CPT�ļ�.

	�޸ļ�¼��
	�޸�ʱ��	    �޸���		�޸�����

*********************************************************************************/


#include "CPTOPSTRUCT.h"
#include "CPTSingleRead.h"


class CShareMem;
class CMapViewAccess;

class CCPTSingleWrite  
{
public:
	CCPTSingleWrite();
	virtual ~CCPTSingleWrite();

	// ��ʼ����Ҫ��szFileName�в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
	BOOL Init(LPCSTR szFileName,									// �ļ���
		DWORD dwMaxSize								= 50*1024*1024, // �ļ���󳤶�
		DWORD dwBufSize								= 1*1024*1024,	// ������Ĵ�С
		BOOL bSerialFile							= FALSE,		// �Ƿ�������¼�е�һ��
		int nFileNo									= 0,			// ���������¼���ļ�˳��ţ���0��ʼ
		LPCSTR szPrevFileName						= NULL,			// ���������¼��������һ���ļ���,����·�����������ļ�ͷ��
		GET_NEXT_FILE_NAME_FUNC pfnGetNextFile		= NULL,			// ���������¼����ȡ��һ���ļ���������·�����FILENAME_LEN
		void * pParam								= NULL			// pfnGetNextFile�ص��� void * ����
		); 

	void CloseAll();
	
	// ��ȡ�ļ���
	LPCSTR GetFileName();
	// ��ȡ��󳤶�
	DWORD GetMaxSize(){return m_pShareOpBuf->m_dwMapSize;}

	// ��ȡ��ǰд���С
	DWORD GetWritedSize(){return m_pShareOpBuf->m_dwContentEnd;};
	
	// ��ȡ��������λ��,ֻ��RecordPacket�����ļ�����ʱ���Ե���
	DWORD GetPktEndPos(){return m_pShareOpBuf->m_filehead.dwIndexPosInFile;};

	// ��ȡ��ǰ��¼�İ���
	inline DWORD GetTotalFrames(){return m_pShareOpBuf->m_dwPacketCount;}

	// ��¼������,���� 1 ������0 �ļ�������1����,dwIndexFrame����
	int RecordPacket(BYTE * pPacket,int nPacketLen,HEADER_INFO * pHeadInfo,
		DWORD * pdwIndexFrame = NULL, DWORD * pdwPos = NULL);
	
	// ͨ��֡�������ڲ���ȡָ�룬���ú󣬿�����ʹ��GetNextPacket��ȡ��������֡����GetPrevPacket
	inline BOOL SetToIndexPacket(DWORD dwIndexFrame){return m_ReadObj.SetToIndexPacket(dwIndexFrame);}

	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	inline int GetNextPacket(PACKET_CONTEXT& theContext){return m_ReadObj.GetNextPacket(theContext);}

	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	inline int GetPrevPacket(PACKET_CONTEXT& theContext){return m_ReadObj.GetPrevPacket(theContext);}

	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	inline int GetPacketFromIndex(DWORD dwIndexFrame,PACKET_CONTEXT& theContext){return m_ReadObj.GetPacketFromIndex(dwIndexFrame,theContext);}

	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	inline int GetPacketFromPos(DWORD dwPos,PACKET_CONTEXT& theContext){return m_ReadObj.GetPacketFromPos(dwPos,theContext);}
	
	// �������ڴ������д�뵽�ļ���
	BOOL SaveMemToDiskFile(LPCSTR szFileName);
	
protected:

	// ��д�ļ�ͷ,��������ϢTRUE��ʾ�ɹ�
	BOOL EndFile();
	
	// �����ļ���Map���󣬲���ʼ��д����
	BOOL CreateMapFile(BOOL bInMem,DWORD dwBufSize);
	
	// ���ļ�β��д������,nPosΪ������־���λ��
	BOOL WriteIndexAtEnd(DWORD dwPos);

	// д��,����д����ֽ�����-1����
	int Write(const void * pData,int nSize);
	// д��,����д����ֽ�����-1����
	int Write(DWORD dwPos,const void * pData,int nSize);
	
private:
	BOOL		m_bInit;						// �Ƿ��ʼ���ı�־
	BOOL		m_bCPTMem;						// �Ƿ������ڴ���
	BOOL		m_bFileEnd;						// �ļ�д���Ƿ����
	CPT_OP_STRUCT * m_pShareOpBuf;				// �������ݽṹ

	DWORD		m_dwCurWritePos;				// дָ��
	
	FILE_ID		m_hFile;						// �ļ����
	HANDLE		m_hMap;							// �ļ��ڴ�ӳ����

	CShareMem	* m_pShareMem;						// �������ݵĹ����ڴ�
	CMapViewAccess * m_pWriteBuf;					// д����
	CCPTSingleRead m_ReadObj;					// ������

	GET_NEXT_FILE_NAME_FUNC m_pfnGetNextFile;	// ��ȡ��һ���ļ���������ʱ����
	void *		m_pParam;						// m_pfnGetNextFile�Ĳ���

	DWORD m_dwOpen ;
};

#endif // !defined(AFX_CPTSINGLEWRITE_H__4F620C2F_AEE6_4762_A12D_60D550F0A748__INCLUDED_)
