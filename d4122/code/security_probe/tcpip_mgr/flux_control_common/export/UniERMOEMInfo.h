//////////////////////////////////////////////////////////////////////////
//UniERMOEMInfo.h

#pragma once

#include "string"
#include "flux_control_common.h"

class FLUX_CONTROL_COMMON_CLASS CUniERMOEMInfo
{
public:
	CUniERMOEMInfo(void);
	~CUniERMOEMInfo(void);

public:
	std::string	m_str_oem_name;//用于标识一个公司产品的名称

	std::string m_str_brand;
	std::string m_str_product_name;
	std::string m_company_name_ch;
	std::string m_company_name_en;
	std::string m_company_home_url;
	std::string m_product_help_url;
	std::string m_product_brief_url;
	std::string m_str_desktop_name;

public:
	std::string	GetFullProductName() const;
	std::string GetDesktopNameEx() const;
	BOOL ReadFromXml(const char * xml_file_name);

};
