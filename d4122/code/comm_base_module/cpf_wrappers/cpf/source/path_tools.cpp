#include "cpf/path_tools.h"
#include "ace/TSS_T.h"
#include "ace/OS_NS_dirent.h"

#ifdef OS_WINDOWS
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#endif

class CArithPathBuffer
{
public:
	ACE_TCHAR	m_buffer[MAX_PATH];
};

//得到最后一个'/'或者'\'
ACE_TCHAR * CPF::GetLastFileNameSeperator(LPACECTSTR strPathFile)
{
	ACE_TCHAR* p1 = (ACE_TCHAR*)ACE_OS::strrchr(strPathFile, WIN_FILENAME_SEPERATOR);

	ACE_TCHAR * p2 = (ACE_TCHAR*)ACE_OS::strrchr(strPathFile, LINUX_FILENAME_SEPERATOR);

	if( p1 && p2 )
	{
		return mymax(p1,p2);
	}
	else if( p1 )
	{
		return p1;
	}
	else
	{
		return p2;
	}
}


BOOL CPF::IsDirectory(LPACECTSTR path)
{	
	ACE_DIR * pDir = ACE_OS::opendir(path);

	if( pDir )
	{
		ACE_OS::closedir(pDir);
		return true;
	}

	return false;
}
//------------------------------------------------------------------------
// Function:	BOOL CopySubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname)
// Purpose:		将＂soucedirname＂路径所指的目录（不包括“soucedirname”目录）下的所有文件和文件夹拷贝到
//				＂soucedirname＂目录下。
// Parameters:	soucedirname 	- 	[in]文件源路径。
//				destdirname	-		[in]目的路径。
//				
// Returns:		TRUE:sucess FALSE:failed
// Author:		liuhuihua, 2006-11-13
//notice:		实现的时候是将一个目录下的文件所有的文件和文件夹从“soucedirname”
//				拷贝到“destdirname”目录下．
//------------------------------------------------------------------------
BOOL CPF::CopySubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector)
{
	if( soucedirname && soucedirname[0]
	&& destdirname && destdirname[0] )
	{
		return MoveOrCopySubDirectory(soucedirname,destdirname,false,selector);
	}

	return false;

}
//------------------------------------------------------------------------
// Function:	BOOL CopyThisandSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname)
// Purpose:		将＂soucedirname＂路径所指的目录（包括“soucedirname”目录）下的所有文件和文件夹拷贝到
//				＂soucedirname＂目录下。
// Parameters:	soucedirname 	- 	[in]文件源路径。
//				destdirname	-		[in]目的路径。
//				
// Returns:		TRUE:sucess FALSE:failed
// Author:		liuhuihua, 2006-11-13
//notice:		实现的时候是将一个目录下的文件所有的文件和文件夹从“soucedirname”
//				拷贝到“destdirname”目录下．
//------------------------------------------------------------------------
BOOL CPF::CopyThisandSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector)
{
	if( soucedirname && soucedirname[0]
	&& destdirname && destdirname[0] )
	{
		return MoveOrCopyThisAndSubDirectory(soucedirname,destdirname,false,selector);
	}

	return false;
}

//------------------------------------------------------------------------
// Function:	BOOL MoveSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname)
// Purpose:		将＂soucedirname＂（不包括“soucedirname”目录）目录下的所有文件和文件夹
//				移动到＂destdirname＂的目录下。
// Parameters:	soucedirname 	- 	[in]文件源路径。
//				destdirname	-		[in]目的路径。
//
// Returns:		TRUE:sucess FALSE:failed
// Author:		liuhuihua, 2006-11-13
//notice:		实现的时候是将一个目录下的文件所有的文件和文件夹从“soucedirname”
//				拷贝到“destdirname”目录下，然后将“soucedirname”目录下的文件
//				文件夹保留。
//------------------------------------------------------------------------
BOOL CPF::MoveSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector)
{
	if( soucedirname && soucedirname[0]
	&& destdirname && destdirname[0] )
	{
		return MoveOrCopySubDirectory(soucedirname,destdirname,true,selector);
	}

	return false;
}

