
#include "stdafx.h"
#include "cpf/CrushDumpFun.h"


#ifdef OS_WINDOWS

#include <Windows.h>  
#include <DbgHelp.h>  
#include <iostream>
#include "ace/OS_NS_stdio.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"

#pragma comment(lib, "dbghelp.lib")  

#define DEF_DUMP_CALLSTACK_TXT	0

#define MAX_ADDRESS_LENGTH	32
#define MAX_NAME_LENGTH		1024

//{
// 崩溃信息  
//   
struct CrashInfo  
{  
	CHAR ErrorCode[MAX_ADDRESS_LENGTH];  
	CHAR Address[MAX_ADDRESS_LENGTH];  
	CHAR Flags[MAX_ADDRESS_LENGTH];  
};  

// CallStack信息  
//   
struct CallStackInfo  
{  
	CHAR ModuleName[MAX_NAME_LENGTH];  
	CHAR MethodName[MAX_NAME_LENGTH];  
	CHAR FileName[MAX_NAME_LENGTH];  
	CHAR LineNumber[MAX_NAME_LENGTH];  
};  

void SafeStrCpy(char* szDest, size_t nMaxDestSize, const char* szSrc);
CrashInfo GetCrashInfo(const EXCEPTION_RECORD *pRecord);
std::vector<CallStackInfo> GetCallStack(const CONTEXT *pContext);

//}


LONG ApplicationCrashHandler_standard(EXCEPTION_POINTERS *pException)  
{
	char str_path[MAX_NAME_LENGTH];
	char str_full_name[MAX_NAME_LENGTH];

	CPF::GetModuleFileName(str_path, sizeof(str_path));

	ACE_TCHAR* pExeName = CPF::GetLastFileNameSeperator(str_path);
	if( !pExeName )
	{
		ACE_ASSERT(FALSE);
		exit(-1);
	}

	*(pExeName) = 0;
	pExeName ++;

	ACE_OS::sprintf(str_full_name, "%s\\dump\\", str_path);
	CPF::CreateFullDirecory(str_full_name);
	ACE_OS::sprintf(str_full_name, "%s\\dump\\%s_%s", str_path, pExeName, CPF::get_curtime_string(5));


	CreateDumpFile(str_full_name, pException);


#if DEF_DUMP_CALLSTACK_TXT
	CreateCallStackFile(str_full_name, pException);
#endif

	exit(-1);


	return EXCEPTION_EXECUTE_HANDLER;  
}


// 创建Dump文件  
//   
void CreateDumpFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
{
	std::string str_path_name = lpstrDumpFilePathName;
	str_path_name += ".dmp";

	// 创建Dump文件  
	//  
	HANDLE hDumpFile = CreateFile(str_path_name.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	// Dump信息  
	//  
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
	dumpInfo.ExceptionPointers = pException;  
	dumpInfo.ThreadId = GetCurrentThreadId();  
	dumpInfo.ClientPointers = TRUE;  

	// 写入Dump文件内容  
	//  
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  

	CloseHandle(hDumpFile);  
}  


void CreateCallStackFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	std::string str_path_name = lpstrDumpFilePathName;
	str_path_name += ".txt";

	// 确保有足够的栈空间  
	//  
#ifdef _M_IX86  
	if (pException->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)  
	{  
		static char TempStack[1024 * 128];  
		__asm mov eax,offset TempStack[1024 * 128];  
		__asm mov esp,eax;  
	}  
#endif    

	CrashInfo crashinfo = GetCrashInfo(pException->ExceptionRecord);  
//	FILE* fp=fopen("D:\\11.temp\\CrushDump\\Release\\log.txt", "w");
	FILE* fp=fopen(str_path_name.c_str(), "w");
	if (NULL==fp)
	{
//		FatalAppExit(-1,  _T("*** 文件无法打开! ***"));
		return;
	}

	// 输出Crash信息
	//
	fprintf(fp, "%s\n", crashinfo.ErrorCode);
	fprintf(fp, "%s\n", crashinfo.Address);
	fprintf(fp, "%s\n", crashinfo.Flags);
// 	cout << "ErrorCode: " << crashinfo.ErrorCode << endl;  
// 	cout << "Address: " << crashinfo.Address << endl;  
// 	cout << "Flags: " << crashinfo.Flags << endl;  

	std::vector<CallStackInfo> arrCallStackInfo = GetCallStack(pException->ContextRecord);  

	// 输出CallStack  
	//  
	fprintf(fp, "\nCallStack: \n");
// 	cout << "CallStack: " << endl;  
	for (std::vector<CallStackInfo>::iterator i = arrCallStackInfo.begin(); i != arrCallStackInfo.end(); ++i)  
	{  
		CallStackInfo callstackinfo = (*i);  

		fprintf(fp, "%s() : [: %s] (File: %s @Line %s)\n", 
			callstackinfo.MethodName, 
			callstackinfo.ModuleName,
			callstackinfo.FileName,
			callstackinfo.LineNumber);
// 		cout << callstackinfo.MethodName << "() : [" 
// 			<< callstackinfo.ModuleName << "] (File: " 
// 			<< callstackinfo.FileName << " @Line " 
// 			<< callstackinfo.LineNumber << ")" << endl;  
	} 
	fclose(fp);

	// 这里以弹出一个错误对话框并退出程序为例子  
	//  
//	FatalAppExit(-1,  _T("*** Unhandled Exception! ***"));  
} 



// 安全拷贝字符串函数  
//  
void SafeStrCpy(char* szDest, size_t nMaxDestSize, const char* szSrc)  
{  
	if (nMaxDestSize <= 0) return;  
	if (strlen(szSrc) < nMaxDestSize)  
	{  
		strcpy_s(szDest, nMaxDestSize, szSrc);  
	}  
	else  
	{  
		strncpy_s(szDest, nMaxDestSize, szSrc, nMaxDestSize);  
		szDest[nMaxDestSize-1] = '\0';  
	}  
}    

