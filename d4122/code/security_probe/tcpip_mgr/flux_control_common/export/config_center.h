//////////////////////////////////////////////////////////////////////////
//config_center.h

#pragma once

#include "flux_control_common.h"

class Cconfig_center;

FLUX_CONTROL_COMMON_API bool ConfigCenter_Initialize();
FLUX_CONTROL_COMMON_API void ConfigCenter_UnInitialize();

FLUX_CONTROL_COMMON_API Cconfig_center* ConfigCenter_Obj();


class FLUX_CONTROL_COMMON_CLASS Cconfig_center
{
public:
	Cconfig_center();

public:
	BOOL init();

public:
	std::string GetProbeFullConfigName(const char * short_name) const;

public:
	std::string		m_prg_base_path;//c:/ts/ts_prg,对于linux，/home/ts/ts_prg
	std::string		m_cfg_base_path;//c:/ts/ts_config,对于linux，/home/ts/ts_config

public:
	std::string		m_prg_base_path2;//c:/ts/ts_prg/probe,对于linux，/home/ts/ts_prg/probe

public:
	std::string		m_str_probe_config_path;//$(m_prg_base_path)/probe_config


};