//------------------------------------------------------------------------
// Function:	MoveThisandubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname)
// Purpose:		将＂soucedirname＂(包括“soucedirname”目录)路径所指的目录下的所有文件
//				和文件夹转移到＂soucedirname＂目录下。
// Parameters:	soucedirname 	- 	[in]文件源路径。
//				destdirname	-		[in]目的路径。
//				
// Returns:		TRUE:sucess FALSE:failed
// Author:		liuhuihua, 2006-11-13
//notice:		实现的时候是将一个目录下的文件所有的文件和文件夹从“soucedirname”
//				拷贝到“destdirname”目录下，然后将“soucedirname”目录删除．
//------------------------------------------------------------------------
BOOL CPF::MoveThisandSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector)
{
	if( soucedirname && soucedirname[0]
	&& destdirname && destdirname[0] )
	{
		return MoveOrCopyThisAndSubDirectory(soucedirname,destdirname,true,selector);
	}

	return false;
}

//------------------------------------------------------------------------
// Function:	BOOL MoveOrCopySubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,BOOL removeflag);

// Purpose:		Move or copy all files from "soucedirname" to "destdirname",.

//removeflag表示是移动目录还是删除目录.

// Returns:		TRUE:sucess FALSE:failed
//------------------------------------------------------------------------
BOOL CPF::MoveOrCopySubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,BOOL removeflag,ACE_SCANDIR_SELECTOR selector)
{
	if( !(soucedirname && soucedirname[0]
	&& destdirname && destdirname[0] ) )
	{
		return false;
	}

	//检查源路径长度是否超过最大长度，超过最大长度部分将返回
	size_t sourcefiledirlen = ACE_OS::strlen(soucedirname);

	if (sourcefiledirlen >= MAX_PATH-1)
	{
		return FALSE;
	}

	ACE_TCHAR sourcefiledir[MAX_PATH];

	ACE_OS::strcpy(sourcefiledir,soucedirname);

	sourcefiledir[sourcefiledirlen] = 0;

	if( AddLastFileNameSep(sourcefiledir,sourcefiledirlen) )
	{
		++sourcefiledirlen;
	}


	//检查目的路径长度是否超过最大长度，超过最大长度部分将返回
	size_t destfiledirlen = ACE_OS::strlen(destdirname);

	if(destfiledirlen >= MAX_PATH-1)
	{
		return FALSE;
	}

	ACE_TCHAR destfiledir[MAX_PATH];

	ACE_OS::strcpy(destfiledir,destdirname);

	destfiledir[destfiledirlen] = 0;

	if( AddLastFileNameSep(destfiledir,destfiledirlen) )
	{
		++destfiledirlen;
	}

	//先创建目标路径
	CPF::CreateFullDirecory(destdirname);

	//扫描源目录的所有文件
	ACE_DIRENT **namelist ;

	int nEntityNum = ACE_OS::scandir((ACE_TCHAR*)soucedirname,&namelist,selector,0);

	//如果目录为空，返回
	if(nEntityNum<=0) return FALSE;

	//拷贝目录中的文件
	for(int nFileIndex=0;nFileIndex<nEntityNum;nFileIndex++)
	{
		if( strcmp(namelist[nFileIndex]->d_name,".") == 0 )
			continue;
		if( strcmp(namelist[nFileIndex]->d_name ,"..") == 0 )
			continue;

		ACE_OS::strcpy(sourcefiledir+sourcefiledirlen,namelist[nFileIndex]->d_name);

		if(destfiledirlen + strlen(namelist[nFileIndex]->d_name) >= MAX_PATH-1 ) 
		{
			return FALSE;
		}

		ACE_OS::strcpy(destfiledir+destfiledirlen,namelist[nFileIndex]->d_name);
		
		if (IsDirectory(sourcefiledir))
		{//如果为目录，递归调用

			MoveOrCopySubDirectory(sourcefiledir,destfiledir,removeflag);

			if( removeflag )
			{
				ACE_OS::rmdir(sourcefiledir);
			}
		}
		else
		{
			CopyFile(sourcefiledir,destfiledir,FALSE);

			if( removeflag )
			{
				ACE_OS::unlink(sourcefiledir);
			}
		}
	}

	//释放空间
	freedirent(namelist,nEntityNum);

	return TRUE;
}

