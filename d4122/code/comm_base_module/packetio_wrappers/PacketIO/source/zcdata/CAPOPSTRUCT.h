#ifndef _CAPOPSTRUCT_H_SUNXG_2003_5_21
#define _CAPOPSTRUCT_H_SUNXG_2003_5_21

#include "CapFileHead.h"
#include "cpf/MyStr.h"

#define MAX_SINGLE_CPT	10000		// ��ÿ����Ϊ0����,ͷ��22�ֽ�,1000������һ������,��4G�����ļ����㹻ʹ��

#pragma pack(1)


typedef struct 
{
	ACE_UINT64 ullPktFirstPos;		// ���ļ��еļ�¼��һ��֡ȫ��λ��
	ACE_UINT64 ullPktEndPos;			// ���ļ��еļ�¼���ȫ��λ��
	ACE_UINT64 ullFirstFrame;		// ���ļ��еļ�¼��һ��֡ȫ��֡���
	DWORD dwFrameCount;				// ���ļ��м�¼��֡�ĸ���
	char szFileName[MAX_PATH];		// ����·���ļ���
}SINGLE_FILE_INFO; 	//�����ļ�����

typedef struct 
{
	BOOL		m_bWriting;					// �ù������Ƿ���CCPTSingleWrite����
	
	char		m_strOrgFilePathName[MAX_PATH];	// ��·��ԭʼ�ļ���
	
	int			m_nFileCount;					// �ļ�����
	ACE_UINT64   m_ullTotalFrame;				// ������
    ACE_UINT64   m_ullTotalSize;				    // �����ļ���С
    SINGLE_FILE_INFO m_Files[MAX_SINGLE_CPT];	// �ļ��б�
	
} CAP_OP_STRUCT;

#pragma pack()

// ���������¼����ȡ��һ���ļ����Ļص�����������ֵӦ�ò���·�����FILENAME_LEN
typedef LPCSTR (* GET_NEXT_FILE_NAME_FUNC) (void * pParam);

// �����ļ���һһ��Ӧ�Ĺ����ڴ湲������Ҫ�����в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
void CAPMakeShareMemName(LPCSTR szFileName,CStr& strSMName);

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#endif


