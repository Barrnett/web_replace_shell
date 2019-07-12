//////////////////////////////////////////////////////////////////////////
//fc_server_export.h

#pragma once

#include "cpf/ostypedef.h"

#if defined(FCSERVERFACADE_EXPORTS)
#define FCSERVERFACADE_API			MPLAT_API_Export_Flag
#define FCSERVERFACADE_CLASS		MPLAT_CLASS_Export_Flag
#else
#define FCSERVERFACADE_API			MPLAT_API_Import_Flag
#define FCSERVERFACADE_CLASS		MPLAT_CLASS_Import_Flag
#endif

