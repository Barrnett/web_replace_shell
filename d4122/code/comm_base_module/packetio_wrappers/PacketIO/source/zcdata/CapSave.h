/*******************************************************************************
  CapSave.h

	�� �� ��:	�����
	����ʱ��:	2003/5/22 
	����������  �������CPT�ļ���¼��(ͨ���ļ�ͷ�е�ǰ�����ļ�������)��
				֧���ڶ�����̼�Ͷ����CCapDataRead���ͬһϵ��CPT�ļ��Ķ���

	�޸ļ�¼��
	�޸�ʱ��	    �޸���		�޸�����

*********************************************************************************/

#if !defined(AFX_CAPSAVE_H__6CC18264_0FC4_4C4F_97ED_5D5609846165__INCLUDED_)
#define AFX_CAPSAVE_H__6CC18264_0FC4_4C4F_97ED_5D5609846165__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CapFileHead.h"
#include "CPTSingleWrite.h"
#include "CapDataRead.h"
#include "CAPOPSTRUCT.h"
#include "ShareMem.h"

class CCapSave  
{
public:
	CCapSave();
	virtual ~CCapSave();
	
	// ��ʼ����Ҫ��szFileName�в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
	BOOL Init(LPCSTR szFileName,
		DWORD dwMaxFileLengthMb					= 50,
		DWORD dwBufSize							= 1*1024*1024,	// �����С
		int nMultiFiles							= 1,
		unsigned int ncaptime					= 0,//����೤ʱ�䣬����ʾ������
		GET_NEXT_FILE_NAME_FUNC pfnGetNextFile	= NULL,			// ���������¼����ȡ��һ���ļ���������·�����FILENAME_LEN
		void * pParam							= NULL			// pfnGetNextFile�ص��� void * ����
		);

	void CloseAll();

	// ��ȡ��ǰ��¼�İ���
	inline ULONGLONG GetTotalFrames(){return m_pCapShareOpBuf->m_ullTotalFrame;}
	// ��ȡ��ǰʹ�õĿռ���
	ULONGLONG GetTotalUsage();
	
	// ��ȡ��ʼ���ļ���
	LPCSTR GetInitFileName();
	// ��ȡ���ڼ�¼�ĵ�ǰ�ļ���
	LPCSTR GetCurrentFileName();

	// ��ȡ��¼��ʽ
	inline BOOL GetMultiFiles() const {return m_nMultiFiles;}


	/*
	*	��¼������,0��ʾ����,-1��ʾ�豸����-2��ʾ�ռ��������-3��ʾ�ļ�д����ˡ�
	�ɹ�ʱ��ullPos��������GetPacket,����������SetToIndexPacket	
	*/
	int RecordPacket(BYTE * pPacket,int nPacketLen,HEADER_INFO * pHeadInfo,
		ULONGLONG * pullIndexFrame = NULL, ULONGLONG * pullPos = NULL);
	
	// ͨ��֡�������ڲ���ȡָ�룬���ú󣬿�����ʹ��GetNextPacket��ȡ��������֡����GetPrevPacket
	// �ú���Ҫ��BuildIndex�Ѿ�����
	inline BOOL SetToIndexPacket(const ULONGLONG & ullIndexFrame){return m_ReadObj.SetToIndexPacket(ullIndexFrame);}
	
	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	inline int GetNextPacket(PACKET_CONTEXT& theContext){return m_ReadObj.GetNextPacket(theContext);}
	
	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	inline int GetPrevPacket(PACKET_CONTEXT& theContext){return m_ReadObj.GetPrevPacket(theContext);}
	
	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	inline int GetPacketFromIndex(const ULONGLONG & ullIndexFrame,PACKET_CONTEXT& theContext){return m_ReadObj.GetPacketFromIndex(ullIndexFrame,theContext);}

	// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
	// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
	inline int GetPacketFromPos(const ULONGLONG & ullPos,PACKET_CONTEXT& theContext){return m_ReadObj.GetPacketFromPos(ullPos,theContext);}
	
protected:
	static 	LPCSTR GetNextFileName(void * pParam);
	LPCSTR MakeFullPathName(LPCSTR szFileName,CStr& strFullName);

	//0��ʾ����,-1��ʾ�豸����-2��ʾ�ռ��������-3��ʾ�ļ�д����ˡ�
	int CreateNextFile();
protected:
	BOOL		m_bInit;
	CStr		m_strInitFileName;		// ��ʼ�����ļ���,ȫ·��
	CStr		m_strPrevFileName;		// ǰһ���ļ�������·��
	CStr		m_strNextFileName;		// ��һ���ļ���,��·��
	
	int			m_nMultiFiles;;			// �Ƿ��¼һϵ�е��ļ�

	ACE_Time_Value	m_start_captime;
	unsigned int	m_ncaptime;			//�Ӵ��ļ���ʼ����೤ʱ�䣬��λΪ��

	DWORD		m_dwBufSize;			// �����С

	DWORD		m_dwMaxSize;		    // ÿ���ļ����ֵ

	GET_NEXT_FILE_NAME_FUNC m_pfnGetNextFile;
	void *		m_pParam;				

	CShareMem	*m_pShareMem;					// �������ݵĹ����ڴ�
	CAP_OP_STRUCT * m_pCapShareOpBuf;			// �����ڴ�ָ��
	

	CCPTSingleWrite m_SingleWriteObj;	// д��Ķ���
	CCapDataRead m_ReadObj;
};

#endif // !defined(AFX_CAPSAVE_H__6CC18264_0FC4_4C4F_97ED_5D5609846165__INCLUDED_)