//如果是拷贝，则将sourcedirname本身以及下面的文件及目录全部拷贝到destdirname下面
//removeflag表示是移动目录还是删除目录.
BOOL CPF::MoveOrCopyThisAndSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,BOOL removeflag,ACE_SCANDIR_SELECTOR selector)
{
	if( !(soucedirname && soucedirname[0]
	&& destdirname && destdirname[0] ) )
	{
		return false;
	}

	LPACECTSTR last_dir = CPF::GetLastDirNameFromFullName(soucedirname);

	ACE_TCHAR destfiledir[MAX_PATH];

	ACE_OS::strcpy(destfiledir,destdirname);

	AddLastFileNameSep(destfiledir);

	ACE_OS::strcat(destfiledir,last_dir);

	CreateFullDirecory(destfiledir);

	MoveOrCopySubDirectory(soucedirname,destfiledir,removeflag,selector);

	if( removeflag )
	{
		ACE_OS::rmdir(soucedirname);
	}

	return true;
}


LPACECTSTR CPF::GetLastDirNameFromFullName(LPACECTSTR strPath)
{
	ACE_ASSERT(strPath && ACE_OS::strlen(strPath) > 0 );

	if( !strPath || ACE_OS::strlen(strPath) <=0 )
		return NULL;

	if( !IsDirectory(strPath) )
		return NULL;

	static ACE_TCHAR strPathTemp[MAX_PATH];

	size_t pathlen = ACE_OS::strlen(strPath);

	if( IS_FILENAME_SEPERATOR(strPath[pathlen-1]))
	{
		ACE_OS::memcpy(strPathTemp,strPath,pathlen-1) ;
		strPathTemp[pathlen-1] = 0;
	}
	else
	{
		ACE_OS::memcpy(strPathTemp,strPath,pathlen) ;
		strPathTemp[pathlen] = 0;
	}

	ACE_TCHAR* p = GetLastFileNameSeperator(strPathTemp);

	if(p == NULL)
		return strPath;

	return (LPACECTSTR)(p + 1);
}



//------------------------------------------------------------------------
// Function:	DeleteDirectory(LPACECTSTR dirname)
// Purpose:		delet all files from "dirname" ,then delete the dir "dirname",.
// Parameters:	soucedirname 	- 	[in]the dir name will be deleted。
// Returns:		TRUE:sucess FALSE:failed
// Author:		刘会华, 2006-11-13
//notice:		实现的时候是将一个目录下的文件所有的文件和文件夹文件夹删除。
//				然后删除根目录。
//------------------------------------------------------------------------
BOOL CPF::DeleteDirectory(LPACECTSTR dirname,BOOL bdelete_self,ACE_SCANDIR_SELECTOR selector)
{
	STD_TString path(dirname);
	AddLastFileNameSep(path);

	size_t filedirlen = path.size();

	if (filedirlen >= MAX_PATH-1)
	{
		return FALSE;
	}

	ACE_TCHAR filedir[MAX_PATH];

	ACE_OS::strcpy(filedir,path.c_str());

	filedir[filedirlen] = 0;

	//扫描目录的文件
	ACE_DIRENT **namelist ;

	int nEntityNum = ACE_OS::scandir((ACE_TCHAR*)dirname,&namelist,selector,0);

	if(nEntityNum<=0) return FALSE;

	//删除目录中的文件
	for(int nFileIndex=0;nFileIndex<nEntityNum;nFileIndex++)
	{
		if( strcmp(namelist[nFileIndex]->d_name,".") == 0 )
			continue;
		if( strcmp(namelist[nFileIndex]->d_name ,"..") == 0 )
			continue;
		ACE_OS::strcpy(filedir+filedirlen,namelist[nFileIndex]->d_name);

		//如果为目录，递归调用
		if (IsDirectory(filedir))
		{
			DeleteDirectory(filedir,true);
		}
		else
		{
			ACE_OS::unlink(filedir);
		}
	}

	if( bdelete_self )
	{
		//删除目录
		ACE_OS::rmdir(dirname);
	}

	//释放空间
	freedirent(namelist,nEntityNum);

	return TRUE;

}


