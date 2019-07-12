#ifndef _PATH_TOOLS_H_ZHUZP_2006_3_11
#define _PATH_TOOLS_H_ZHUZP_2006_3_11
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"

namespace CPF
{
	//判断一个是文件或者路径
	CPF_EXPORT
	BOOL IsDirectory(LPACECTSTR path);

	CPF_EXPORT
	ACE_TCHAR * GetLastFileNameSeperator(LPACECTSTR strPathFile);


	//目录下所有文件与子目录,
	//bdelete_self=true,dirname目录本身也删除，否则dirname目录本身不删除
	CPF_EXPORT
	BOOL DeleteDirectory(LPACECTSTR dirname,BOOL bdelete_self,ACE_SCANDIR_SELECTOR selector = NULL);

	//拷贝子目录中的所有文件和文件夹
	CPF_EXPORT
	BOOL CopySubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector = NULL);

	//拷贝整个目录
	CPF_EXPORT
	BOOL CopyThisandSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector = NULL);

	//移动子目录中的所有文件和文件夹
	CPF_EXPORT
	BOOL MoveSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector = NULL);

	//移动整个目录
	CPF_EXPORT
	BOOL MoveThisandSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector = NULL);

	//如果是拷贝，则将sourcedirname下面的文件及目录全部拷贝到destdirname下面(不包含本身)。
	//removeflag表示是移动目录还是删除目录.
	CPF_EXPORT
	BOOL MoveOrCopySubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,BOOL removeflag,ACE_SCANDIR_SELECTOR selector = NULL);

	//如果是拷贝，则将sourcedirname本身以及下面的文件及目录全部拷贝到destdirname下面
	//removeflag表示是移动目录还是删除目录.
	CPF_EXPORT
	BOOL MoveOrCopyThisAndSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,BOOL removeflag,ACE_SCANDIR_SELECTOR selector = NULL);


	CPF_EXPORT
	LPACECTSTR GetLastDirNameFromFullName(LPACECTSTR strPath);

	CPF_EXPORT
	void freedirent(ACE_DIRENT **namelist,int itemnum);

	CPF_EXPORT
	BOOL IsFullPathName(const char * pName);
	CPF_EXPORT
	BOOL IsFullPathName(const wchar_t * pName);

	//看pName是否是全路径，如果不是，则加上当前程序的执行目录
	CPF_EXPORT
	LPACECTSTR TransToFullPathName(LPACETSTR outbuf,size_t bufsize,LPACECTSTR pName);

	CPF_EXPORT
		BOOL CreateFullDirecory(LPACECTSTR fullpathname,size_t offset=0);

	CPF_EXPORT
		//把lpszPathORFileName添加到lpBuffer中，自动去重对于的路径分隔符
		//比如 lpBuffer = "c:\aa\"  lpszPathORFileName="\bb\cc\"
		//最后的结果是 "c:\aa\bb\cc\"
	LPACECTSTR AddToPath(LPACETSTR lpBuffer, LPACECTSTR lpszPathORFileName);

	CPF_EXPORT
		LPACECTSTR JoinPathToPath(LPACETSTR lpBuffer, size_t nBufSize,
			LPACECTSTR lpszPath, LPACECTSTR lpszPathORFileName);

	//从一个完整文件名中,得到除了文件名之外的全路径名,不包含最后一个 '\\'
	CPF_EXPORT
		LPACECTSTR GetPathNameFromFullName(LPACETSTR fullName, size_t nSizeFullName,LPACECTSTR strPathFile);

	//从一个完整文件名中,得到没有路径的文件名,
	//得到的指针还是指着strPathFile里面的内容
	CPF_EXPORT
		LPACECTSTR GetFileNameFromFullName(LPACECTSTR strPathFile);

	//替换掉文件名的后缀。如果suf=NULL，表示不要后缀。
	//如果以前没有后，则加上后缀
	CPF_EXPORT
		void ReplaceSufFromFileName(LPACECTSTR strPathFile,const char * suf,std::string& newfilename);

	//得到文件的后缀名称的指针，如果没有后缀返回NULL。
	//如果得到，返回的指针在strPathFile的内部。
	CPF_EXPORT
		LPACECTSTR GetSufFromFileName(LPACECTSTR strPathFile);

	CPF_EXPORT
	inline BOOL GetSufFromFileName(LPACECTSTR strPathFile,STD_TString& strSuf)
	{
		LPACECTSTR pSuf = GetSufFromFileName(strPathFile);

		if( pSuf )
		{
			strSuf = pSuf;
			return true;
		}

		strSuf.clear();
		return false;
	}

	CPF_EXPORT
	inline STD_TString GetSufFromFileName2(LPACECTSTR strPathFile)
	{
		LPACECTSTR pSuf = GetSufFromFileName(strPathFile);

		if( pSuf )
		{
			return std::string(pSuf);
		}
		
		return STD_TString("");
	}

	//得到当前执行程序的目录，不带最后一个'\\'

	CPF_EXPORT
		LPACECTSTR GetModulePathName(LPACETSTR fullName, size_t nSizeFullName);


	CPF_EXPORT 
		//得到当前执行文件名的全称
		LPACECTSTR GetModuleFileName(LPACETSTR fullName, size_t nSizeFullName);


	CPF_EXPORT 
		LPACECTSTR GetModuleFullFileName(LPACETSTR fullName, size_t nSizeFullName, LPACECTSTR fileName);

	//如果最后的字符不是'\'或者'/',则补上。用户要保证缓冲区足够
	//如果补了最后一个符合，则返回true，否则返回false
	CPF_EXPORT
		BOOL AddLastFileNameSep(STD_TString& filepath);

	CPF_EXPORT
		BOOL AddLastFileNameSep(LPACETSTR filepath,size_t len);

	CPF_EXPORT
		BOOL AddLastFileNameSep(LPACETSTR filepath);

	CPF_EXPORT
		void DelLastFileNameSep(LPACETSTR filename);

	CPF_EXPORT
		void DelLastFileNameSep(STD_TString& filepath);

	//得到应用程序目录下的下级文件的完全路径名
	//比如得到\\ini_ch\all.ini:pPath为ini_ch，fileName为All.ini
	//参数中pPath可以前后带'\'，也可以不带。
	//fileName可以在前面带或者不带'\'	
	CPF_EXPORT
		LPACECTSTR GetModuleOtherFileName(LPACETSTR fullName, size_t nSizeFullName,LPACECTSTR pPath,LPACECTSTR fileName);

