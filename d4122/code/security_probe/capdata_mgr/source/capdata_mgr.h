//////////////////////////////////////////////////////////////////////////
//capdata_mgr.h

#pragma once

#include "cpf/ostypedef.h"

#if defined(CAPDATA_MGR_EXPORTS)
#define CAPDATA_MGR_API		MPLAT_API_Export_Flag
#define CAPDATA_MGR_CLASS	MPLAT_CLASS_Export_Flag
#else
#define CAPDATA_MGR_API		MPLAT_API_Import_Flag
#define CAPDATA_MGR_CLASS	MPLAT_CLASS_Import_Flag
#endif