void CPF::freedirent(ACE_DIRENT **namelist,int itemnum)
{
	while (itemnum-- > 0)
	{
#if defined (ACE_LACKS_STRUCT_DIR)
		if (namelist[itemnum]->d_name)
		{
			ACE_OS::free (namelist[itemnum]->d_name);
		}
#endif
		ACE_OS::free (namelist[itemnum]);	
	}
	ACE_OS::free(namelist);


}

BOOL CPF::IsFullPathName(const char * pName)
{
#ifdef OS_WINDOWS
	return (pName[1] == ':');
#else
	return ( pName[0] == '/' );
#endif
}

BOOL CPF::IsFullPathName(const wchar_t * pName)
{
#ifdef OS_WINDOWS
		return (pName[1] == L':');
#else
		return ( pName[0] == L'/' );
#endif
}


const char * CPF::TransToFullPathName(LPACETSTR outbuf,size_t bufsize,LPACECTSTR pName)
{
	if( IsFullPathName(pName) )
	{
		if( bufsize <= ACE_OS::strlen(pName) )
			return NULL;

		strcpy(outbuf,pName);

		return outbuf;
	}
	else
	{
		if( pName[0] == ACE_TEXT('.') )
			++pName;

		return CPF::GetModuleFullFileName(outbuf,bufsize,pName);
	}

}

LPACECTSTR CPF::JoinPathToPath(LPACETSTR lpBuffer, size_t nBufSize,
							   LPACECTSTR lpszPath, LPACECTSTR lpszPathORFileName)
{
	ACE_ASSERT(lpBuffer && lpszPathORFileName );
	if (!lpBuffer || !lpszPathORFileName)
		return NULL;

	nBufSize /= sizeof(ACE_TCHAR);

	lpBuffer[0] = 0;

	if (lpszPath != NULL)
	{
		if (ACE_OS::strlen(lpszPath) >= nBufSize)
			return NULL;

		ACE_OS::strcpy(lpBuffer, lpszPath);

		ACE_TCHAR ch = lpszPath[ACE_OS::strlen(lpszPath)-1];

		if(!IS_FILENAME_SEPERATOR(ch))
		{
			strcat(lpBuffer, FILENAME_SEPERATOR_STR);
		}
	}

	if (lpszPathORFileName != NULL)
	{
		while( IS_FILENAME_SEPERATOR(*lpszPathORFileName) )
			++lpszPathORFileName;

		if (ACE_OS::strlen(lpBuffer) + ACE_OS::strlen(lpszPathORFileName) >= nBufSize)
			return NULL;

		ACE_OS::strcat(lpBuffer, lpszPathORFileName);
	}

	return lpBuffer;
}

LPACECTSTR CPF::AddToPath(LPACETSTR lpBuffer, LPACECTSTR lpszPathORFileName)
{
	if (lpBuffer != NULL && lpBuffer[0] )
	{
		ACE_TCHAR ch = lpBuffer[ACE_OS::strlen(lpBuffer)-1];

		if(!IS_FILENAME_SEPERATOR(ch))
		{
			strcat(lpBuffer, FILENAME_SEPERATOR_STR);
		}
	}

	if (lpszPathORFileName != NULL && lpszPathORFileName[0] )
	{
		while( IS_FILENAME_SEPERATOR(*lpszPathORFileName) )
			++lpszPathORFileName;

		ACE_OS::strcat(lpBuffer, lpszPathORFileName);
	}

	return lpBuffer;
}


//从一个完整文件名中,得到除了文件名之外的全路径名,不包含最后一个 '\\'
LPACECTSTR CPF::GetPathNameFromFullName(LPACETSTR fullName, size_t nSizeFullName, LPACECTSTR strPathFile)
{
	nSizeFullName /= sizeof(ACE_TCHAR);

	if( CPF::IsDirectory(strPathFile) )
	{
		if( ACE_OS::strlen(strPathFile) >= nSizeFullName )
			return NULL;

		strcpy(fullName,strPathFile);
	}
	else
	{
		ACE_TCHAR* p = GetLastFileNameSeperator(strPathFile);

		if( !p || p - strPathFile >= (int)nSizeFullName )
			return NULL;

		ACE_OS::strncpy(fullName,strPathFile,p-strPathFile);

		fullName[p-strPathFile] = 0;
	}

	return (LPACECTSTR)fullName;
}