#ifdef _WINDOWS_
	CPF_EXPORT
		LPACECTSTR GetWindowsSystem32Directory(LPACETSTR directoryBuffer, size_t nSize);

	CPF_EXPORT
		LPACECTSTR GetWindowsSystem32FilePath(LPACETSTR directoryBuffer, size_t nSize, LPACECTSTR filename);
#endif//_WINDOWS_

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////利用线程专储
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CPF_EXPORT
		LPACECTSTR JoinPathToPath(LPACECTSTR lpszPath, LPACECTSTR lpszPathORFileName);

	CPF_EXPORT
		LPACECTSTR GetPathNameFromFullName(LPACECTSTR strPathFile);
	
	CPF_EXPORT 
		LPACECTSTR GetModulePathName();

	CPF_EXPORT 
		//得到当前执行文件名的全称
		LPACECTSTR GetModuleFileName();

	CPF_EXPORT
		LPACECTSTR TransToFullPathName(LPACECTSTR pName);

	
	CPF_EXPORT 
		LPACECTSTR GetModuleFullFileName(LPACECTSTR fileName);

	CPF_EXPORT
		LPACECTSTR GetModuleOtherFileName(LPACECTSTR pPath,LPACECTSTR fileName);

#ifdef _WINDOWS_
	CPF_EXPORT
		LPACECTSTR GetWindowsSystem32Directory();

	CPF_EXPORT
		LPACECTSTR GetWindowsSystem32FilePath(LPACECTSTR filename);
#endif//_WINDOWS_

}


///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_PATH_TOOLS_H_ZHUZP_2006_3_11


