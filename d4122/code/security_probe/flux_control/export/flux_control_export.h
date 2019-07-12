//////////////////////////////////////////////////////////////////////////
//flux_control_export.h

#pragma once

#include "cpf/ostypedef.h"

#if defined(FLUX_CONTROL_EXPORTS)
#define FLUX_CONTROL_API		MPLAT_API_Export_Flag
#define FLUX_CONTROL_CLASS		MPLAT_CLASS_Export_Flag
#else
#define FLUX_CONTROL_API		MPLAT_API_Import_Flag
#define FLUX_CONTROL_CLASS		MPLAT_CLASS_Import_Flag
#endif

typedef struct tagSTAT_TABLE_INFO
{
	tagSTAT_TABLE_INFO()
	{
		row_nums = 0;
		total_nums = 0;
		vm_id = -1;
	}

	std::string tab_name;
	ACE_UINT32  vm_id;
	int			row_nums;
	int			total_nums;

}STAT_TABLE_INFO;


