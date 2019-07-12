//////////////////////////////////////////////////////////////////////////
//flux_control_common.h

#pragma once

#include "cpf/ostypedef.h"

#if defined(FLUX_CONTROL_COMMON_EXPORTS)
#define FLUX_CONTROL_COMMON_API			MPLAT_API_Export_Flag
#define FLUX_CONTROL_COMMON_CLASS		MPLAT_CLASS_Export_Flag
#else
#define FLUX_CONTROL_COMMON_API			MPLAT_API_Import_Flag
#define FLUX_CONTROL_COMMON_CLASS		MPLAT_CLASS_Import_Flag
#endif