LPACECTSTR CPF::GetFileNameFromFullName(LPACECTSTR strPathFile)
{
	ACE_ASSERT(strPathFile && ACE_OS::strlen(strPathFile) > 0 );

	if( !strPathFile || ACE_OS::strlen(strPathFile) <=0 )
		return NULL;

	if( CPF::IsDirectory(strPathFile) )
		return NULL;

	if( IS_FILENAME_SEPERATOR(strPathFile[ACE_OS::strlen(strPathFile)-1]))
		return NULL;

	ACE_TCHAR* p = GetLastFileNameSeperator(strPathFile);

	if(p == NULL)
		return strPathFile;

	return (LPACECTSTR)(p + 1);
}

LPACECTSTR CPF::GetSufFromFileName(LPACECTSTR strPathFile)
{
	if( strPathFile == NULL )
		return NULL;

	LPACECTSTR psuf = ACE_OS::strrchr(strPathFile,ACE_TEXT('.'));

	LPACECTSTR ptemp = NULL;
	
#ifdef OS_WINDOWS
	LPACECTSTR ptemp1 = ACE_OS::strrchr(strPathFile,ACE_TEXT('\\'));
	LPACECTSTR ptemp2 = ACE_OS::strrchr(strPathFile,ACE_TEXT('/'));

	ptemp = mymax(ptemp1,ptemp2);
#else
	ptemp = ACE_OS::strrchr(strPathFile,ACE_TEXT('/'));
#endif

	if( ptemp > psuf )
	{
		return NULL;
	}

	if( psuf )
	{
		++psuf;
	}

	return psuf;
}

CPF_EXPORT
void CPF::ReplaceSufFromFileName(LPACECTSTR strPathFile,const char * suf,std::string& newfilename)
{
	LPACECTSTR pos = GetSufFromFileName(strPathFile);

	if( pos )
	{
		newfilename.assign(strPathFile,pos-strPathFile-1);
	}
	else
	{
		newfilename = strPathFile;
	}

	if( suf )
	{
		newfilename += ".";
		newfilename += suf;
	}

	return;

}


CPF_EXPORT 
//得到当前执行文件名的全称
LPACECTSTR CPF::GetModuleFileName(LPACETSTR fullName, size_t nSizeFullName)
{
	fullName[0] = 0;

#if defined(_WINDOWS_)
	if(::GetModuleFileName(NULL, fullName, (DWORD)nSizeFullName) == 0)
#else
	memset( fullName, 0, nSizeFullName);	// linux必须全部清0，否则显示不对
	if(ACE_OS::readlink(ACE_TEXT("/proc/self/exe"), fullName, nSizeFullName) == -1)
#endif
	{
		return NULL;
	}

	return fullName;
}


CPF_EXPORT
LPACECTSTR CPF::GetModulePathName(LPACETSTR fullName, size_t nSizeFullName)
{
	if( !GetModuleFileName(fullName,nSizeFullName) )
	{
		return NULL;
	}

	ACE_TCHAR* p = GetLastFileNameSeperator(fullName);

	nSizeFullName /= sizeof(ACE_TCHAR);
	if( !p || p - fullName >= (int)nSizeFullName )
		return NULL;

	if (p == fullName)
		*(p+1) = 0;
	else
		*(p) = 0;

	return fullName;
}


LPACECTSTR CPF::GetModuleFullFileName(LPACETSTR fullName, size_t nSizeFullName, LPACECTSTR fileName)
{
	if( !CPF::GetModulePathName(fullName, nSizeFullName) )
		return NULL;

	nSizeFullName /= sizeof(ACE_TCHAR);

	if( ACE_OS::strlen(fileName) + ACE_OS::strlen(fullName) > nSizeFullName )
		return NULL;

	ACE_OS::strcat(fullName, FILENAME_SEPERATOR_STR);

	while(IS_FILENAME_SEPERATOR(*fileName))
		++fileName;

	ACE_OS::strcat( fullName, fileName );

	return fullName;
}

