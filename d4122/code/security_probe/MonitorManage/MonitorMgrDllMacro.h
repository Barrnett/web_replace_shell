#ifndef MONITOR_MANAGER_DLL_MACRO_H_
#define MONITOR_MANAGER_DLL_MACRO_H_

#include "cpf/ostypedef.h"

#if defined(MONITORMANAGE_EXPORTS)
#define MONITOR_MANAGER_API		MPLAT_API_Export_Flag
#define MONITOR_MANAGER_CLASS	MPLAT_CLASS_Export_Flag
#else
#define MONITOR_MANAGER_API		MPLAT_API_Import_Flag
#define MONITOR_MANAGER_CLASS	MPLAT_CLASS_Import_Flag
#endif

#endif