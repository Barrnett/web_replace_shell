
#include "cpf/cpf.h"
#include <vector> 

#ifdef OS_WINDOWS

// 在EXE的初始化代码处，调用下行代码，可以在程序崩溃的时候，生成DUMP文件
//
// SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
//


CPF_EXPORT LONG ApplicationCrashHandler_standard(EXCEPTION_POINTERS *pException);

// 生成的dump文件，需要放在程序和pdb所在的目录下，双击后用vc打开，点击run，则会停止在异常产生的位置
CPF_EXPORT void CreateDumpFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);

// 生成CallStack文件，要求程序目录下存在对应的pdb文件，否则看不到自己代码的堆栈信息
CPF_EXPORT void CreateCallStackFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);


#endif // OS_WINDOWS

