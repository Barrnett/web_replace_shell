#ifndef _PATH_TOOLS_H_ZHUZP_2006_3_11
#define _PATH_TOOLS_H_ZHUZP_2006_3_11
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"

namespace CPF
{
	//�ж�һ�����ļ�����·��
	CPF_EXPORT
	BOOL IsDirectory(LPACECTSTR path);

	CPF_EXPORT
	ACE_TCHAR * GetLastFileNameSeperator(LPACECTSTR strPathFile);


	//Ŀ¼�������ļ�����Ŀ¼,
	//bdelete_self=true,dirnameĿ¼����Ҳɾ��������dirnameĿ¼����ɾ��
	CPF_EXPORT
	BOOL DeleteDirectory(LPACECTSTR dirname,BOOL bdelete_self,ACE_SCANDIR_SELECTOR selector = NULL);

	//������Ŀ¼�е������ļ����ļ���
	CPF_EXPORT
	BOOL CopySubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector = NULL);

	//��������Ŀ¼
	CPF_EXPORT
	BOOL CopyThisandSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector = NULL);

	//�ƶ���Ŀ¼�е������ļ����ļ���
	CPF_EXPORT
	BOOL MoveSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector = NULL);

	//�ƶ�����Ŀ¼
	CPF_EXPORT
	BOOL MoveThisandSubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,ACE_SCANDIR_SELECTOR selector = NULL);

	//����ǿ�������sourcedirname������ļ���Ŀ¼ȫ��������destdirname����(����������)��
	//removeflag��ʾ���ƶ�Ŀ¼����ɾ��Ŀ¼.
	CPF_EXPORT
	BOOL MoveOrCopySubDirectory(LPACECTSTR soucedirname,LPACECTSTR destdirname,BOOL removeflag,ACE_SCANDIR_SELECTOR selector = NULL);

	//����ǿ�������sourcedirname�����Լ�������ļ���Ŀ¼ȫ��������destdirname����
	//removeflag��ʾ���ƶ�Ŀ¼����ɾ��Ŀ¼.
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

	//��pName�Ƿ���ȫ·����������ǣ�����ϵ�ǰ�����ִ��Ŀ¼
	CPF_EXPORT
	LPACECTSTR TransToFullPathName(LPACETSTR outbuf,size_t bufsize,LPACECTSTR pName);

	CPF_EXPORT
		BOOL CreateFullDirecory(LPACECTSTR fullpathname,size_t offset=0);

	CPF_EXPORT
		//��lpszPathORFileName��ӵ�lpBuffer�У��Զ�ȥ�ض��ڵ�·���ָ���
		//���� lpBuffer = "c:\aa\"  lpszPathORFileName="\bb\cc\"
		//���Ľ���� "c:\aa\bb\cc\"
	LPACECTSTR AddToPath(LPACETSTR lpBuffer, LPACECTSTR lpszPathORFileName);

	CPF_EXPORT
		LPACECTSTR JoinPathToPath(LPACETSTR lpBuffer, size_t nBufSize,
			LPACECTSTR lpszPath, LPACECTSTR lpszPathORFileName);

	//��һ�������ļ�����,�õ������ļ���֮���ȫ·����,���������һ�� '\\'
	CPF_EXPORT
		LPACECTSTR GetPathNameFromFullName(LPACETSTR fullName, size_t nSizeFullName,LPACECTSTR strPathFile);

	//��һ�������ļ�����,�õ�û��·�����ļ���,
	//�õ���ָ�뻹��ָ��strPathFile���������
	CPF_EXPORT
		LPACECTSTR GetFileNameFromFullName(LPACECTSTR strPathFile);

	//�滻���ļ����ĺ�׺�����suf=NULL����ʾ��Ҫ��׺��
	//�����ǰû�к�����Ϻ�׺
	CPF_EXPORT
		void ReplaceSufFromFileName(LPACECTSTR strPathFile,const char * suf,std::string& newfilename);

	//�õ��ļ��ĺ�׺���Ƶ�ָ�룬���û�к�׺����NULL��
	//����õ������ص�ָ����strPathFile���ڲ���
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

	//�õ���ǰִ�г����Ŀ¼���������һ��'\\'

	CPF_EXPORT
		LPACECTSTR GetModulePathName(LPACETSTR fullName, size_t nSizeFullName);


	CPF_EXPORT 
		//�õ���ǰִ���ļ�����ȫ��
		LPACECTSTR GetModuleFileName(LPACETSTR fullName, size_t nSizeFullName);


	CPF_EXPORT 
		LPACECTSTR GetModuleFullFileName(LPACETSTR fullName, size_t nSizeFullName, LPACECTSTR fileName);

	//��������ַ�����'\'����'/',���ϡ��û�Ҫ��֤�������㹻
	//����������һ�����ϣ��򷵻�true�����򷵻�false
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

	//�õ�Ӧ�ó���Ŀ¼�µ��¼��ļ�����ȫ·����
	//����õ�\\ini_ch\all.ini:pPathΪini_ch��fileNameΪAll.ini
	//������pPath����ǰ���'\'��Ҳ���Բ�����
	//fileName������ǰ������߲���'\'	
	CPF_EXPORT
		LPACECTSTR GetModuleOtherFileName(LPACETSTR fullName, size_t nSizeFullName,LPACECTSTR pPath,LPACECTSTR fileName);

#ifdef _WINDOWS_
	CPF_EXPORT
		LPACECTSTR GetWindowsSystem32Directory(LPACETSTR directoryBuffer, size_t nSize);

	CPF_EXPORT
		LPACECTSTR GetWindowsSystem32FilePath(LPACETSTR directoryBuffer, size_t nSize, LPACECTSTR filename);
#endif//_WINDOWS_

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////�����߳�ר��
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CPF_EXPORT
		LPACECTSTR JoinPathToPath(LPACECTSTR lpszPath, LPACECTSTR lpszPathORFileName);

	CPF_EXPORT
		LPACECTSTR GetPathNameFromFullName(LPACECTSTR strPathFile);
	
	CPF_EXPORT 
		LPACECTSTR GetModulePathName();

	CPF_EXPORT 
		//�õ���ǰִ���ļ�����ȫ��
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