LPACECTSTR CPF::GetModuleOtherFileName(LPACETSTR fullName, size_t nSizeFullName, LPACECTSTR pPath,LPACECTSTR fileName)
{
	ACE_TCHAR temppath[MAX_PATH];

	if( !CPF::JoinPathToPath(temppath,sizeof(temppath),pPath,fileName) )
		return NULL;

	return CPF::GetModuleFullFileName(fullName,nSizeFullName,temppath);
}


BOOL CPF::AddLastFileNameSep(STD_TString& filepath)
{
	if( filepath.size()  > 0 )
	{
		if( !IS_FILENAME_SEPERATOR(filepath[filepath.size()-1]))
		{
			filepath += FILENAME_SEPERATOR;
			return true;
		}
	}

	return false;
}


BOOL CPF::AddLastFileNameSep(LPACETSTR filepath,size_t len)
{
	if( !IS_FILENAME_SEPERATOR(filepath[len-1]))
	{
		filepath[len] = FILENAME_SEPERATOR;
		filepath[len+1] = ACE_TEXT('\0');

		return true;
	}

	return false;
}


BOOL CPF::AddLastFileNameSep(LPACETSTR filepath)
{
	return AddLastFileNameSep(filepath,ACE_OS::strlen(filepath));
}

void CPF::DelLastFileNameSep(STD_TString& filepath)
{
	ACE_TCHAR * buffer = new ACE_TCHAR[filepath.length()+1];

	strcpy(buffer,filepath.c_str());

	CPF::DelLastFileNameSep(buffer);

	filepath = buffer;

	delete []buffer;

	return;
}

void CPF::DelLastFileNameSep(LPACETSTR filename)
{
	for(int i = (int)strlen(filename) -1 ; i >= 0; --i)
	{
		if( IS_FILENAME_SEPERATOR(filename[i]) )
		{
			filename[i] = 0;
		}
		else
		{
			break;
		}
	}

	return;
}


#ifdef _WINDOWS_
LPACECTSTR CPF::GetWindowsSystem32Directory(LPACETSTR directoryBuffer, size_t nSize)
{
	size_t uRet = GetWindowsDirectory(directoryBuffer, (UINT)nSize);
	if (0 == uRet)
	{
		return NULL;
	}

	if (IS_FILENAME_SEPERATOR(directoryBuffer[ACE_OS::strlen(directoryBuffer) - 1]))
	{
		directoryBuffer[uRet - 1] = '\0';
	}

	nSize /= sizeof(ACE_TCHAR);

	const ACE_TCHAR pszSystem32[] = ACE_TEXT("\\system32");

	if ( uRet+ACE_OS::strlen(pszSystem32) >= nSize)
	{
		return NULL;
	}

	ACE_OS::strcat(directoryBuffer, pszSystem32);

	return directoryBuffer;

}

LPACECTSTR CPF::GetWindowsSystem32FilePath(LPACETSTR directoryBuffer, size_t nSize, LPACECTSTR filename)
{
	if( !GetWindowsSystem32Directory(directoryBuffer, nSize) )
		return NULL;

	nSize /= sizeof(ACE_TCHAR);

	if( ACE_OS::strlen(directoryBuffer) + ACE_OS::strlen(filename) + 1 >= nSize)
		return NULL;

	ACE_OS::strcat(directoryBuffer,FILENAME_SEPERATOR_STR);

	while( IS_FILENAME_SEPERATOR(*filename) )
		++filename;

	ACE_OS::strcat(directoryBuffer,filename);

	return directoryBuffer;
}

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CPF_EXPORT
LPACECTSTR CPF::JoinPathToPath(LPACECTSTR lpszPath, LPACECTSTR lpszPathORFileName)
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::JoinPathToPath(ariPathBuffer->m_buffer,MAX_PATH*sizeof(ACE_TCHAR),
		lpszPath,lpszPathORFileName);
}


