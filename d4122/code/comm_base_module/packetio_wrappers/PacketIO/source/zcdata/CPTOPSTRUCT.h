#ifndef _CPTOPSTRUCT_H_SUNXG_2003_5_21
#define _CPTOPSTRUCT_H_SUNXG_2003_5_21
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


#include "CapFileHead.h"

#define MAX_ONE_CPT_INDEX	200000	// ��ÿ����Ϊ0����,ͷ��22�ֽ�,1000������һ������,��4G�����ļ����㹻ʹ��

#pragma pack(1)

typedef struct 
{
	BOOL		m_bWriting;					// �ù������Ƿ���CCPTSingleWrite����

	char		m_strFilePathName[MAX_PATH];	// ��·�������ļ���
	
	DWORD		m_dwPacketCount;			// ���ļ����Ѿ���¼�İ���
	

	DWORD		m_dwMapSize;				// ӳ���ļ���С
	DWORD		m_dwContentEnd;				// ʵ�����ݵ�β��
	
	CAPFILE_HEAD m_filehead;				// �ļ�ͷ
	
	DWORD		m_dwIndexCount;				// ��������
	DWORD		m_adwPktIndex[MAX_ONE_CPT_INDEX];//Լ781K��������
	
} CPT_OP_STRUCT;

#pragma pack()

// ���������¼����ȡ��һ���ļ����Ļص�����������ֵӦ�ò���·�����FILENAME_LEN
typedef LPCSTR (* GET_NEXT_FILE_NAME_FUNC) (void * pParam);



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
#endif//_CPTOPSTRUCT_H_SUNXG_2003_5_21


