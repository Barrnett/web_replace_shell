
#include "cpf/cpf.h"
#include <vector> 

#ifdef OS_WINDOWS

// ��EXE�ĳ�ʼ�����봦���������д��룬�����ڳ��������ʱ������DUMP�ļ�
//
// SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
//


CPF_EXPORT LONG ApplicationCrashHandler_standard(EXCEPTION_POINTERS *pException);

// ���ɵ�dump�ļ�����Ҫ���ڳ����pdb���ڵ�Ŀ¼�£�˫������vc�򿪣����run�����ֹͣ���쳣������λ��
CPF_EXPORT void CreateDumpFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);

// ����CallStack�ļ���Ҫ�����Ŀ¼�´��ڶ�Ӧ��pdb�ļ������򿴲����Լ�����Ķ�ջ��Ϣ
CPF_EXPORT void CreateCallStackFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);


#endif // OS_WINDOWS