// 得到程序崩溃信息  
//  
CrashInfo GetCrashInfo(const EXCEPTION_RECORD *pRecord)  
{  
	CrashInfo crashinfo;  
	SafeStrCpy(crashinfo.Address, MAX_ADDRESS_LENGTH, "N/A");  
	SafeStrCpy(crashinfo.ErrorCode, MAX_ADDRESS_LENGTH, "N/A");  
	SafeStrCpy(crashinfo.Flags, MAX_ADDRESS_LENGTH, "N/A");  

	sprintf_s(crashinfo.Address, "%08X", pRecord->ExceptionAddress);  
	sprintf_s(crashinfo.ErrorCode, "%08X", pRecord->ExceptionCode);  
	sprintf_s(crashinfo.Flags, "%08X", pRecord->ExceptionFlags);  

	return crashinfo;  
}  

// 得到CallStack信息  
//  
std::vector<CallStackInfo> GetCallStack(const CONTEXT *pContext)  
{  
	HANDLE hProcess = GetCurrentProcess();  

	SymInitialize(hProcess, NULL, TRUE);  

	std::vector<CallStackInfo> arrCallStackInfo;  

	CONTEXT c = *pContext;  

	STACKFRAME64 sf;  
	memset(&sf, 0, sizeof(STACKFRAME64));  
	DWORD dwImageType = IMAGE_FILE_MACHINE_I386;  

	// 不同的CPU类型，具体信息可查询MSDN  
	//  
#ifdef _M_IX86  
	sf.AddrPC.Offset = c.Eip;  
	sf.AddrPC.Mode = AddrModeFlat;  
	sf.AddrStack.Offset = c.Esp;  
	sf.AddrStack.Mode = AddrModeFlat;  
	sf.AddrFrame.Offset = c.Ebp;  
	sf.AddrFrame.Mode = AddrModeFlat;  
#elif _M_X64  
	dwImageType = IMAGE_FILE_MACHINE_AMD64;  
	sf.AddrPC.Offset = c.Rip;  
	sf.AddrPC.Mode = AddrModeFlat;  
	sf.AddrFrame.Offset = c.Rsp;  
	sf.AddrFrame.Mode = AddrModeFlat;  
	sf.AddrStack.Offset = c.Rsp;  
	sf.AddrStack.Mode = AddrModeFlat;  
#elif _M_IA64  
	dwImageType = IMAGE_FILE_MACHINE_IA64;  
	sf.AddrPC.Offset = c.StIIP;  
	sf.AddrPC.Mode = AddrModeFlat;  
	sf.AddrFrame.Offset = c.IntSp;  
	sf.AddrFrame.Mode = AddrModeFlat;  
	sf.AddrBStore.Offset = c.RsBSP;  
	sf.AddrBStore.Mode = AddrModeFlat;  
	sf.AddrStack.Offset = c.IntSp;  
	sf.AddrStack.Mode = AddrModeFlat;  
#else  
#error "Platform not supported!"  
#endif  

	HANDLE hThread = GetCurrentThread();  

	while (true)  
	{  
		// 该函数是实现这个功能的最重要的一个函数  
		// 函数的用法以及参数和返回值的具体解释可以查询MSDN  
		//  
		if (!StackWalk64(dwImageType, hProcess, hThread, &sf, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))  
		{  
			break;  
		}  

		if (sf.AddrFrame.Offset == 0)  
		{  
			break;  
		}  

		CallStackInfo callstackinfo;  
		SafeStrCpy(callstackinfo.MethodName, MAX_NAME_LENGTH, "N/A");  
		SafeStrCpy(callstackinfo.FileName, MAX_NAME_LENGTH, "N/A");  
		SafeStrCpy(callstackinfo.ModuleName, MAX_NAME_LENGTH, "N/A");  
		SafeStrCpy(callstackinfo.LineNumber, MAX_NAME_LENGTH, "N/A");  

		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH];  
		IMAGEHLP_SYMBOL64 *pSymbol = (IMAGEHLP_SYMBOL64*)symbolBuffer;  
		memset(pSymbol, 0, sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH);  

		pSymbol->SizeOfStruct = sizeof(symbolBuffer);  
		pSymbol->MaxNameLength = MAX_NAME_LENGTH;  

		DWORD symDisplacement = 0;  

		// 得到函数名  
		//  
		if (SymGetSymFromAddr64(hProcess, sf.AddrPC.Offset, NULL, pSymbol))  
		{  
			SafeStrCpy(callstackinfo.MethodName, MAX_NAME_LENGTH, pSymbol->Name);  
		}  

		IMAGEHLP_LINE64 lineInfo;  
		memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));  

		lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);  

		DWORD dwLineDisplacement;  

		// 得到文件名和所在的代码行  
		//  
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))  
		{  
			SafeStrCpy(callstackinfo.FileName, MAX_NAME_LENGTH, lineInfo.FileName);  
			sprintf_s(callstackinfo.LineNumber, "%d", lineInfo.LineNumber);  
		}  

		IMAGEHLP_MODULE64 moduleInfo;  
		memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));  

		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);  

		// 得到模块名  
		//  
		if (SymGetModuleInfo64(hProcess, sf.AddrPC.Offset, &moduleInfo))  
		{  
			SafeStrCpy(callstackinfo.ModuleName, MAX_NAME_LENGTH, moduleInfo.ModuleName);  
		}  

		arrCallStackInfo.push_back(callstackinfo);  
	}  

	SymCleanup(hProcess);  

	return arrCallStackInfo;  
}  


#endif // OS_WINDOWS