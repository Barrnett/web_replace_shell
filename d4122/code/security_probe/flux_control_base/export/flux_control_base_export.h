//////////////////////////////////////////////////////////////////////////
//flux_control_base_export.h

#pragma once

#include "cpf/ostypedef.h"

#if defined(FLUX_CONTROL_BASE_EXPORTS)
#define FLUX_CONTROL_BASE_API		MPLAT_API_Export_Flag
#define FLUX_CONTROL_BASE_CLASS		MPLAT_CLASS_Export_Flag
#else
#define FLUX_CONTROL_BASE_API		MPLAT_API_Import_Flag
#define FLUX_CONTROL_BASE_CLASS	MPLAT_CLASS_Import_Flag
#endif

#define DIAL_MGR_CLASS				FLUX_CONTROL_BASE_CLASS

