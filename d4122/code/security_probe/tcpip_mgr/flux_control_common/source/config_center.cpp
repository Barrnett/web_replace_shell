// config_center.cpp : Defines the exported functions for the DLL application.
//

#include "StdAfx.h"
#include "config_center.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/path_tools.h"


std::string Cconfig_center::GetProbeFullConfigName(const char * short_name) const
{
	return std::string(CPF::JoinPathToPath(m_str_probe_config_path.c_str(),short_name));
}

Cconfig_center::Cconfig_center()
{
	return;
}

BOOL Cconfig_center::init()
{
#ifdef OS_WINDOWS
	m_prg_base_path = "c:/ts/ts_prg/";
	m_cfg_base_path = "c:/ts/ts_config/";
#else
	m_prg_base_path = "/home/ts/ts_prg/";
	m_cfg_base_path = "/home/ts/ts_config/";
#endif

	m_prg_base_path2 = CPF::JoinPathToPath(m_prg_base_path.c_str(), "probe/");
	
	CPF::CreateFullDirecory(m_prg_base_path.c_str());
	CPF::CreateFullDirecory(m_cfg_base_path.c_str());

	//m_str_probe_config_path="c/ts/ts_config/probe_config/"
	m_str_probe_config_path = CPF::JoinPathToPath(m_cfg_base_path.c_str(), "probe_config/");

	CPF::CreateFullDirecory(m_str_probe_config_path.c_str());

	return true;
}

static Cconfig_center*	g_config_center = NULL;
static bool				g_config_Inited =	false;

bool ConfigCenter_Initialize()
{
	if (g_config_Inited)
		return true;

	g_config_Inited = true;

	g_config_center = new Cconfig_center;

	g_config_center->init();

	return true;
}

void ConfigCenter_UnInitialize()
{
	if( g_config_center )
	{
		delete g_config_center;
		g_config_center = NULL;
	}

	g_config_Inited = false;

	return;
}

Cconfig_center* ConfigCenter_Obj()
{
	if (!g_config_Inited)
	{
		ACE_ASSERT(false);
		return NULL;
	}

	return g_config_center;
}