LPACECTSTR CPF::TransToFullPathName(LPACECTSTR pName)
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::TransToFullPathName(ariPathBuffer->m_buffer,MAX_PATH*sizeof(ACE_TCHAR),pName);

}

LPACECTSTR  CPF::GetPathNameFromFullName(LPACECTSTR strPathFile)
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::GetPathNameFromFullName(ariPathBuffer->m_buffer,MAX_PATH*sizeof(ACE_TCHAR),strPathFile);

}


LPACECTSTR CPF::GetModuleFileName()
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::GetModuleFileName( ariPathBuffer->m_buffer, MAX_PATH*sizeof(ACE_TCHAR));
}


//得到当前执行程序的目录，不带最后一个'\\'
LPACECTSTR CPF::GetModulePathName()
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::GetModulePathName( ariPathBuffer->m_buffer, MAX_PATH*sizeof(ACE_TCHAR));
}

LPACECTSTR CPF::GetModuleFullFileName(LPACECTSTR fileName)
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::GetModuleFullFileName( ariPathBuffer->m_buffer, MAX_PATH*sizeof(ACE_TCHAR), fileName);
}

LPACECTSTR CPF::GetModuleOtherFileName(LPACECTSTR pPath,LPACECTSTR fileName)
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::GetModuleOtherFileName(ariPathBuffer->m_buffer, MAX_PATH*sizeof(ACE_TCHAR),pPath,fileName);
}

#ifdef _WINDOWS_
LPACECTSTR CPF::GetWindowsSystem32Directory()
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::GetWindowsSystem32Directory(ariPathBuffer->m_buffer,MAX_PATH*sizeof(ACE_TCHAR));

}

LPACECTSTR CPF::GetWindowsSystem32FilePath(LPACECTSTR filename)
{
	static ACE_TSS<CArithPathBuffer>	ariPathBuffer;

	return CPF::GetWindowsSystem32FilePath(ariPathBuffer->m_buffer,MAX_PATH*sizeof(ACE_TCHAR), filename);

}
#endif

// 新建一个Depart的目录
BOOL CPF::CreateFullDirecory(LPACECTSTR full_pathname,size_t offset)
{
	//备份当前路径
	ACE_TCHAR bkCurPath[MAX_PATH];

	ACE_OS::getcwd(bkCurPath,MAX_PATH);

	ACE_TCHAR cur_path[MAX_PATH];
	ACE_OS::strcpy(cur_path,full_pathname);
	CPF::AddLastFileNameSep(cur_path);

	ACE_TCHAR* p = (ACE_TCHAR *)cur_path+offset;
	ACE_TCHAR* q;

	while (p[0])
	{
		ACE_TCHAR * q1 = strchr(p, FILENAME_SEPERATOR);

#ifdef OS_WINDOWS
		ACE_TCHAR * q2 = strchr(p, LINUX_FILENAME_SEPERATOR);

		if( !q1 && !q2 )
			q = NULL;
		else if( q1 && q2 )
			q = q1 > q2 ? q2 : q1;
		else if( q1 )
			q = q1;
		else 
			q = q2;
#else
		q = q1;
#endif

		if (q != NULL)
		{
			*q = '\0';
		}

#ifdef OS_WINDOWS
		if( ACE_OS::strlen(cur_path) == 2 )
#else 
		if(	ACE_OS::strlen(cur_path) == 0 )
#endif
		{
			ACE_OS::chdir(cur_path);
		}
		else
		{
			if( ACE_OS::mkdir(cur_path,0777) != 0 )
			{
#ifdef OS_WINDOWS
				DWORD dwErr = GetLastError();
				if (dwErr != ERROR_ALREADY_EXISTS)
				{
					return FALSE;					
				}
#else
				if( errno != EEXIST )
					return FALSE;
#endif
			}

			chdir(cur_path);

			if (q == NULL)
				break;
		}

		*q = FILENAME_SEPERATOR;
		p = q + 1;
	}

	//恢复当前路径
	ACE_OS::chdir(bkCurPath);

	return TRUE;
}

