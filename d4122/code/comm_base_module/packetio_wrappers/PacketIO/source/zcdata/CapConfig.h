#ifndef _CAPCONFIG_SUNXG_2003_5_27
#define _CAPCONFIG_SUNXG_2003_5_27

//--------------------------------------------------------------------------------
// �ļ�����		CapConfig.h
// �����ߣ�		
// ����ʱ�䣺	2003.5.27
// ����������	���岶������,���������ڴ���ļ�
// �޸ļ�¼��
// �޸�ʱ��	�޸���		�޸�����
// ��
//--------------------------------------------------------------------------------

#include "Str.h"

class CCapConfig
{
public:
	CCapConfig()
	{
		bCapToMem = FALSE;
		bUseFilter = FALSE;
		bSimpleFilter = TRUE;
		bMultiFile = TRUE;
	}
public:
	CStr strFilePath;
	BOOL bMultiFile;
	int nFileSizeMb;		// ��������ڴ�,��Ϊ�ڴ��С
	BOOL bUseFilter;
	BOOL bCPTOnly;			// ֻ����CPT�ļ���������.NST�ļ�
	BOOL bSimpleFilter;		// �Ƿ�򵥹���
	BOOL bCapToMem;			// �Ƿ񲶻��ڴ�
	CStr strFilterName;	// ����������
};				


#endif
