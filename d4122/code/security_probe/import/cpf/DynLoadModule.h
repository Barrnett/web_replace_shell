//////////////////////////////////////////////////////////////////////////
//DynLoadModule.h

#ifndef _DYN_LOAD_MODULE_H_2006_05_13
#define _DYN_LOAD_MODULE_H_2006_05_13
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"
#include "cpf/other_tools.h"
#include "cpf/path_tools.h"
#include <vector>
#include <utility>
#include "cpf/IniFile.h"
#include "cpf/TinyXmlEx.h"


//动态库提供三个函数:
//(1)bool DLLNAME_Init(const INIT_PARAM_TYPE& param);(2) void DLLNAME_Close();(3)T* DLLNAME_GetWorkInstance()

//每个dll库需要输出的接口:假设输出的dll的名字为:MyTest.dll

//在DLL被加载的时候调用,应该在这里生成CBaseObserver的对象.这个函数只会被调用一次
//bool MYTEST_Init();/

//在DLL被卸载之前调用,应该在这里销毁生成的CBaseObserver的对象.这个函数只会被调用一次
//void MYTEST_Close();

//返回生成的T的对象的指针,可能被调用多次.
//T * MYTEST_GetWorkInstance();

//注意: MYTEST是dll名字的全大写.
//(因为windows下不区分文件名的大小写,但是c函数区分,为了减少出错,因此统一将函数的名称变成大写)

//INIT_PARAM_TYPE调用 DLLNAME_Init传入的参数
template<typename T,typename INIT_PARAM_TYPE>
class CDynLoadModuleInfo
{
public:
	STD_TString			m_strDllName;
	ACE_SHLIB_HANDLE	m_hDll;
	T*					m_pInstance;
	STD_TString			m_strParam;

	CDynLoadModuleInfo()
	{
		m_hDll = ACE_SHLIB_INVALID_HANDLE;
		m_pInstance = NULL;
	}

	typedef bool (*PFN_DLLINIT)(const INIT_PARAM_TYPE& param);
	typedef void (*PFN_DLLCLOSE)(void);
	typedef T* (*PFN_GETWORKINSTANCE)();

	//minfo作为输入，应该有m_strDllName。
	bool LoadModule(const ACE_TCHAR * pname,const ACE_TCHAR * dllparam,const INIT_PARAM_TYPE& param);
	void UnLoadModuleModule();

};

template<typename T,typename INIT_PARAM_TYPE>
bool CDynLoadModuleInfo<T,INIT_PARAM_TYPE>::LoadModule(const ACE_TCHAR * pname,const ACE_TCHAR * dllparam,const INIT_PARAM_TYPE& param)
{
	if( pname == NULL || pname[0] == 0 )
		return false;

	m_strDllName = pname;

	if( dllparam )
	{
		m_strParam = dllparam;
	}

	ACE_TCHAR dllname[MAX_PATH];

	CPF::get_dll_name(dllname,pname);

	m_hDll = ACE_OS::dlopen(dllname);
	if (ACE_SHLIB_INVALID_HANDLE == m_hDll)
	{
		return false;
	}

	ACE_TCHAR funcPrefix[64];

	if( CPF::IsFullPathName(m_strDllName.c_str()) )
	{
		ACE_OS::strcpy(funcPrefix,CPF::GetFileNameFromFullName(m_strDllName.c_str()));
	}
	else
	{
		ACE_OS::strcpy(funcPrefix, m_strDllName.c_str());
	}

	CPF::str_upr(funcPrefix);
	ACE_OS::strcat(funcPrefix, ACE_TEXT("_"));

	ACE_TCHAR funcName[128];

	ACE_OS::strcpy(funcName, funcPrefix);
	ACE_OS::strcat(funcName, ACE_TEXT("Init"));

	//call FLUX_CONTROL_Init()
	PFN_DLLINIT pfnDllInit = (PFN_DLLINIT)ACE_OS::dlsym(m_hDll, funcName);

	if (NULL == pfnDllInit)
	{
		ACE_OS::dlclose(m_hDll);
		m_hDll = ACE_SHLIB_INVALID_HANDLE;
		return false;
	}
	if (!(*pfnDllInit)(param))
	{
		ACE_OS::dlclose(m_hDll);
		m_hDll = ACE_SHLIB_INVALID_HANDLE;
		return false;
	}

	ACE_OS::strcpy(funcName, funcPrefix);
	ACE_OS::strcat(funcName, ACE_TEXT("GetWorkInstance"));
	//call FLUX_CONTROL_GetWorkInstance()
	PFN_GETWORKINSTANCE pfnGetWorkInstance = (PFN_GETWORKINSTANCE)ACE_OS::dlsym(m_hDll, funcName);
	if (NULL == pfnGetWorkInstance)
	{
		ACE_OS::dlclose(m_hDll);
		m_hDll = ACE_SHLIB_INVALID_HANDLE;
		return false;
	}
	m_pInstance = (*pfnGetWorkInstance)();
	if (NULL == m_pInstance)
	{
		ACE_OS::dlclose(m_hDll);
		m_hDll = ACE_SHLIB_INVALID_HANDLE;
		return false;
	}

	return true;
}

