//////////////////////////////////////////////////////////////////////////
//cpf2.h

#include "cpf/ostypedef.h"

#if defined(CPF2_DLL_EXPORTS)
#define CPF2_API		MPLAT_API_Export_Flag
#define CPF2_CLASS		MPLAT_CLASS_Export_Flag
#else
#define CPF2_API		MPLAT_API_Import_Flag
#define CPF2_CLASS		MPLAT_CLASS_Import_Flag
#endif