template<typename T,typename INIT_PARAM_TYPE>
void CDynLoadModuleInfo<T,INIT_PARAM_TYPE>::UnLoadModuleModule()
{
	if( ACE_SHLIB_INVALID_HANDLE != m_hDll )
	{
		ACE_TCHAR funcPrefix[64];

		if( CPF::IsFullPathName(m_strDllName.c_str()) )
		{
			ACE_OS::strcpy(funcPrefix,CPF::GetFileNameFromFullName(m_strDllName.c_str()));
		}
		else
		{
			ACE_OS::strcpy(funcPrefix, m_strDllName.c_str());
		}

		CPF::str_upr(funcPrefix);
		ACE_OS::strcat(funcPrefix, ACE_TEXT("_"));

		ACE_TCHAR funcName[128];

		ACE_OS::strcpy(funcName, funcPrefix);
		ACE_OS::strcat(funcName, ACE_TEXT("Close"));

		PFN_DLLCLOSE pfnDllClose = (PFN_DLLCLOSE)ACE_OS::dlsym(m_hDll, funcName);

		if (NULL != pfnDllClose)
		{
			(*pfnDllClose)();
		}

		ACE_OS::dlclose(m_hDll);
	}

	m_hDll = ACE_SHLIB_INVALID_HANDLE;
	m_pInstance = NULL;

	return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<typename T,typename INIT_PARAM_TYPE>
class CDynLoadModule
{
public:
	int LoadFromIniFile(const ACE_TCHAR * filename,
		const ACE_TCHAR * sectionname,
		const INIT_PARAM_TYPE& param);

	int LoadFromXmlFile(const ACE_TCHAR * filename,
		const ACE_TCHAR * list_node_name,
		const INIT_PARAM_TYPE& param);

	void Close();

	int GetModuleNums() const
	{
		return (int)m_vtModuleInfo.size();
	}

	CDynLoadModuleInfo<T,INIT_PARAM_TYPE>* GetModuleInfo(int index) const
	{
		return m_vtModuleInfo[index];
	}

	const std::vector< std::pair<std::string,std::string> >& GetFailedInfo() const
	{
		return m_vt_failedInfo;
	}

public:
	int LoadDlls(const std::vector<STD_TString>& list_dll,
		const std::vector<STD_TString>&	list_name,
		const INIT_PARAM_TYPE& param);

	//first=dll,second=name
	int LoadDlls( const std::vector<std::pair<STD_TString,STD_TString> >& list_dll_name,
		const INIT_PARAM_TYPE& param);

private:
	std::vector< CDynLoadModuleInfo<T,INIT_PARAM_TYPE> * >		m_vtModuleInfo;

	std::vector< std::pair<std::string,std::string> >			m_vt_failedInfo;
};

template<typename T,typename INIT_PARAM_TYPE>
int CDynLoadModule<T,INIT_PARAM_TYPE>::LoadFromIniFile(const ACE_TCHAR * filename,
													   const ACE_TCHAR * sectionname,
													   const INIT_PARAM_TYPE& param)
{
	CIniFile ini;

	if( !ini.Open(filename) )
		return 0;

	std::vector<STD_TString>	list_dll;
	std::vector<STD_TString>	list_name;

	if( ini.GetItemsInSection(sectionname,list_dll,list_name) == 0 )
	{
		ini.Close();
		return 0;
	}

	return LoadDlls(list_dll,list_name,param);
}

template<typename T,typename INIT_PARAM_TYPE>
int CDynLoadModule<T,INIT_PARAM_TYPE>::LoadFromXmlFile(const ACE_TCHAR * filename,
													   const ACE_TCHAR * list_node_name,													   
													   const INIT_PARAM_TYPE& param)
{
	CTinyXmlEx reader;

	if( 0 != reader.open(filename) )
		return 0;
	
	TiXmlNode * list_node = reader.GetRootNode()->FirstChildElement(list_node_name);

	if( !list_node_name )
		return 0;

	std::vector<STD_TString>	list_dll;
	std::vector<STD_TString>	list_name;

	for( TiXmlNode * node = list_node->FirstChildElement("observer");
		node;
		node = node->NextSiblingElement("observer") )
	{
		int onoff = 1;
		reader.GetAttr(node,"onoff",onoff);

		if( onoff == 0 )
			continue;

		std::string desc;
		reader.GetAttr(node,"desc",desc);

		std::string dllname;
		
		if( !reader.GetValue(node,dllname) )
			continue;

		list_dll.push_back(dllname);
		list_name.push_back(desc);
	}

	return LoadDlls(list_dll,list_name,param);
}


template<typename T,typename INIT_PARAM_TYPE>
int CDynLoadModule<T,INIT_PARAM_TYPE>::LoadDlls(
			const std::vector<STD_TString>& list_dll,
			const std::vector<STD_TString>&	list_name,
			const INIT_PARAM_TYPE& param)
{
	int nums = 0;

	for (size_t i = 0; i < list_dll.size(); ++i )
	{
		CDynLoadModuleInfo<T,INIT_PARAM_TYPE>* pObserverInfo =
			new CDynLoadModuleInfo<T,INIT_PARAM_TYPE>;

		if( pObserverInfo->LoadModule(list_dll[i].c_str(),list_name[i].c_str(),param) )
		{
			m_vtModuleInfo.push_back(pObserverInfo);

			++nums;
		}
		else
		{
			delete pObserverInfo;

			m_vt_failedInfo.push_back(std::make_pair(list_dll[i],list_name[i]));
		}
	}

	return nums;
}


template<typename T,typename INIT_PARAM_TYPE>
int CDynLoadModule<T,INIT_PARAM_TYPE>::LoadDlls(
	const std::vector< std::pair<STD_TString,STD_TString> >& list_dll_name,
	const INIT_PARAM_TYPE& param)
{
	int nums = 0;

	for (size_t i = 0; i < list_dll_name.size(); ++i )
	{
		CDynLoadModuleInfo<T,INIT_PARAM_TYPE>* pObserverInfo =
			new CDynLoadModuleInfo<T,INIT_PARAM_TYPE>;

		if( pObserverInfo->LoadModule(list_dll_name[i].first.c_str(),
			list_dll_name[i].second.c_str(),param) )
		{
			m_vtModuleInfo.push_back(pObserverInfo);

			++nums;
		}
		else
		{
			delete pObserverInfo;

			m_vt_failedInfo.push_back(list_dll_name[i]);
		}
	}

	return nums;
}


template<typename T,typename INIT_PARAM_TYPE>
void CDynLoadModule<T,INIT_PARAM_TYPE>::Close()
{
#ifdef OS_LINUX
	typename std::vector<CDynLoadModuleInfo<T, INIT_PARAM_TYPE>*,std::allocator<CDynLoadModuleInfo<T, INIT_PARAM_TYPE>*> >::iterator it;
#else	
	std::vector<CDynLoadModuleInfo<T,INIT_PARAM_TYPE> *>::iterator it;
#endif
	for (it = m_vtModuleInfo.begin();it != m_vtModuleInfo.end(); ++it)
	{
		(*it)->UnLoadModuleModule();
		delete (*it);
	}

	m_vtModuleInfo.clear();

	m_vt_failedInfo.clear();

	return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_DYN_LOAD_MODULE_H_2006_05_13
